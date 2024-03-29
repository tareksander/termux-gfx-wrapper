#include "egl.hpp"

#include <unistd.h>
#include <sys/mman.h>
#include <sys/fcntl.h>
#include <sys/utsname.h>
#include <memory.h>

#ifdef __ANDROID__
#include <linux/ashmem.h>
#include <sys/ioctl.h>

static inline int memfd_create(const char *name, unsigned int flags) {
#ifndef __NR_memfd_create
    #if defined __i386__
#define __NR_memfd_create 356
#elif defined __x86_64__
#define __NR_memfd_create 319
#elif defined __arm__
#define __NR_memfd_create 385
#elif defined __aarch64__
#define __NR_memfd_create 279
#endif
#endif
#ifdef __NR_memfd_create
    return syscall(__NR_memfd_create, name, flags);
#else
    errno = ENOSYS;
	return -1;
#endif
}
#endif

#include "format.hpp"

#include <chrono>

static inline int
os_create_anonymous_file(size_t size) {
    int fd, ret = -1;
    long flags;
    
    // this should be used according to google: https://android.googlesource.com/platform/art/+/master/libartbase/base/memfd.cc#51
    static bool memfd_available = [] {
        //dprintf(2, "memfd available\n");
        struct utsname uts;
        if (uname(&uts) != 0)
            return false;
        int major, minor;
        if (sscanf(uts.release, "%d.%d", &major, &minor) != 2 || major < 3 || (major == 3 && minor < 17))
            return false;
        return true;
    }();
    
    if (memfd_available) {
        fd = memfd_create("xorg", MFD_CLOEXEC|MFD_ALLOW_SEALING);
        if (fd != -1) {
            fcntl(fd, F_ADD_SEALS, F_SEAL_SHRINK);
            if (ftruncate(fd, size) < 0) {
                close(fd);
            } else {
                //dprintf(2, "Using memfd\n");
                return fd;
            }
        }
    } else {
        #ifdef __ANDROID__
            fd = open("/dev/ashmem", O_RDWR | O_CLOEXEC);
            if (fd < 0)
                return fd;
            ret = ioctl(fd, /** ASHMEM_SET_SIZE */ _IOW(0x77, 3, size_t), size);
            if (ret < 0)
                goto err;
            flags = fcntl(fd, F_GETFD);
            if (flags == -1)
                goto err;
            if (fcntl(fd, F_SETFD, flags | FD_CLOEXEC) == -1)
                goto err;

            //dprintf(2, "Using ashmem\n");
            return fd;
            err:
            close(fd);
        #endif
    }
    return ret;
}

static __thread xcb_generic_error_t* err = nullptr;

namespace egl_wrapper {
    void X11Display::checkExtensions() const {
        // check for the shm extension
        {
            xcb_shm_query_version_reply_t* shmv = xcb_shm_query_version_reply(xcbC, xcb_shm_query_version(xcbC), &err);
            if (err != nullptr) {
                //dprintf(2, "shm error\n");
                free(shmv);
                free(err);
                throw Exceptions::NoDisplayException();
            }
            if (shmv->major_version != 1 || shmv->minor_version < 2 || shmv->shared_pixmaps == false) {
                //dprintf(2, "shm version\n");
                free(shmv);
                throw Exceptions::NoDisplayException();
            }
            free(shmv);
        }
        // check for present extension
        {
            xcb_present_query_version_reply_t* presv = xcb_present_query_version_reply(xcbC, xcb_present_query_version(xcbC, XCB_PRESENT_MAJOR_VERSION, XCB_PRESENT_MINOR_VERSION), &err);
            if (err != nullptr) {
                //dprintf(2, "present error\n");
                free(presv);
                free(err);
                throw Exceptions::NoDisplayException();
            }
            if (presv->major_version != 1 || presv->minor_version < 2) {
                //dprintf(2, "present version\n");
                free(presv);
                throw Exceptions::NoDisplayException();
            }
            free(presv);
        }
    }
    
    X11Display::X11Display(Display* x11Display, const EGLAttrib *attrib_list) : EGLDisplayBackend(DisplayType::X11) {
        //dprintf(2, "xlib\n");
        if (x11Display == EGL_DEFAULT_DISPLAY) {
            //dprintf(2, "default\n");
            screenIndex = -1;
            if (attrib_list != nullptr) {
                if (*attrib_list != EGL_PLATFORM_X11_SCREEN_EXT && *attrib_list != EGL_NONE) throw Exceptions::BadAttributeException();
                attrib_list++;
                screenIndex = int(*attrib_list);
                attrib_list++;
                if (*attrib_list != EGL_NONE) throw Exceptions::BadAttributeException();
            }
            int* screenp = &screenIndex;
            if (screenIndex == -1) {
                screenIndex = 0;
            }
            this->xcbC = xcb_connect(nullptr, screenp);
            if (xcb_connection_has_error(this->xcbC) != 0) {
                xcb_disconnect(this->xcbC);
                throw Exceptions::NoDisplayException();
            }
            this->ownsConnection = true;
        } else {
            screenIndex = 0;
            this->ownsConnection = false;
            this->xcbC = XGetXCBConnection(x11Display);
        }
        //dprintf(2, "check\n");
        checkExtensions();
    }
    
    X11Display::X11Display(xcb_connection_t* xcbC, const EGLAttrib *attrib_list) : EGLDisplayBackend(DisplayType::X11) {
        //dprintf(2, "xcb\n");
        if (xcbC == (void*)EGL_DEFAULT_DISPLAY) {
            screenIndex = -1;
            if (attrib_list != nullptr) {
                if (*attrib_list != EGL_PLATFORM_XCB_SCREEN_EXT && *attrib_list != EGL_NONE) throw Exceptions::BadAttributeException();
                attrib_list++;
                screenIndex = int(*attrib_list);
                attrib_list++;
                if (*attrib_list != EGL_NONE) throw Exceptions::BadAttributeException();
            }
            int* screenp = &screenIndex;
            if (screenIndex == -1) {
                screenIndex = 0;
            }
            this->xcbC = xcb_connect(nullptr, screenp);
            if (xcb_connection_has_error(this->xcbC) != 0) {
                xcb_disconnect(this->xcbC);
                throw Exceptions::NoDisplayException();
            }
            this->ownsConnection = true;
        } else {
            screenIndex = 0;
            this->ownsConnection = false;
            this->xcbC = xcbC;
        }
        checkExtensions();
    }

    X11Display::~X11Display() {
        eglTerminate();
        if (ownsConnection) {
            xcb_disconnect(xcbC);
            ownsConnection = false;
        }
        xcbC = nullptr;
    }

    EGLBoolean X11Display::eglChooseConfig(const EGLint* attrib_list, EGLConfig* configs, EGLint config_size, EGLint* num_config) {
        auto retVal = real_eglChooseConfig(nativeDisplay, attrib_list, configs, config_size, num_config);
        lastError = real_eglGetError();
        return retVal;
    }

    EGLBoolean X11Display::eglCopyBuffers(EGLSurface surface, EGLNativePixmapType target) {
        return EGL_FALSE;
    }
    
    EGLContext X11Display::eglCreateContext(EGLConfig config, EGLContext share_context, const EGLint* attrib_list) {
        const EGLint* attrib = attrib_list;
        if (attrib != nullptr) {
            while (*attrib != EGL_NONE) {
                if (*attrib == EGL_CONTEXT_MAJOR_VERSION && *(attrib + 1) == 2) {
                    EGLContext ctx = real_eglCreateContext(nativeDisplay, config, share_context, attrib_list);
                    lastError = real_eglGetError();
                    if (ctx != EGL_NO_CONTEXT) {
                        auto c = new Context(EGL_OPENGL_ES_API, ctx);
                        contexts.insert(c); // could leak memory for OOM
                        return c;
                    }
                    return EGL_NO_CONTEXT;
                }
                attrib++;
            }
        }
        lastError = EGL_BAD_ATTRIBUTE;
        return EGL_NO_CONTEXT;
    }

    EGLSurface X11Display::eglCreatePbufferSurface(EGLConfig config, const EGLint* attrib_list) {
        EGLSurface ret = real_eglCreatePbufferSurface(nativeDisplay, config, attrib_list);
        lastError = real_eglGetError();
        if (ret != EGL_NO_SURFACE) {
            std::unique_ptr<Surface> s = std::make_unique<Surface>();
            s->type = Surface::Type::PBUFFER;
            auto backend = std::make_unique<PBufferSurfaceBackend>();
            backend->pbuffer = ret;
            s->backend = std::move(backend);
            Surface* sraw = s.release();
            surfaces.insert(sraw); // could leak memory for OOM
            return sraw;
        }
        return EGL_NO_SURFACE;
    }

    EGLSurface X11Display::eglCreatePixmapSurface(EGLConfig config, EGLNativePixmapType pixmap, const EGLint* attrib_list) {
        return EGL_NO_SURFACE;
    }

    EGLSurface X11Display::eglCreateWindowSurface(EGLConfig config, EGLNativeWindowType win, const EGLint* attrib_list) {
        std::unique_ptr<WindowSurface> s = std::make_unique<WindowSurface>();
        s->type = Surface::Type::WINDOW;
        s->xcbC = xcbC;
        s->w = win;
        s->conf = config;
        if (hwbufferDMABUFAvailable) {
            auto backend = std::make_unique<HardwareBufferSurfaceBackend>();
            //backend->width = 1;
            //backend->height = 1;
            EGLint pbattribs[] = {
                EGL_WIDTH, 1,
                EGL_HEIGHT, 1,
                EGL_NONE};
            backend->dummy = real_eglCreatePbufferSurface(nativeDisplay, config, pbattribs);
            if (backend->dummy == EGL_NO_SURFACE) return EGL_NO_SURFACE;
            s->backend = std::move(backend);
        } else {
            auto backend = std::make_unique<PBufferSurfaceBackend>();
            backend->width = 1;
            backend->height = 1;
            EGLint pbattribs[] = {
                EGL_WIDTH, 1,
                EGL_HEIGHT, 1,
                EGL_NONE};
            backend->pbuffer = real_eglCreatePbufferSurface(nativeDisplay, config, pbattribs);
            if (backend->pbuffer == EGL_NO_SURFACE) return EGL_NO_SURFACE;
            s->backend = std::move(backend);
        }
        s->eid = xcb_generate_id(xcbC);
        s->ev = xcb_register_for_special_xge(xcbC, &xcb_present_id, s->eid, nullptr);
        if ((err = xcb_request_check(xcbC, xcb_present_select_input_checked(xcbC, s->eid, s->w, XCB_PRESENT_EVENT_MASK_IDLE_NOTIFY | XCB_PRESENT_EVENT_MASK_COMPLETE_NOTIFY | XCB_PRESENT_EVENT_MASK_CONFIGURE_NOTIFY)))) {
            dprintf(2, "Failed to xcb_present_select_input_checked()\n");
            free(err);
            return EGL_NO_SURFACE;
        }
        xcb_get_geometry_reply_t* geo = xcb_get_geometry_reply(xcbC, xcb_get_geometry(xcbC, s->w), &err);
        if (err) {
            dprintf(2, "Failed to xcb_get_geometry()\n");
            free(err);
            return EGL_NO_SURFACE;
        }
        s->wWidth = geo->width;
        s->wHeight = geo->height;
        free(geo);
        Surface* sraw = s.release();
        surfaces.insert(sraw); // could leak memory for OOM
        return sraw;
    }
    
    EGLBoolean X11Display::eglDestroyContext(EGLContext ctx) {
        if (ctx == EGL_NO_CONTEXT) {
            lastError = EGL_BAD_CONTEXT;
            return EGL_FALSE;
        }
        auto c = reinterpret_cast<Context*>(ctx);
        contexts.erase(c);
        delete c;
        return EGL_TRUE;
    }

    X11Display::WindowSurface::~WindowSurface() {
        for (int i = 0; i <= 1; i++) {
            while (! pNotified[i]) {
                xcb_generic_event_t* e;
                while ((e = xcb_wait_for_special_event(xcbC, ev)) != nullptr) {
                    auto pe = reinterpret_cast<xcb_present_generic_event_t*>(e);
                    if (pe->evtype == XCB_PRESENT_IDLE_NOTIFY) {
                        auto ie = reinterpret_cast<xcb_present_idle_notify_event_t*>(e);
                        if (ie->pixmap == p[0]) {
                            pNotified[0] = true;
                        }
                        if (ie->pixmap == p[1]) {
                            pNotified[1] = true;
                        }
                        if (pNotified[i]) break;
                    }
                }
            }
            if (p[i] != -1) {
                xcb_free_pixmap(xcbC, p[i]);
            }
            if (seg[i] != -1) {
                xcb_shm_detach(xcbC, seg[i]);
            }
            if (pFD[i] != -1) {
                close(pFD[i]);
            }
            if (pData[i] != nullptr) {
                munmap(pData[i], pWidth[i] * pHeight[i] * 4);
            }
        }
        if (ev != nullptr) {
            xcb_unregister_for_special_event(xcbC, ev);
        }
    }

    EGLBoolean X11Display::eglDestroySurface(EGLSurface surface) {
        if (surface == EGL_NO_SURFACE) {
            lastError = EGL_BAD_SURFACE;
            return EGL_FALSE;
        }
        auto s = reinterpret_cast<Surface*>(surface);
        if (s->type == Surface::Type::PBUFFER) {
            surfaces.erase(s);
            delete s;
            return EGL_TRUE;
        }
        if (s->type == Surface::Type::WINDOW) {
            surfaces.erase(s);
            delete static_cast<WindowSurface*>(s);
            return EGL_TRUE;
        }
        lastError = EGL_BAD_SURFACE;
        return EGL_FALSE;
    }

    EGLBoolean X11Display::eglGetConfigAttrib(EGLConfig config, EGLint attribute, EGLint* value) {
        if (attribute == EGL_NATIVE_VISUAL_ID) {
            *value = EGLint(vis.visual_id);
            return EGL_TRUE;
        }
        return real_eglGetConfigAttrib(nativeDisplay, config, attribute, value);
    }
    
    
    EGLBoolean X11Display::eglGetConfigs(EGLConfig* configs, EGLint config_size, EGLint* num_config) {
        return real_eglGetConfigs(nativeDisplay, configs, config_size, num_config);
    }
    
    
    EGLBoolean X11Display::eglInitialize(EGLint* major, EGLint* minor) {
        //dprintf(2, "x11 init display\n");
        
        const xcb_setup_t* setup = xcb_get_setup(xcbC);
        xcb_screen_iterator_t sit = xcb_setup_roots_iterator(setup);
        if (screenIndex > sit.rem) {
            //dprintf(2, "screen index invalid: %d, %d\n", screenIndex, sit.rem);
            xcb_screen_end(sit);
            lastError = EGL_NOT_INITIALIZED;
            return EGL_FALSE;
        }
        for (int i = 0; i < screenIndex; i++) {
            xcb_screen_next(&sit);
        }
        screen = sit.data;
        xcb_screen_end(sit);
        bool visualFound = false;
        xcb_depth_iterator_t dit = xcb_screen_allowed_depths_iterator(screen);
        for (; dit.rem; xcb_depth_next(&dit)) {
            const int len = dit.data->visuals_len;
            //if (dit.data->depth != 32) continue;
            const xcb_visualtype_t* visuals = xcb_depth_visuals(dit.data);
            for (int i = 0; i < len; i++) {
                //dprintf(2, "class: %d  bpp: %d\n", visuals[i]._class, visuals[i].bits_per_rgb_value);
                if (visuals[i]._class == XCB_VISUAL_CLASS_DIRECT_COLOR && visuals[i].bits_per_rgb_value == 8) {
                    vis = visuals[i];
                    visualFound = true;
                    break;
                }
            }
            if (visualFound) break;
        }
        xcb_depth_end(dit);
        if (! visualFound) {
            lastError = EGL_NOT_INITIALIZED;
            return EGL_FALSE;
        }
        
        if (major != nullptr)
            *major = 1;
        if (minor != nullptr)
            *minor = 4;
        //dprintf(2, "x11 init success\n");
        return EGL_TRUE;
    }
    
    EGLBoolean X11Display::eglMakeCurrent(EGLSurface draw, EGLSurface read, EGLContext ctx) {
        // fprintf(stderr, "%p : %p\n", draw, read);
        // fflush(stderr);
        
        // For hardwareBuffer surfaces, draw and read have to be the same
        if ((((Surface*)draw)->backend->type == SurfaceBackend::Type::HWBUFFER || ((Surface*)read)->backend->type == SurfaceBackend::Type::HWBUFFER) && draw != read) return EGL_BAD_MATCH;
        // If the current surface is a HardwareBuffer surface, free the GL state if another Context is made current.
        // This is so that the HardwareBuffer memory can be freed if the surface is destroyed and the context is no longer current.
        EGLSurface oldEGLSurface = glvnd->getCurrentSurface(EGL_DRAW);
        if (oldEGLSurface != EGL_NO_SURFACE) {
            Surface* oldSurface = (Surface*) oldEGLSurface;
            if (oldSurface->backend->type == SurfaceBackend::Type::HWBUFFER) {
                auto backend = static_cast<HardwareBufferSurfaceBackend*>(oldSurface->backend.get());
                if (backend->gl.lastContext != EGL_NO_CONTEXT && (ctx != backend->gl.lastContext || backend->regenerateGL)) {
                    real_glDeleteRenderbuffers(2, (GLuint*)(backend->gl.renderbuffer));
                    real_glDeleteRenderbuffers(2, (GLuint*)(backend->gl.renderbufferDepth));
                    real_glDeleteFramebuffers(2, (GLuint*)(backend->gl.framebuffer));
                    backend->gl = {};
                    //dprintf(2, "HardwareBuffer gl state deleted\n");
                }
            }
        }
        auto retVal = real_eglMakeCurrent(nativeDisplay, Surface::getSurface((Surface*)draw), Surface::getSurface((Surface*)read), Context::getContext((Context*)ctx));
        lastError = real_eglGetError();
        if (lastError == EGL_SUCCESS) {
            if (((Surface*)draw)->backend->type == SurfaceBackend::Type::HWBUFFER) {
                //dprintf(2, "makeCurrent HardwareBuffer surface\n");
                auto backend = static_cast<HardwareBufferSurfaceBackend*>(((Surface*)draw)->backend.get());
                for (int i = 0; i <= 1; i++) {
                    if (backend->buffers[i] == nullptr) {
                        dprintf(2, "HardwareBuffer null\n");
                        continue;
                    }
                    EGLImage img = backend->images[i];
                    if (img == EGL_NO_IMAGE_KHR) {
                        dprintf(2, "EGLImage none\n");
                        continue;
                    }
                    
                    // already created
                    if (backend->gl.framebuffer[i] != -1) continue;
                    
                    // save old bindings
                    GLint rb, fb;
                    real_glGetIntegerv(GL_RENDERBUFFER_BINDING, &rb);
                    if (real_glGetError() != GL_NO_ERROR) {
                        backend->gl = {};
                        lastError = EGL_BAD_CONTEXT;
                        dprintf(2, "real_glGetIntegerv\n");
                        return EGL_FALSE;
                    }
                    real_glGetIntegerv(GL_FRAMEBUFFER_BINDING, &fb);
                    if (real_glGetError() != GL_NO_ERROR) {
                        backend->gl = {};
                        lastError = EGL_BAD_CONTEXT;
                        dprintf(2, "real_glGetIntegerv\n");
                        return EGL_FALSE;
                    }
                    
                    // bind the color buffer
                    real_glGenRenderbuffers(1, (GLuint*)(backend->gl.renderbuffer + i));
                    if (real_glGetError() != GL_NO_ERROR) {
                        backend->gl = {};
                        lastError = EGL_BAD_CONTEXT;
                        dprintf(2, "real_glGenRenderbuffers\n");
                        return EGL_FALSE;
                    }
                    real_glGenRenderbuffers(1, (GLuint*)(backend->gl.renderbufferDepth + i));
                    if (real_glGetError() != GL_NO_ERROR) {
                        backend->gl = {};
                        lastError = EGL_BAD_CONTEXT;
                        dprintf(2, "real_glGenRenderbuffers\n");
                        return EGL_FALSE;
                    }
                    real_glGenFramebuffers(1, (GLuint*)(backend->gl.framebuffer + i));
                    if (real_glGetError() != GL_NO_ERROR) {
                        backend->gl = {};
                        lastError = EGL_BAD_CONTEXT;
                        dprintf(2, "real_glGenFramebuffers\n");
                        return EGL_FALSE;
                    }
                    
                    
                    real_glBindRenderbuffer(GL_RENDERBUFFER, backend->gl.renderbuffer[i]);
                    if (real_glGetError() != GL_NO_ERROR) {
                        backend->gl = {};
                        lastError = EGL_BAD_CONTEXT;
                        dprintf(2, "real_glBindRenderbuffer\n");
                        return EGL_FALSE;
                    }
                    real_glEGLImageTargetRenderbufferStorageOES(GL_RENDERBUFFER, img);
                    if (real_glGetError() != GL_NO_ERROR) {
                        backend->gl = {};
                        lastError = EGL_BAD_CONTEXT;
                        dprintf(2, "real_glEGLImageTargetRenderbufferStorageOES\n");
                        return EGL_FALSE;
                    }
                    
                    // create a depth buffer
                    real_glBindRenderbuffer(GL_RENDERBUFFER, backend->gl.renderbufferDepth[i]);
                    if (real_glGetError() != GL_NO_ERROR) {
                        backend->gl = {};
                        lastError = EGL_BAD_CONTEXT;
                        dprintf(2, "real_glBindRenderbuffer\n");
                        return EGL_FALSE;
                    }
                    
                    AHardwareBuffer_Desc desc;
                    libandroid.AHardwareBuffer_describe(backend->buffers[i], &desc);
                    
                    real_glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT16, desc.width, desc.height);
                    if (real_glGetError() != GL_NO_ERROR) {
                        backend->gl = {};
                        lastError = EGL_BAD_CONTEXT;
                        dprintf(2, "real_glRenderbufferStorage\n");
                        return EGL_FALSE;
                    }
                    
                    // setup the framebuffer
                    real_glBindFramebuffer(GL_FRAMEBUFFER, backend->gl.framebuffer[i]);
                    if (real_glGetError() != GL_NO_ERROR) {
                        backend->gl = {};
                        lastError = EGL_BAD_CONTEXT;
                        dprintf(2, "real_glBindFramebuffer\n");
                        return EGL_FALSE;
                    }
                    
                    real_glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, backend->gl.renderbufferDepth[i]);
                    if (real_glGetError() != GL_NO_ERROR) {
                        backend->gl = {};
                        lastError = EGL_BAD_CONTEXT;
                        dprintf(2, "real_glFramebufferRenderbuffer\n");
                        return EGL_FALSE;
                    }
                    
                    real_glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_RENDERBUFFER, backend->gl.renderbuffer[i]);
                    if (real_glGetError() != GL_NO_ERROR) {
                        backend->gl = {};
                        lastError = EGL_BAD_CONTEXT;
                        dprintf(2, "real_glFramebufferRenderbuffer\n");
                        return EGL_FALSE;
                    }
                    
                    
                    //dprintf(2, "complete: 0x%x\n", real_glCheckFramebufferStatus(GL_FRAMEBUFFER));
                    
                    // restore old bindings
                    real_glBindRenderbuffer(GL_RENDERBUFFER, rb);
                    if (real_glGetError() != GL_NO_ERROR) {
                        backend->gl = {};
                        lastError = EGL_BAD_CONTEXT;
                        dprintf(2, "real_glBindRenderbuffer\n");
                        return EGL_FALSE;
                    }
                    real_glBindFramebuffer(GL_FRAMEBUFFER, fb);
                    if (real_glGetError() != GL_NO_ERROR) {
                        backend->gl = {};
                        lastError = EGL_BAD_CONTEXT;
                        dprintf(2, "real_glBindFramebuffer\n");
                        return EGL_FALSE;
                    }
                }
                GLint fb;
                real_glGetIntegerv(GL_FRAMEBUFFER_BINDING, &fb);
                if (real_glGetError() != GL_NO_ERROR) {
                    backend->gl = {};
                    lastError = EGL_BAD_CONTEXT;
                    dprintf(2, "real_glGetIntegerv\n");
                    return EGL_FALSE;
                }
                // GLint vp[4];
                // real_glGetIntegerv(GL_VIEWPORT, vp);
                // if (real_glGetError() != GL_NO_ERROR) {
                //     backend->gl = {};
                //     lastError = EGL_BAD_CONTEXT;
                //     dprintf(2, "real_glGetIntegerv\n");
                //     return EGL_FALSE;
                // }
                // dprintf(2, "vp: %d, %d, %d, %d\n", vp[0], vp[1], vp[2], vp[3]);
                if ((fb == 0 || fb == backend->gl.framebuffer[(backend->current + 1) % 2]) && backend->gl.framebuffer[backend->current] != -1) {
                    //dprintf(2, "fb 0 bound, redirecting to fb %d\n", backend->gl.framebuffer[backend->current]);
                    real_glBindFramebuffer(GL_FRAMEBUFFER, backend->gl.framebuffer[backend->current]);
                }
                backend->gl.lastContext = ctx;
            }
        }
        
        
        // fprintf(stderr, "err: 0x%x\n", lastError);
        // fflush(stderr);
        return retVal;
    }
    
    EGLBoolean X11Display::eglQueryContext(EGLContext ctx, EGLint attribute, EGLint* value) {
        return EGL_FALSE;
    }

    const char* X11Display::eglQueryString(EGLint name) {
        if (name == EGL_CLIENT_APIS) return "OpenGL_ES";
        if (name == EGL_EXTENSIONS) return "EGL_KHR_platform_x11 EGL_EXT_platform_x11 EGL_EXT_platform_xcb";
        static std::string vendor = [] {
            return std::string("termux-gfx-wrapper (") + real_eglQueryString(nativeDisplay, EGL_VENDOR) + ")";
        }();
        if (name == EGL_VENDOR) return vendor.c_str();
        static std::string version = [] {
            return std::string("1.4 termux-gfx-wrapper (") + real_eglQueryString(nativeDisplay, EGL_VENDOR) + ")";
        }();
        if (name == EGL_VERSION) return version.c_str();
        return nullptr;
    }

    EGLBoolean X11Display::eglQuerySurface(EGLSurface surface, EGLint attribute, EGLint* value) {
        return EGL_FALSE;
    }

    EGLBoolean X11Display::eglSwapBuffers(EGLSurface surface) {
        //dprintf(2, "swapBuffers\n");
        if (surface == EGL_NO_SURFACE) {
            lastError = EGL_BAD_SURFACE;
            return EGL_FALSE;
        }
        auto s = reinterpret_cast<Surface*>(surface);
        if (s->type == Surface::Type::PBUFFER) {
            return EGL_TRUE;
        }
        if (s->type == Surface::Type::WINDOW) {
            //dprintf(2, "window\n");
            auto w = static_cast<WindowSurface*>(s);
            xcb_generic_event_t* e;
            while ((e = xcb_poll_for_special_event(xcbC, w->ev)) != nullptr) {
                auto pe = reinterpret_cast<xcb_present_generic_event_t*>(e);
                if (pe->evtype == XCB_PRESENT_CONFIGURE_NOTIFY) {
                    auto ce = reinterpret_cast<xcb_present_configure_notify_event_t*>(pe);
                    if (ce->window == w->w) {
                        w->wWidth = ce->width;
                        w->wHeight = ce->height;
                    }
                }
                if (pe->evtype == XCB_PRESENT_IDLE_NOTIFY) {
                    auto ie = reinterpret_cast<xcb_present_idle_notify_event_t*>(e);
                    if (ie->pixmap == w->p[0]) {
                        w->pNotified[0] = true;
                    }
                    if (ie->pixmap == w->p[1]) {
                        w->pNotified[1] = true;
                    }
                }
                if (pe->evtype == XCB_PRESENT_COMPLETE_NOTIFY) {
                    xcb_present_complete_notify_event_t* ce = reinterpret_cast<xcb_present_complete_notify_event_t*>(pe);
                    w->presented = true;
                    if (ce->mode != XCB_PRESENT_COMPLETE_MODE_SKIP) {
                        w->lastPresented = std::chrono::duration_cast<std::chrono::microseconds>(std::chrono::steady_clock::now().time_since_epoch()).count();
                    }
                }
            }
            while (! w->pNotified[w->currentP] || ((! w->presented) && x11Mode == X11Mode::BLOCK)) {
                e = xcb_wait_for_special_event(xcbC, w->ev);
                auto pe = reinterpret_cast<xcb_present_generic_event_t*>(e);
                if (pe->evtype == XCB_PRESENT_CONFIGURE_NOTIFY) {
                    auto ce = reinterpret_cast<xcb_present_configure_notify_event_t*>(pe);
                    if (ce->window == w->w) {
                        w->wWidth = ce->width;
                        w->wHeight = ce->height;
                    }
                }
                if (pe->evtype == XCB_PRESENT_IDLE_NOTIFY) {
                    auto ie = reinterpret_cast<xcb_present_idle_notify_event_t*>(e);
                    if (ie->pixmap == w->p[0]) {
                        w->pNotified[0] = true;
                    }
                    if (ie->pixmap == w->p[1]) {
                        w->pNotified[1] = true;
                    }
                }
                if (pe->evtype == XCB_PRESENT_COMPLETE_NOTIFY) {
                    w->presented = true;
                }
            }
            if (w->backend->type == SurfaceBackend::Type::HWBUFFER) {
                auto backend = static_cast<HardwareBufferSurfaceBackend*>(w->backend.get());
                if (w->presented && w->pData[w->currentP] != nullptr) {
                    // X11 uses the raw DMABUF memory, so we need to make sure to flush GL, so the changes make it to X
                    real_glFinish();
                    // void* tmp;
                    // libandroid.AHardwareBuffer_lock(backend->buffers[backend->current], AHARDWAREBUFFER_USAGE_CPU_READ_OFTEN, -1, NULL, &tmp);
                    // libandroid.AHardwareBuffer_unlock(backend->buffers[backend->current], NULL);
                    // if (real_glGetError() != GL_NO_ERROR) {
                    //     lastError = EGL_BAD_CONTEXT;
                    //     dprintf(2, "real_glFinish\n");
                    //     return EGL_FALSE;
                    // }
                    // if (real_eglGetCurrentContext() == EGL_NO_CONTEXT) {
                    //     lastError = EGL_BAD_CONTEXT;
                    //     dprintf(2, "real_eglGetCurrentContext\n");
                    //     return EGL_FALSE;
                    // }
                    // if (! hwbufferBGRAvailable)
                    //     format::GLColorToX11((uint32_t*)w->pData[w->currentP], w->pWidth[w->currentP], w->pHeight[w->currentP], true);
                    // memset(w->pData[w->currentP], 0xff, 4 * w->pWidth[w->currentP] * w->pHeight[w->currentP]);
                    if ((err = xcb_request_check(xcbC, xcb_present_pixmap_checked(xcbC, w->w, w->p[w->currentP], 0, 0, 0, 0, 0, 0, 0, 0, XCB_PRESENT_OPTION_NONE, 0, 0, 0, 0, nullptr)))) {
                        dprintf(2, "Failed to xcb_present_pixmap_checked()\n");
                        free(err);
                        return EGL_FALSE;
                    }
                    w->pNotified[w->currentP] = false;
                    //dprintf(2, "draw %d %d %d %d\n", w->wWidth, w->wHeight, w->pWidth, w->pHeight);
                    w->currentP = (w->currentP + 1) % 2;
                    w->presented = false;
                }
                if (w->wWidth != w->pWidth[w->currentP] || w->wHeight != w->pHeight[w->currentP] || w->pData[w->currentP] == nullptr) {
                    dprintf(2, "resize\n");
                    // wait for the pixmap to idle before deleting
                    while (! w->pNotified[w->currentP]) {
                        e = xcb_wait_for_special_event(xcbC, w->ev);
                        auto pe = reinterpret_cast<xcb_present_generic_event_t*>(e);
                        if (pe->evtype == XCB_PRESENT_CONFIGURE_NOTIFY) {
                            auto ce = reinterpret_cast<xcb_present_configure_notify_event_t*>(pe);
                            if (ce->window == w->w) {
                                w->wWidth = ce->width;
                                w->wHeight = ce->height;
                            }
                        }
                        if (pe->evtype == XCB_PRESENT_IDLE_NOTIFY) {
                            auto ie = reinterpret_cast<xcb_present_idle_notify_event_t*>(e);
                            if (ie->pixmap == w->p[0]) {
                                w->pNotified[0] = true;
                            }
                            if (ie->pixmap == w->p[1]) {
                                w->pNotified[1] = true;
                            }
                        }
                        if (pe->evtype == XCB_PRESENT_COMPLETE_NOTIFY) {
                            w->presented = true;
                        }
                    }
                    SmartHardwareBuffer hb{allocHB(HBFormat(), w->wWidth, w->wHeight, HBUsage)};
                    if (hb == nullptr) {
                        lastError = EGL_BAD_ALLOC;
                        dprintf(2, "could not alloc HardwareBuffer\n");
                        return EGL_FALSE;
                    }
                    EGLint iattribs[] = {
                        EGL_IMAGE_PRESERVED_KHR,
                        EGL_NONE};
                    SmartEGLImage hbi{real_eglCreateImageKHR(nativeDisplay, EGL_NO_CONTEXT, EGL_NATIVE_BUFFER_ANDROID, real_eglGetNativeClientBufferANDROID(hb), iattribs)};
                    if (hbi == EGL_NO_IMAGE_KHR) {
                        lastError = EGL_BAD_ALLOC;
                        dprintf(2, "could not create EGLImage from HardwareBuffer\n");
                        return EGL_FALSE;
                    }
                    if (w->p[w->currentP] != -1) {
                        xcb_free_pixmap(xcbC, w->p[w->currentP]);
                    }
                    if (w->seg[w->currentP] != -1) {
                        xcb_shm_detach(xcbC, w->seg[w->currentP]);
                    }
                    if (w->pFD[w->currentP] != -1) {
                        close(w->pFD[w->currentP]);
                    }
                    if (w->pData[w->currentP] != nullptr) {
                        munmap(w->pData[w->currentP], w->pWidth[w->currentP] * w->pHeight[w->currentP] * 4);
                    }
                    w->pWidth[w->currentP] = w->wWidth;
                    w->pHeight[w->currentP] = w->wHeight;
                    backend->buffers[w->currentP] = hb;
                    backend->images[w->currentP] = std::move(hbi);
                    backend->regenerateGL = true;
                    
                    w->pFD[w->currentP] = HBDMABUF(hb);
                    w->pData[w->currentP] = mmap(nullptr, w->pWidth[w->currentP] * w->pHeight[w->currentP] * 4, PROT_READ | PROT_WRITE, MAP_SHARED, w->pFD[w->currentP], 0);
                    if (w->pData == MAP_FAILED) {
                        dprintf(2, "could not mmap HardwareBuffer\n");
                        close(w->pFD[w->currentP]);
                        w->pFD[w->currentP] = -1;
                        w->pData[w->currentP] = nullptr;
                        return EGL_FALSE;
                    }
                    w->seg[w->currentP] = xcb_generate_id(xcbC);
                    if ((err = xcb_request_check(xcbC, xcb_shm_attach_fd_checked(xcbC, w->seg[w->currentP], w->pFD[w->currentP], false)))) {
                        dprintf(2, "could create shm\n");
                        close(w->pFD[w->currentP]);
                        w->pFD[w->currentP] = -1;
                        w->pData[w->currentP] = nullptr;
                        free(err);
                        return EGL_FALSE;
                    }
                    w->p[w->currentP] = xcb_generate_id(xcbC);
                    if ((err = xcb_request_check(xcbC, xcb_shm_create_pixmap_checked(xcbC, w->p[w->currentP], w->w, w->pWidth[w->currentP], w->pHeight[w->currentP], 24, w->seg[w->currentP], 0)))) {
                        dprintf(2, "could not create pixmap\n");
                        close(w->pFD[w->currentP]);
                        w->pFD[w->currentP] = -1;
                        munmap(w->pData, w->pWidth[w->currentP] * w->pHeight[w->currentP] * 4);
                        w->pData[w->currentP] = nullptr;
                        w->p[w->currentP] = -1;
                        xcb_shm_detach(xcbC, w->seg[w->currentP]);
                        free(err);
                        return EGL_FALSE;
                    }
                }
                // set current buffer, regenerate GL state
                backend->current = w->currentP;
                eglMakeCurrent(surface, surface, glvnd->getCurrentContext());
            } else {
                if (w->wWidth != w->pWidth[w->currentP] || w->wHeight != w->pHeight[w->currentP] || w->pData[w->currentP] == nullptr) {
                    if (w->p[w->currentP] != -1) {
                        xcb_free_pixmap(xcbC, w->p[w->currentP]);
                    }
                    if (w->seg[w->currentP] != -1) {
                        xcb_shm_detach(xcbC, w->seg[w->currentP]);
                    }
                    if (w->pFD[w->currentP] != -1) {
                        close(w->pFD[w->currentP]);
                    }
                    if (w->pData[w->currentP] != nullptr) {
                        munmap(w->pData[w->currentP], w->pWidth[w->currentP] * w->pHeight[w->currentP] * 4);
                    }
                    w->pWidth[w->currentP] = w->wWidth;
                    w->pHeight[w->currentP] = w->wHeight;
                    w->pFD[w->currentP] = os_create_anonymous_file(w->pWidth[w->currentP] * w->pHeight[w->currentP] * 4);
                    if (w->pFD[w->currentP] == -1) {
                        dprintf(2, "Failed to create anonymous file\n");
                        return EGL_FALSE;
                    }
                    w->pData[w->currentP] = mmap(nullptr, w->pWidth[w->currentP] * w->pHeight[w->currentP] * 4, PROT_READ | PROT_WRITE, MAP_SHARED, w->pFD[w->currentP], 0);
                    if (w->pData == MAP_FAILED) {
                        dprintf(2, "could not mmap ashmem\n");
                        close(w->pFD[w->currentP]);
                        w->pFD[w->currentP] = -1;
                        w->pData[w->currentP] = nullptr;
                        return EGL_FALSE;
                    }
                    w->seg[w->currentP] = xcb_generate_id(xcbC);
                    if ((err = xcb_request_check(xcbC, xcb_shm_attach_fd_checked(xcbC, w->seg[w->currentP], w->pFD[w->currentP], false)))) {
                        dprintf(2, "could create shm\n");
                        close(w->pFD[w->currentP]);
                        w->pFD[w->currentP] = -1;
                        w->pData[w->currentP] = nullptr;
                        free(err);
                        return EGL_FALSE;
                    }
                    w->p[w->currentP] = xcb_generate_id(xcbC);
                    if ((err = xcb_request_check(xcbC, xcb_shm_create_pixmap_checked(xcbC, w->p[w->currentP], w->w, w->pWidth[w->currentP], w->pHeight[w->currentP], 24, w->seg[w->currentP], 0)))) {
                        dprintf(2, "could not create pixmap\n");
                        close(w->pFD[w->currentP]);
                        w->pFD[w->currentP] = -1;
                        munmap(w->pData, w->pWidth[w->currentP] * w->pHeight[w->currentP] * 4);
                        w->pData[w->currentP] = nullptr;
                        w->p[w->currentP] = -1;
                        xcb_shm_detach(xcbC, w->seg[w->currentP]);
                        free(err);
                        return EGL_FALSE;
                    }
                    
                    // only present the buffer to the pixmap if the last one completed
                    if (w->presented) {
                        // set the alignment, if it was changed, the reading would be broken
                        GLint align;
                        glGetIntegerv(GL_PACK_ALIGNMENT, &align);
                        glPixelStorei(GL_PACK_ALIGNMENT, 4);
                        real_glReadPixels(0, 0, w->pWidth[w->currentP], w->pHeight[w->currentP], GL_RGBA, GL_UNSIGNED_BYTE, w->pData[w->currentP]);
                        glPixelStorei(GL_PACK_ALIGNMENT, align);
                        
                        format::GLColorToX11((uint32_t*)w->pData[w->currentP], w->pWidth[w->currentP], w->pHeight[w->currentP], true);
                        
                        if ((err = xcb_request_check(xcbC, xcb_present_pixmap_checked(xcbC, w->w, w->p[w->currentP], 0, 0, 0, 0, 0, 0, 0, 0, XCB_PRESENT_OPTION_NONE, 0, 0, 0, 0, nullptr)))) {
                            dprintf(2, "could not present\n");
                            free(err);
                            return EGL_FALSE;
                        }
                        w->pNotified[w->currentP] = false;
                        w->currentP = (w->currentP + 1) % 2;
                        w->presented = false;
                    }
                    
                    auto backend = std::make_unique<PBufferSurfaceBackend>();
                    backend->width = w->wWidth;
                    backend->height = w->wHeight;
                    EGLint pbattribs[] = {
                        EGL_WIDTH, w->wWidth,
                        EGL_HEIGHT, w->wHeight,
                        EGL_NONE};
                    backend->pbuffer = real_eglCreatePbufferSurface(nativeDisplay, w->conf, pbattribs);
                    if (backend->pbuffer == EGL_NO_SURFACE) {
                        dprintf(2, "could not resize buffer\n");
                        return EGL_FALSE;
                    }
                    w->backend = std::move(backend);
                    // next frame will go to the new size buffer
                    real_eglMakeCurrent(nativeDisplay, Surface::getSurface(w), Surface::getSurface(w), Context::getContext((Context*) glvnd->getCurrentContext()));
                    //dprintf(2, "draw resize\n");
                } else {
                    uint64_t now = std::chrono::duration_cast<std::chrono::microseconds>(std::chrono::steady_clock::now().time_since_epoch()).count();
                    // only present the buffer to the pixmap if the last one completed, or we need to for the frame to arrive in time
                    if (w->presented || now + w->postProcessingTime >= w->lastPresented + 16000) {
                        // if (now >= w->lastPresented + w->postProcessingTime) {
                        //     dprintf(2, "early\n");
                        // }
                        // set the alignment, if it was changed, the reading would be broken
                        
                        GLint align;
                        glGetIntegerv(GL_PACK_ALIGNMENT, &align);
                        glPixelStorei(GL_PACK_ALIGNMENT, 4);
                        real_glReadPixels(0, 0, w->pWidth[w->currentP], w->pHeight[w->currentP], GL_RGBA, GL_UNSIGNED_BYTE, w->pData[w->currentP]);
                        glPixelStorei(GL_PACK_ALIGNMENT, align);
                        
                        format::GLColorToX11((uint32_t*)w->pData[w->currentP], w->pWidth[w->currentP], w->pHeight[w->currentP], true);

        //                /* Create black (foreground) graphic context */
        //                xcb_gcontext_t const gc = xcb_generate_id( xcbC );
        //                uint32_t const gc_mask = XCB_GC_FOREGROUND | XCB_GC_GRAPHICS_EXPOSURES;
        //                uint32_t const gc_values[] = {screen->black_pixel, 0};
        //                xcb_create_gc(xcbC, gc, w->w, gc_mask, gc_values);
        //                free(xcb_request_check(xcbC, xcb_shm_put_image(xcbC, w->w, gc, w->wWidth, w->wHeight, 0, 0, w->pWidth, w->pHeight, 0, 0, 24, XCB_IMAGE_FORMAT_Z_PIXMAP, 0, w->seg, 0)));

                        if ((err = xcb_request_check(xcbC, xcb_present_pixmap_checked(xcbC, w->w, w->p[w->currentP], 0, 0, 0, 0, 0, 0, 0, 0, XCB_PRESENT_OPTION_NONE, 0, 0, 0, 0, nullptr)))) {
                            dprintf(2, "Failed to xcb_present_pixmap_checked()\n");
                            free(err);
                            return EGL_FALSE;
                        }
                        w->postProcessingTime = (std::chrono::duration_cast<std::chrono::microseconds>(std::chrono::steady_clock::now().time_since_epoch()).count() - now) * 2;
                        // dprintf(2, "time: %llu\n", w->postProcessingTime);
                        w->pNotified[w->currentP] = false;
                        //dprintf(2, "draw %d %d %d %d\n", w->wWidth, w->wHeight, w->pWidth, w->pHeight);
                        w->currentP = (w->currentP + 1) % 2;
                        w->presented = false;
                    }
                }
            }
            return EGL_TRUE;
        }
        return EGL_FALSE;
    }

    EGLBoolean X11Display::eglTerminate() {
        for (Context* c : contexts) {
            delete c;
        }
        contexts.clear();
        for(Surface* s : surfaces) {
            delete s;
        }
        surfaces.clear();
        return EGL_TRUE;
    }

    EGLBoolean X11Display::eglBindTexImage(EGLSurface surface, EGLint buffer) {
        return EGL_FALSE;
    }

    EGLBoolean X11Display::eglReleaseTexImage(EGLSurface surface, EGLint buffer) {
        return EGL_FALSE;
    }

    EGLBoolean X11Display::eglSurfaceAttrib(EGLSurface surface, EGLint attribute, EGLint value) {
        return EGL_FALSE;
    }

    EGLBoolean X11Display::eglSwapInterval(EGLint interval) {
        return EGL_TRUE;
    }

    EGLSurface X11Display::eglCreatePbufferFromClientBuffer(EGLenum buftype, EGLClientBuffer buffer, EGLConfig config, const EGLint* attrib_list) {
        return EGL_FALSE;
    }

    EGLBoolean X11Display::eglReleaseThread() {
        return real_eglReleaseThread();
    }

    EGLContext X11Display::eglGetCurrentContext() {
        return glvnd->getCurrentContext();
    }
    
    EGLSync X11Display::eglCreateSync(EGLenum type, const EGLAttrib* attrib_list) {
        return EGL_NO_SYNC;
    }
    
    EGLBoolean X11Display::eglDestroySync(EGLSync sync) {
        return EGL_FALSE;
    }

    EGLint X11Display::eglClientWaitSync(EGLSync sync, EGLint flags, EGLTime timeout) {
        return EGL_FALSE;
    }
    
    EGLBoolean X11Display::eglGetSyncAttrib(EGLSync sync, EGLint attribute, EGLAttrib* value) {
        return EGL_FALSE;
    }

    EGLImage X11Display::eglCreateImage(EGLContext ctx, EGLenum target, EGLClientBuffer buffer, const EGLAttrib* attrib_list) {
        return EGL_NO_IMAGE;
    }

    EGLBoolean X11Display::eglDestroyImage(EGLImage image) {
        return EGL_FALSE;
    }
    
    EGLSurface X11Display::eglCreatePlatformWindowSurface(EGLConfig config, void* native_window, const EGLAttrib* attrib_list) {
        return EGL_NO_SURFACE;
    }

    EGLSurface X11Display::eglCreatePlatformPixmapSurface(EGLConfig config, void* native_pixmap, const EGLAttrib* attrib_list) {
        return EGL_NO_SURFACE;
    }

    EGLBoolean X11Display::eglWaitSync(EGLSync sync, EGLint flags) {
        return EGL_FALSE;
    }
}

