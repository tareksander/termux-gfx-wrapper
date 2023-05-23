#include "egl.hpp"


// Short for for casting the EGLDisplay and calling a function.
#define CALLD(name) std::unique_lock<std::mutex> l{reinterpret_cast<EGLDisplayBackend*>(d)->lock}; return reinterpret_cast<EGLDisplayBackend*>(d)->name


namespace egl_wrapper::dispatch {
    // EGL 1.0
    EGLBoolean eglChooseConfig(EGLDisplay d, const EGLint* attrib_list, EGLConfig* configs, EGLint config_size, EGLint* num_config) {
        if (d == EGL_NO_DISPLAY) {
            lastError = EGL_BAD_DISPLAY;
            return EGL_FALSE;
        }
        try {
            CALLD(eglChooseConfig)(attrib_list, configs, config_size, num_config);
        } CATCH_EGL_EXCEPTIONS
        return EGL_FALSE;
    }
    
    
    EGLBoolean eglCopyBuffers(EGLDisplay d, EGLSurface surface, EGLNativePixmapType target) {
        if (d == EGL_NO_DISPLAY) {
            lastError = EGL_BAD_DISPLAY;
            return EGL_FALSE;
        }
        try {
            CALLD(eglCopyBuffers)(surface, target);
        } CATCH_EGL_EXCEPTIONS
        return EGL_FALSE;
    }
    
    
    EGLContext eglCreateContext(EGLDisplay d, EGLConfig config, EGLContext share_context, const EGLint* attrib_list) {
        if (d == EGL_NO_DISPLAY) {
            lastError = EGL_BAD_DISPLAY;
            return EGL_FALSE;
        }
        try {
            CALLD(eglCreateContext)(config, share_context, attrib_list);
        } CATCH_EGL_EXCEPTIONS
        return EGL_NO_CONTEXT;
    }
    
    
    EGLSurface eglCreatePbufferSurface(EGLDisplay d, EGLConfig config, const EGLint* attrib_list) {
        if (d == EGL_NO_DISPLAY) {
            lastError = EGL_BAD_DISPLAY;
            return EGL_FALSE;
        }
        try {
            CALLD(eglCreatePbufferSurface)(config, attrib_list);
        } CATCH_EGL_EXCEPTIONS
        return EGL_NO_SURFACE;
    }
    
    
    EGLSurface eglCreatePixmapSurface(EGLDisplay d, EGLConfig config, EGLNativePixmapType pixmap, const EGLint* attrib_list) {
        if (d == EGL_NO_DISPLAY) {
            lastError = EGL_BAD_DISPLAY;
            return EGL_FALSE;
        }
        try {
            CALLD(eglCreatePixmapSurface)(config, pixmap, attrib_list);
        } CATCH_EGL_EXCEPTIONS
        return EGL_NO_SURFACE;
    }
    
    
    EGLSurface eglCreateWindowSurface(EGLDisplay d, EGLConfig config, EGLNativeWindowType win, const EGLint* attrib_list) {
        if (d == EGL_NO_DISPLAY) {
            lastError = EGL_BAD_DISPLAY;
            return EGL_FALSE;
        }
        try {
            CALLD(eglCreateWindowSurface)(config, win, attrib_list);
        } CATCH_EGL_EXCEPTIONS
        return EGL_NO_SURFACE;
    }
    
    
    EGLBoolean eglDestroyContext(EGLDisplay d, EGLContext ctx) {
        if (d == EGL_NO_DISPLAY) {
            lastError = EGL_BAD_DISPLAY;
            return EGL_FALSE;
        }
        try {
            CALLD(eglDestroyContext)(ctx);
        } CATCH_EGL_EXCEPTIONS
        return EGL_FALSE;
    }
    
    
    EGLBoolean eglDestroySurface(EGLDisplay d, EGLSurface surface) {
        if (d == EGL_NO_DISPLAY) {
            lastError = EGL_BAD_DISPLAY;
            return EGL_FALSE;
        }
        try {
            CALLD(eglDestroySurface)(surface);
        } CATCH_EGL_EXCEPTIONS
        return EGL_FALSE;
    }
    
    
    EGLBoolean eglGetConfigAttrib(EGLDisplay d, EGLConfig config, EGLint attribute, EGLint* value) {
        if (d == EGL_NO_DISPLAY) {
            lastError = EGL_BAD_DISPLAY;
            return EGL_FALSE;
        }
        try {
            CALLD(eglGetConfigAttrib)(config, attribute, value);
        } CATCH_EGL_EXCEPTIONS
        return EGL_FALSE;
    }
    
    
    EGLBoolean eglGetConfigs(EGLDisplay d, EGLConfig* configs, EGLint config_size, EGLint* num_config) {
        if (d == EGL_NO_DISPLAY) {
            lastError = EGL_BAD_DISPLAY;
            return EGL_FALSE;
        }
        try {
            CALLD(eglGetConfigs)(configs, config_size, num_config);
        } CATCH_EGL_EXCEPTIONS
        return EGL_FALSE;
    }
    
    
    EGLint eglGetError(void) {
        EGLint e = lastError;
        lastError = EGL_SUCCESS;
        return e;
    }
    
    
    EGLBoolean eglInitialize(EGLDisplay d, EGLint* major, EGLint* minor) {
        if (d == EGL_NO_DISPLAY) {
            lastError = EGL_BAD_DISPLAY;
            return EGL_FALSE;
        }
        try {
            CALLD(eglInitialize)(major, minor);
        } CATCH_EGL_EXCEPTIONS
        return EGL_FALSE;
    }
    
    
    EGLBoolean eglMakeCurrent(EGLDisplay d, EGLSurface draw, EGLSurface read, EGLContext ctx) {
        if (d == EGL_NO_DISPLAY) {
            lastError = EGL_BAD_DISPLAY;
            return EGL_FALSE;
        }
        try {
            CALLD(eglMakeCurrent)(draw, read, ctx);
        } CATCH_EGL_EXCEPTIONS
        return EGL_FALSE;
    }
    
    
    EGLBoolean eglQueryContext(EGLDisplay d, EGLContext ctx, EGLint attribute, EGLint* value) {
        if (d == EGL_NO_DISPLAY) {
            lastError = EGL_BAD_DISPLAY;
            return EGL_FALSE;
        }
        try {
            CALLD(eglQueryContext)(ctx, attribute, value);
        } CATCH_EGL_EXCEPTIONS
        return EGL_FALSE;
    }
    
    
    const char* eglQueryString(EGLDisplay d, EGLint name) {
        if (d == EGL_NO_DISPLAY) {
            return PLATFORMS_STRING;
        }
        try {
            CALLD(eglQueryString)(name);
        } CATCH_EGL_EXCEPTIONS
        return NULL;
    }
    
    
    EGLBoolean eglQuerySurface(EGLDisplay d, EGLSurface surface, EGLint attribute, EGLint* value) {
        if (d == EGL_NO_DISPLAY) {
            lastError = EGL_BAD_DISPLAY;
            return EGL_FALSE;
        }
        try {
            CALLD(eglQuerySurface)(surface, attribute, value);
        } CATCH_EGL_EXCEPTIONS
        return EGL_FALSE;
    }
    
    
    EGLBoolean eglSwapBuffers(EGLDisplay d, EGLSurface surface) {
        if (d == EGL_NO_DISPLAY) {
            lastError = EGL_BAD_DISPLAY;
            return EGL_FALSE;
        }
        try {
            CALLD(eglSwapBuffers)(surface);
        } CATCH_EGL_EXCEPTIONS
        return EGL_FALSE;
    }
    
    
    EGLBoolean eglTerminate(EGLDisplay d) {
        if (d == EGL_NO_DISPLAY) {
            lastError = EGL_BAD_DISPLAY;
            return EGL_FALSE;
        }
        try {
            CALLD(eglTerminate)();
        } CATCH_EGL_EXCEPTIONS
        return EGL_FALSE;
    }
    
    
    EGLBoolean eglWaitGL(void) {
        return real_eglWaitGL();
    }
    
    
    EGLBoolean eglWaitNative(EGLint engine) {
        return real_eglWaitNative(engine);
    }
    
    
    
    // EGL 1.1
    EGLBoolean eglBindTexImage(EGLDisplay d, EGLSurface surface, EGLint buffer) {
        if (d == EGL_NO_DISPLAY) {
            lastError = EGL_BAD_DISPLAY;
            return EGL_FALSE;
        }
        try {
            CALLD(eglBindTexImage)(surface, buffer);
        } CATCH_EGL_EXCEPTIONS
        return EGL_FALSE;
    }
    
    
    EGLBoolean eglReleaseTexImage(EGLDisplay d, EGLSurface surface, EGLint buffer) {
        if (d == EGL_NO_DISPLAY) {
            lastError = EGL_BAD_DISPLAY;
            return EGL_FALSE;
        }
        try {
            CALLD(eglReleaseTexImage)(surface, buffer);
        } CATCH_EGL_EXCEPTIONS
        return EGL_FALSE;
    }
    
    
    EGLBoolean eglSurfaceAttrib(EGLDisplay d, EGLSurface surface, EGLint attribute, EGLint value) {
        if (d == EGL_NO_DISPLAY) {
            lastError = EGL_BAD_DISPLAY;
            return EGL_FALSE;
        }
        try {
            CALLD(eglSurfaceAttrib)(surface, attribute, value);
        } CATCH_EGL_EXCEPTIONS
        return EGL_FALSE;
    }
    
    
    EGLBoolean eglSwapInterval(EGLDisplay d, EGLint interval) {
        if (d == EGL_NO_DISPLAY) {
            lastError = EGL_BAD_DISPLAY;
            return EGL_FALSE;
        }
        try {
            CALLD(eglSwapInterval)(interval);
        } CATCH_EGL_EXCEPTIONS
        return EGL_FALSE;
    }
    
    
    
    // EGL 1.2
    EGLBoolean eglBindAPI(EGLenum api) {
        // return value not used in libglvnd, but just to be sure
        return getSupportsAPI(api);
    }
    
    EGLSurface eglCreatePbufferFromClientBuffer(EGLDisplay d, EGLenum buftype, EGLClientBuffer buffer, EGLConfig config, const EGLint* attrib_list) {
        return EGL_NO_SURFACE;
    }
    
    
    EGLBoolean eglReleaseThread(void) {
        // TODO relay to all displays that are initialized, if needed
        return EGL_TRUE;
    }
    
    
    EGLBoolean eglWaitClient(void) {
        return real_eglWaitClient();
    }
    
    
    // EGL 1.5
    EGLSync eglCreateSync(EGLDisplay d, EGLenum type, const EGLAttrib* attrib_list) {
        if (d == EGL_NO_DISPLAY) {
            lastError = EGL_BAD_DISPLAY;
            return EGL_FALSE;
        }
        try {
            CALLD(eglCreateSync)(type, attrib_list);
        } CATCH_EGL_EXCEPTIONS
        return EGL_NO_SYNC;
    }
    
    
    EGLBoolean eglDestroySync(EGLDisplay d, EGLSync sync) {
        if (d == EGL_NO_DISPLAY) {
            lastError = EGL_BAD_DISPLAY;
            return EGL_FALSE;
        }
        try {
            CALLD(eglDestroySync)(sync);
        } CATCH_EGL_EXCEPTIONS
        return EGL_FALSE;
    }
    
    
    EGLint eglClientWaitSync(EGLDisplay d, EGLSync sync, EGLint flags, EGLTime timeout) {
        if (d == EGL_NO_DISPLAY) {
            lastError = EGL_BAD_DISPLAY;
            return EGL_FALSE;
        }
        try {
            CALLD(eglClientWaitSync)(sync, flags, timeout);
        } CATCH_EGL_EXCEPTIONS
        return EGL_FALSE;
    }
    
    
    EGLBoolean eglGetSyncAttrib(EGLDisplay d, EGLSync sync, EGLint attribute, EGLAttrib* value) {
        if (d == EGL_NO_DISPLAY) {
            lastError = EGL_BAD_DISPLAY;
            return EGL_FALSE;
        }
        try {
            CALLD(eglGetSyncAttrib)(sync, attribute, value);
        } CATCH_EGL_EXCEPTIONS
        return EGL_FALSE;
    }
    
    
    EGLImage eglCreateImage(EGLDisplay d, EGLContext ctx, EGLenum target, EGLClientBuffer buffer, const EGLAttrib* attrib_list) {
        if (d == EGL_NO_DISPLAY) {
            lastError = EGL_BAD_DISPLAY;
            return EGL_FALSE;
        }
        try {
            CALLD(eglCreateImage)(ctx, target, buffer, attrib_list);
        } CATCH_EGL_EXCEPTIONS
        return EGL_NO_IMAGE;
    }
    
    
    EGLBoolean eglDestroyImage(EGLDisplay d, EGLImage image) {
        if (d == EGL_NO_DISPLAY) {
            lastError = EGL_BAD_DISPLAY;
            return EGL_FALSE;
        }
        try {
            CALLD(eglDestroyImage)(image);
        } CATCH_EGL_EXCEPTIONS
        return EGL_FALSE;
    }
    
    
    EGLSurface eglCreatePlatformWindowSurface(EGLDisplay d, EGLConfig config, void* native_window, const EGLAttrib* attrib_list) {
        if (d == EGL_NO_DISPLAY) {
            lastError = EGL_BAD_DISPLAY;
            return EGL_FALSE;
        }
        try {
            CALLD(eglCreatePlatformWindowSurface)(config, native_window, attrib_list);
        } CATCH_EGL_EXCEPTIONS
        return EGL_NO_SURFACE;
    }
    
    
    EGLSurface eglCreatePlatformPixmapSurface(EGLDisplay d, EGLConfig config, void* native_pixmap, const EGLAttrib* attrib_list) {
        if (d == EGL_NO_DISPLAY) {
            lastError = EGL_BAD_DISPLAY;
            return EGL_FALSE;
        }
        try {
            CALLD(eglCreatePlatformPixmapSurface)(config, native_pixmap, attrib_list);
        } CATCH_EGL_EXCEPTIONS
        return EGL_NO_SURFACE;
    }
    
    
    EGLBoolean eglWaitSync(EGLDisplay d, EGLSync sync, EGLint flags) {
        if (d == EGL_NO_DISPLAY) {
            lastError = EGL_BAD_DISPLAY;
            return EGL_FALSE;
        }
        try {
            CALLD(eglWaitSync)(sync, flags);
        } CATCH_EGL_EXCEPTIONS
        return EGL_FALSE;
    }
    
    EGLImageKHR eglCreateImageKHR(EGLDisplay dpy, EGLContext ctx, EGLenum target, EGLClientBuffer buffer, const EGLint* attrib_list) {
        glvnd->threadInit();
        if (dpy == EGL_NO_DISPLAY) {
            glvnd->setEGLError(EGL_BAD_DISPLAY);
            return EGL_NO_IMAGE_KHR;
        }
        auto vnd = glvnd->getVendorFromDisplay(dpy);
        if (vnd == thisVendor) {
            // TODO dispatch to display
        } else {
            PFNEGLCREATEIMAGEKHRPROC f = (PFNEGLCREATEIMAGEKHRPROC) glvnd->fetchDispatchEntry(vnd, eglCreateImageKHRIndex);
            if (f == NULL) {
                glvnd->setEGLError(EGL_BAD_DISPLAY);
                return EGL_NO_IMAGE_KHR;
            }
            return f(dpy, ctx, target, buffer, attrib_list);
        }

        return nullptr;
    }
    
    EGLBoolean eglDestroyImageKHR(EGLDisplay dpy, EGLImageKHR image) {
        glvnd->threadInit();
        if (dpy == EGL_NO_DISPLAY) {
            glvnd->setEGLError(EGL_BAD_DISPLAY);
            return EGL_FALSE;
        }

        auto vnd = glvnd->getVendorFromDisplay(dpy);
        if (vnd == thisVendor) {
            // TODO dispatch to display
        } else {
            auto f = (PFNEGLDESTROYIMAGEKHRPROC) glvnd->fetchDispatchEntry(vnd, eglDestroyImageKHRIndex);
            if (f == nullptr) {
                glvnd->setEGLError(EGL_BAD_DISPLAY);
                return EGL_FALSE;
            }

            return f(dpy, image);
        }
    }
    
    
    EGLClientBuffer eglCreateNativeClientBufferANDROID(const EGLint* attrib_list) {
        glvnd->threadInit();
        // no display, dispatch not possible
        auto ret = real_eglCreateNativeClientBufferANDROID(attrib_list);
        lastError = real_eglGetError();
        return ret;
    }
    
    
    
    EGLClientBuffer eglGetNativeClientBufferANDROID(const struct AHardwareBuffer* buffer) {
        glvnd->threadInit();
        // no display, dispatch not possible
        auto ret = real_eglGetNativeClientBufferANDROID(buffer);
        lastError = real_eglGetError();
        return ret;
    }
    
    
}

