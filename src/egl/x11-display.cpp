#include "egl.hpp"

namespace egl_wrapper {
    
    X11Display::X11Display(Display* x11Display, const EGLAttrib *attrib_list) : EGLDisplayBackend(DisplayType::X11) {
        if (x11Display == EGL_DEFAULT_DISPLAY) {
            int screen = -1;
            if (attrib_list != NULL) {
                if (*attrib_list != EGL_PLATFORM_X11_SCREEN_EXT && *attrib_list != EGL_NONE) throw Exceptions::BadAttributeException();
                attrib_list++;
                screen = *attrib_list;
                attrib_list++;
                if (*attrib_list != EGL_NONE) throw Exceptions::BadAttributeException();
            }
            int* screenp = &screen;
            if (screen == -1) screenp = NULL;
            this->xcbC = xcb_connect(NULL, screenp);
            if (xcb_connection_has_error(this->xcbC) != 0) {
                xcb_disconnect(this->xcbC);
                throw Exceptions::NoDisplayException();
            }
            this->ownsConnection = true;
        } else {
            this->ownsConnection = false;
            this->xcbC = XGetXCBConnection(x11Display);
            fprintf(stderr, "xlib display created\n");
            fflush(stderr);
        }
    }
    
    X11Display::X11Display(xcb_connection_t* xcbC, const EGLAttrib *attrib_list) : EGLDisplayBackend(DisplayType::X11) {
        if (xcbC == (void*)EGL_DEFAULT_DISPLAY) {
            int screen = -1;
            if (attrib_list != NULL) {
                if (*attrib_list != EGL_PLATFORM_XCB_SCREEN_EXT && *attrib_list != EGL_NONE) throw Exceptions::BadAttributeException();
                attrib_list++;
                screen = *attrib_list;
                attrib_list++;
                if (*attrib_list != EGL_NONE) throw Exceptions::BadAttributeException();
            }
            int* screenp = &screen;
            if (screen == -1) screenp = NULL;
            this->xcbC = xcb_connect(NULL, screenp);
            if (xcb_connection_has_error(this->xcbC) != 0) {
                xcb_disconnect(this->xcbC);
                throw Exceptions::NoDisplayException();
            }
            this->ownsConnection = true;
        } else {
            this->ownsConnection = false;
            this->xcbC = xcbC;
        }
    }
    
    
    X11Display::~X11Display() {
        if (ownsConnection) {
            xcb_disconnect(xcbC);
            ownsConnection = false;
        }
        xcbC = NULL;
    }
    
    
    
    EGLBoolean X11Display::eglChooseConfig(const EGLint* attrib_list, EGLConfig* configs, EGLint config_size, EGLint* num_config) {
        return EGL_FALSE;
    }
    
    
    EGLBoolean X11Display::eglCopyBuffers(EGLSurface surface, EGLNativePixmapType target) {
        return EGL_FALSE;
    }
    
    
    EGLContext X11Display::eglCreateContext(EGLConfig config, EGLContext share_context, const EGLint* attrib_list) {
        return EGL_NO_CONTEXT;
    }
    
    
    EGLSurface X11Display::eglCreatePbufferSurface(EGLConfig config, const EGLint* attrib_list) {
        return EGL_NO_SURFACE;
    }
    
    
    EGLSurface X11Display::eglCreatePixmapSurface(EGLConfig config, EGLNativePixmapType pixmap, const EGLint* attrib_list) {
        return EGL_NO_SURFACE;
    }
    
    
    EGLSurface X11Display::eglCreateWindowSurface(EGLConfig config, EGLNativeWindowType win, const EGLint* attrib_list) {
        return EGL_NO_SURFACE;
    }
    
    
    EGLBoolean X11Display::eglDestroyContext(EGLContext ctx) {
        return EGL_FALSE;
    }
    
    
    EGLBoolean X11Display::eglDestroySurface(EGLSurface surface) {
        return EGL_FALSE;
    }
    
    
    EGLBoolean X11Display::eglGetConfigAttrib(EGLConfig config, EGLint attribute, EGLint* value) {
        return EGL_FALSE;
    }
    
    
    EGLBoolean X11Display::eglGetConfigs(EGLConfig* configs, EGLint config_size, EGLint* num_config) {
        return EGL_FALSE;
    }
    
    
    EGLBoolean X11Display::eglInitialize(EGLint* major, EGLint* minor) {
        fprintf(stderr, "x11 init display\n");
        fflush(stderr);
        
        if (major != NULL)
            *major = 1;
        if (minor != NULL)
            *minor = 4;
        return EGL_TRUE;
    }
    
    
    EGLBoolean X11Display::eglQueryContext(EGLContext ctx, EGLint attribute, EGLint* value) {
        return EGL_FALSE;
    }
    
    
    const char* X11Display::eglQueryString(EGLint name) {
        if (name == EGL_CLIENT_APIS) return "OpenGL_ES";
        if (name == EGL_EXTENSIONS) return "EGL_KHR_platform_x11 EGL_EXT_platform_x11 EGL_EXT_platform_xcb";
        if (name == EGL_VENDOR) return "Termux EGL wrapper";
        if (name == EGL_VERSION) return "1.4 Termux EGL wrapper";
        return NULL;
    }
    
    
    EGLBoolean X11Display::eglQuerySurface(EGLSurface surface, EGLint attribute, EGLint* value) {
        return EGL_FALSE;
    }
    
    
    EGLBoolean X11Display::eglSwapBuffers(EGLSurface surface) {
        return EGL_FALSE;
    }
    
    
    EGLBoolean X11Display::eglTerminate() {
        return EGL_FALSE;
    }
    
    
    EGLBoolean X11Display::eglWaitGL(void) {
        return EGL_FALSE;
    }
    
    
    EGLBoolean X11Display::eglWaitNative(EGLint engine) {
        return EGL_FALSE;
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
    
    
    EGLBoolean X11Display::eglReleaseThread(void) {
        return EGL_FALSE;
    }
    
    
    EGLBoolean X11Display::eglWaitClient(void) {
        return EGL_FALSE;
    }
    
    
    EGLContext X11Display::eglGetCurrentContext(void) {
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

