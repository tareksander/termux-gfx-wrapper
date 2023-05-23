#include "egl.hpp"

#include <unistd.h>
#include <sys/mman.h>

#ifdef __ANDROID__
#include <linux/ashmem.h>
#include <sys/ioctl.h>
#endif

namespace egl_wrapper {
    void X11Display::checkExtensions() const {
        // check for the shm extension
        {
            xcb_generic_error_t* err = nullptr;
            xcb_shm_query_version_reply_t* shmv = xcb_shm_query_version_reply(xcbC, xcb_shm_query_version(xcbC), &err);
            if (err != nullptr) {
                //fprintf(stderr, "shm error\n");
                //fflush(stderr);
                free(shmv);
                throw Exceptions::NoDisplayException();
            }
            if (shmv->major_version != 1 || shmv->minor_version < 2 || shmv->shared_pixmaps == false) {
                //fprintf(stderr, "shm version\n");
                //fflush(stderr);
                free(shmv);
                throw Exceptions::NoDisplayException();
            }
            free(shmv);
        }
        // check for present extension
        {
            xcb_generic_error_t* err = nullptr;
            xcb_present_query_version_reply_t* presv = xcb_present_query_version_reply(xcbC, xcb_present_query_version(xcbC, XCB_PRESENT_MAJOR_VERSION, XCB_PRESENT_MINOR_VERSION), &err);
            if (err != nullptr) {
                //fprintf(stderr, "present error\n");
                //fflush(stderr);
                free(presv);
                throw Exceptions::NoDisplayException();
            }
            if (presv->major_version != 1 || presv->minor_version < 2) {
                //fprintf(stderr, "present version\n");
                //fflush(stderr);
                free(presv);
                throw Exceptions::NoDisplayException();
            }
            free(presv);
        }
    }
    
    X11Display::X11Display(Display* x11Display, const EGLAttrib *attrib_list) : EGLDisplayBackend(DisplayType::X11) {
        //fprintf(stderr, "xlib\n");
        //fflush(stderr);
        if (x11Display == EGL_DEFAULT_DISPLAY) {
            //fprintf(stderr, "default\n");
            //fflush(stderr);
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
        //fprintf(stderr, "check\n");
        //fflush(stderr);
        checkExtensions();
    }
    
    X11Display::X11Display(xcb_connection_t* xcbC, const EGLAttrib *attrib_list) : EGLDisplayBackend(DisplayType::X11) {
        //fprintf(stderr, "xcb\n");
        //fflush(stderr);
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
        s->eid = xcb_generate_id(xcbC);
        s->ev = xcb_register_for_special_xge(xcbC, &xcb_present_id, s->eid, nullptr);
        if (xcb_request_check(xcbC, xcb_present_select_input_checked(xcbC, s->eid, s->w, XCB_PRESENT_EVENT_MASK_COMPLETE_NOTIFY | XCB_PRESENT_EVENT_MASK_CONFIGURE_NOTIFY)) != nullptr) {
            return EGL_NO_SURFACE;
        }
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
        if (p != -1) {
            xcb_free_pixmap(xcbC, p);
        }
        if (ev != nullptr) {
            xcb_unregister_for_special_event(xcbC, ev);
        }
        if (pFD != -1) {
            close(pFD);
        }
        if (pData != nullptr) {
            munmap(pData, pWidth * pHeight * 4);
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
        //fprintf(stderr, "x11 init display\n");
        //fflush(stderr);
        
        const xcb_setup_t* setup = xcb_get_setup(xcbC);
        xcb_screen_iterator_t sit = xcb_setup_roots_iterator(setup);
        if (screenIndex > sit.rem) {
            //fprintf(stderr, "screen index invalid: %d, %d\n", screenIndex, sit.rem);
            //fflush(stderr);
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
                //fprintf(stderr, "class: %d  bpp: %d\n", visuals[i]._class, visuals[i].bits_per_rgb_value);
                //fflush(stderr);
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
        //fprintf(stderr, "x11 init success\n");
        //fflush(stderr);
        return EGL_TRUE;
    }
    
    EGLBoolean X11Display::eglMakeCurrent(EGLSurface draw, EGLSurface read, EGLContext ctx) {
        auto retVal = real_eglMakeCurrent(nativeDisplay, Surface::getSurface((Surface*)draw), Surface::getSurface((Surface*)read), Context::getContext((Context*)ctx));
        lastError = real_eglGetError();
        return retVal;
    }
    
    EGLBoolean X11Display::eglQueryContext(EGLContext ctx, EGLint attribute, EGLint* value) {
        return EGL_FALSE;
    }

    const char* X11Display::eglQueryString(EGLint name) {
        if (name == EGL_CLIENT_APIS) return "OpenGL_ES";
        if (name == EGL_EXTENSIONS) return "EGL_KHR_platform_x11 EGL_EXT_platform_x11 EGL_EXT_platform_xcb";
        if (name == EGL_VENDOR) return "Termux EGL wrapper X11";
        if (name == EGL_VERSION) return "1.4 Termux EGL wrapper X11";
        return nullptr;
    }

    EGLBoolean X11Display::eglQuerySurface(EGLSurface surface, EGLint attribute, EGLint* value) {
        return EGL_FALSE;
    }

    EGLBoolean X11Display::eglSwapBuffers(EGLSurface surface) {
        //fprintf(stderr, "swapBuffers\n");
        //fflush(stderr);
        if (surface == EGL_NO_SURFACE) {
            lastError = EGL_BAD_SURFACE;
            return EGL_FALSE;
        }
        auto s = reinterpret_cast<Surface*>(surface);
        if (s->type == Surface::Type::PBUFFER) {
            return EGL_TRUE;
        }
        if (s->type == Surface::Type::WINDOW) {
            //fprintf(stderr, "window\n");
            //fflush(stderr);
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
                if (pe->evtype == XCB_PRESENT_COMPLETE_NOTIFY) {
//                    auto ce = reinterpret_cast<xcb_present_complete_notify_event_t*>(pe);
                    w->notifyNeeded = false;
                }
            }
            while (w->notifyNeeded) {
                auto pe = reinterpret_cast<xcb_present_generic_event_t*>(xcb_wait_for_special_event(xcbC, w->ev));
                if (pe->evtype == XCB_PRESENT_CONFIGURE_NOTIFY) {
                    auto ce = reinterpret_cast<xcb_present_configure_notify_event_t*>(pe);
                    if (ce->window == w->w) {
                        w->wWidth = ce->width;
                        w->wHeight = ce->height;
                    }
                }
                if (pe->evtype == XCB_PRESENT_COMPLETE_NOTIFY) {
//                    auto ce = reinterpret_cast<xcb_present_complete_notify_event_t*>(pe);
                    w->notifyNeeded = false;
                }
            }
            if (w->wWidth != w->pWidth || w->wHeight != w->pHeight || w->pData == nullptr) {
                if (w->p != -1) {
                    xcb_free_pixmap(xcbC, w->p);
                }
                if (w->pFD != -1) {
                    close(w->pFD);
                }
                if (w->pData != nullptr) {
                    munmap(w->pData, w->pWidth * w->pHeight * 4);
                }
                w->pWidth = w->wWidth;
                w->pHeight = w->wHeight;
#if 1
                {
                    w->pFD = memfd_create("pixmap", MFD_CLOEXEC);
                    if (w->pFD == -1) {
                        return EGL_FALSE;
                    }
                    if (ftruncate(w->pFD, w->pWidth * w->pHeight * 4) != 0) {
                        close(w->pFD);
                        w->pFD = -1;
                        return EGL_FALSE;
                    }
                }
#endif
#ifdef __ANDROID__
                if (w->pFD == -1){
                    // use ashmem for now
                    w->pFD = open("/dev/ashmem", O_RDWR);
                    if (w->pFD == -1) {
                        fprintf(stderr, "could not open /dev/ashmem\n");
                        fflush(stderr);
                        return EGL_FALSE;
                    }
                    if (ioctl(w->pFD, ASHMEM_SET_SIZE, size_t(w->pWidth * w->pHeight * 4)) == -1) {
                        fprintf(stderr, "could not resize ashmem\n");
                        fflush(stderr);
                        close(w->pFD);
                        return EGL_FALSE;
                    }
                }
#endif
                w->pData = mmap(nullptr, w->pWidth * w->pHeight * 4, PROT_READ | PROT_WRITE, MAP_SHARED, w->pFD, 0);
                if (w->pData == MAP_FAILED) {
                    fprintf(stderr, "could not mmap ashmem\n");
                    fflush(stderr);
                    close(w->pFD);
                    w->pFD = -1;
                    w->pData = nullptr;
                    return EGL_FALSE;
                }
                xcb_shm_seg_t seg = xcb_generate_id(xcbC);
                if (xcb_request_check(xcbC, xcb_shm_attach_fd(xcbC, seg, w->pFD, true)) != nullptr) {
                    fprintf(stderr, "could create shm\n");
                    fflush(stderr);
                    close(w->pFD);
                    w->pFD = -1;
                    w->pData = nullptr;
                    return EGL_FALSE;
                }
                w->p = xcb_generate_id(xcbC);
                if (xcb_request_check(xcbC, xcb_shm_create_pixmap(xcbC, w->p, w->w, w->pWidth, w->pHeight, 32, seg, 0)) != nullptr) {
                    fprintf(stderr, "could not create pixmap\n");
                    fflush(stderr);
                    close(w->pFD);
                    w->pFD = -1;
                    munmap(w->pData, w->pWidth * w->pHeight * 4);
                    w->pData = nullptr;
                    w->p = -1;
                    xcb_shm_detach(xcbC, seg);
                    return EGL_FALSE;
                }
                xcb_shm_detach(xcbC, seg);
                
                real_glReadPixels(0, 0, w->pWidth, w->pHeight, GL_RGBA, GL_UNSIGNED_BYTE, w->pData);
                
                if (xcb_request_check(xcbC, xcb_present_pixmap(xcbC, w->w, w->p, 0, 0, 0, 0, 0, 0, 0, 0, XCB_PRESENT_OPTION_NONE, 0, 0, 0, 0, nullptr)) != nullptr) {
                    fprintf(stderr, "could not present\n");
                    fflush(stderr);
                    return EGL_FALSE;
                }
                w->notifyNeeded = true;
                
                if (w->backend->type == SurfaceBackend::Type::PBUFFER) {
                    auto backend = std::make_unique<PBufferSurfaceBackend>();
                    backend->width = w->wWidth;
                    backend->height = w->wHeight;
                    EGLint pbattribs[] = {
                        EGL_WIDTH, w->wWidth,
                        EGL_HEIGHT, w->wHeight,
                        EGL_NONE};
                    backend->pbuffer = real_eglCreatePbufferSurface(nativeDisplay, w->conf, pbattribs);
                    if (backend->pbuffer == EGL_NO_SURFACE) {
                        fprintf(stderr, "could not resize buffer\n");
                        fflush(stderr);
                        return EGL_FALSE;
                    }
                    w->backend = std::move(backend);
                    // next frame will go to the new size buffer
                    real_eglMakeCurrent(nativeDisplay, Surface::getSurface(w), Surface::getSurface(w), Context::getContext((Context*) glvnd->getCurrentContext()));
                }
                fprintf(stderr, "draw resize\n");
                fflush(stderr);
            } else {
                real_glReadPixels(0, 0, w->pWidth, w->pHeight, GL_RGBA, GL_UNSIGNED_BYTE, w->pData);
                
                if (xcb_request_check(xcbC, xcb_present_pixmap(xcbC, w->w, w->p, 0, 0, 0, 0, 0, 0, 0, 0, XCB_PRESENT_OPTION_NONE, 0, 0, 0, 0, nullptr)) != nullptr) {
                    return EGL_FALSE;
                }
                w->notifyNeeded = true;
                fprintf(stderr, "draw\n");
                fflush(stderr);
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
        return EGL_FALSE;
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

