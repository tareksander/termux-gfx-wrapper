#include "egl.hpp"

namespace egl_wrapper {
    
    X11Display::X11Display(Display* x11Display, const EGLAttrib *attrib_list) {
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
    
    X11Display::X11Display(xcb_connection_t* xcbC, const EGLAttrib *attrib_list) {
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
        
    }
    
    
    EGLBoolean X11Display::eglCopyBuffers(EGLSurface surface, EGLNativePixmapType target) {
        
    }
    
    
    EGLContext X11Display::eglCreateContext(EGLConfig config, EGLContext share_context, const EGLint* attrib_list) {
        
    }
    
    
    EGLSurface X11Display::eglCreatePbufferSurface(EGLConfig config, const EGLint* attrib_list) {
        
    }
    
    
    EGLSurface X11Display::eglCreatePixmapSurface(EGLConfig config, EGLNativePixmapType pixmap, const EGLint* attrib_list) {
        
    }
    
    
    EGLSurface X11Display::eglCreateWindowSurface(EGLConfig config, EGLNativeWindowType win, const EGLint* attrib_list) {
        
    }
    
    
    EGLBoolean X11Display::eglDestroyContext(EGLContext ctx) {
        
    }
    
    
    EGLBoolean X11Display::eglDestroySurface(EGLSurface surface) {
        
    }
    
    
    EGLBoolean X11Display::eglGetConfigAttrib(EGLConfig config, EGLint attribute, EGLint* value) {
        
    }
    
    
    EGLBoolean X11Display::eglGetConfigs(EGLConfig* configs, EGLint config_size, EGLint* num_config) {
        
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
        
    }
    
    
    const char* X11Display::eglQueryString(EGLint name) {
        return NULL;
    }
    
    
    EGLBoolean X11Display::eglQuerySurface(EGLSurface surface, EGLint attribute, EGLint* value) {
        
    }
    
    
    EGLBoolean X11Display::eglSwapBuffers(EGLSurface surface) {
        
    }
    
    
    EGLBoolean X11Display::eglTerminate() {
        
    }
    
    
    EGLBoolean X11Display::eglWaitGL(void) {
        
    }
    
    
    EGLBoolean X11Display::eglWaitNative(EGLint engine) {
        
    }
    
    
    EGLBoolean X11Display::eglBindTexImage(EGLSurface surface, EGLint buffer) {
        
    }
    
    
    EGLBoolean X11Display::eglReleaseTexImage(EGLSurface surface, EGLint buffer) {
        
    }
    
    
    EGLBoolean X11Display::eglSurfaceAttrib(EGLSurface surface, EGLint attribute, EGLint value) {
        
    }
    
    
    EGLBoolean X11Display::eglSwapInterval(EGLint interval) {
        
    }
    
    
    EGLSurface X11Display::eglCreatePbufferFromClientBuffer(EGLenum buftype, EGLClientBuffer buffer, EGLConfig config, const EGLint* attrib_list) {
        
    }
    
    
    EGLBoolean X11Display::eglReleaseThread(void) {
        
    }
    
    
    EGLBoolean X11Display::eglWaitClient(void) {
        
    }
    
    
    EGLContext X11Display::eglGetCurrentContext(void) {
        
    }
    
    
    EGLSync X11Display::eglCreateSync(EGLenum type, const EGLAttrib* attrib_list) {
        
    }
    
    
    EGLBoolean X11Display::eglDestroySync(EGLSync sync) {
        
    }
    
    
    EGLint X11Display::eglClientWaitSync(EGLSync sync, EGLint flags, EGLTime timeout) {
        
    }
    
    
    EGLBoolean X11Display::eglGetSyncAttrib(EGLSync sync, EGLint attribute, EGLAttrib* value) {
        
    }
    
    
    EGLImage X11Display::eglCreateImage(EGLContext ctx, EGLenum target, EGLClientBuffer buffer, const EGLAttrib* attrib_list) {
        
    }
    
    
    EGLBoolean X11Display::eglDestroyImage(EGLImage image) {
        
    }
    
    
    EGLSurface X11Display::eglCreatePlatformWindowSurface(EGLConfig config, void* native_window, const EGLAttrib* attrib_list) {
        
    }
    
    
    EGLSurface X11Display::eglCreatePlatformPixmapSurface(EGLConfig config, void* native_pixmap, const EGLAttrib* attrib_list) {
        
    }
    
    
    EGLBoolean X11Display::eglWaitSync(EGLSync sync, EGLint flags) {
        
    }
    
    
    
}

