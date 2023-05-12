#include "egl.hpp"


// Short for for casting the EGLDisplay and calling a function.
#define CALLD(disp, name) std::unique_lock<std::mutex> l{reinterpret_cast<EGLDisplayBackend*>(disp)->lock}; return reinterpret_cast<EGLDisplayBackend*>(disp)->name


namespace egl_wrapper::dispatch {
    // EGL 1.0
    EGLBoolean eglChooseConfig(EGLDisplay d, const EGLint* attrib_list, EGLConfig* configs, EGLint config_size, EGLint* num_config) {
        try {
            CALLD(d, eglChooseConfig)(attrib_list, configs, config_size, num_config);
        } CATCH_EGL_EXCEPTIONS
        return EGL_FALSE;
    }
    
    
    EGLBoolean eglCopyBuffers(EGLDisplay d, EGLSurface surface, EGLNativePixmapType target) {
        try {
            CALLD(d,eglCopyBuffers)(surface, target);
        } CATCH_EGL_EXCEPTIONS
        return EGL_FALSE;
    }
    
    
    EGLContext eglCreateContext(EGLDisplay d, EGLConfig config, EGLContext share_context, const EGLint* attrib_list) {
        try {
            CALLD(d, eglCreateContext)(config, share_context, attrib_list);
        } CATCH_EGL_EXCEPTIONS
        return EGL_NO_CONTEXT;
    }
    
    
    EGLSurface eglCreatePbufferSurface(EGLDisplay d, EGLConfig config, const EGLint* attrib_list) {
        try {
            CALLD(d, eglCreatePbufferSurface)(config, attrib_list);
        } CATCH_EGL_EXCEPTIONS
        return EGL_NO_SURFACE;
    }
    
    
    EGLSurface eglCreatePixmapSurface(EGLDisplay d, EGLConfig config, EGLNativePixmapType pixmap, const EGLint* attrib_list) {
        try {
            CALLD(d, eglCreatePixmapSurface)(config, pixmap, attrib_list);
        } CATCH_EGL_EXCEPTIONS
        return EGL_NO_SURFACE;
    }
    
    
    EGLSurface eglCreateWindowSurface(EGLDisplay d, EGLConfig config, EGLNativeWindowType win, const EGLint* attrib_list) {
        try {
            CALLD(d, eglCreateWindowSurface)(config, win, attrib_list);
        } CATCH_EGL_EXCEPTIONS
        return EGL_NO_SURFACE;
    }
    
    
    EGLBoolean eglDestroyContext(EGLDisplay d, EGLContext ctx) {
        try {
            CALLD(d, eglDestroyContext)(ctx);
        } CATCH_EGL_EXCEPTIONS
        return EGL_FALSE;
    }
    
    
    EGLBoolean eglDestroySurface(EGLDisplay d, EGLSurface surface) {
        try {
            CALLD(d, eglDestroySurface)(surface);
        } CATCH_EGL_EXCEPTIONS
        return EGL_FALSE;
    }
    
    
    EGLBoolean eglGetConfigAttrib(EGLDisplay d, EGLConfig config, EGLint attribute, EGLint* value) {
        try {
            CALLD(d, eglGetConfigAttrib)(config, attribute, value);
        } CATCH_EGL_EXCEPTIONS
        return EGL_FALSE;
    }
    
    
    EGLBoolean eglGetConfigs(EGLDisplay d, EGLConfig* configs, EGLint config_size, EGLint* num_config) {
        try {
            CALLD(d, eglGetConfigs)(configs, config_size, num_config);
        } CATCH_EGL_EXCEPTIONS
        return EGL_FALSE;
    }
    
    
    EGLBoolean eglInitialize(EGLDisplay d, EGLint* major, EGLint* minor) {
        try {
            CALLD(d, eglInitialize)(major, minor);
        } CATCH_EGL_EXCEPTIONS
        return EGL_FALSE;
    }
    
    
    EGLBoolean eglMakeCurrent(EGLDisplay d, EGLSurface draw, EGLSurface read, EGLContext ctx) {
        try {
            CALLD(d, eglMakeCurrent)(draw, read, ctx);
        } CATCH_EGL_EXCEPTIONS
        return EGL_FALSE;
    }
    
    
    EGLBoolean eglQueryContext(EGLDisplay d, EGLContext ctx, EGLint attribute, EGLint* value) {
        try {
            CALLD(d, eglQueryContext)(ctx, attribute, value);
        } CATCH_EGL_EXCEPTIONS
        return EGL_FALSE;
    }
    
    
    const char* eglQueryString(EGLDisplay d, EGLint name) {
        try {
            CALLD(d, eglQueryString)(name);
        } CATCH_EGL_EXCEPTIONS
        return NULL;
    }
    
    
    EGLBoolean eglQuerySurface(EGLDisplay d, EGLSurface surface, EGLint attribute, EGLint* value) {
        try {
            CALLD(d, eglQuerySurface)(surface, attribute, value);
        } CATCH_EGL_EXCEPTIONS
        return EGL_FALSE;
    }
    
    
    EGLBoolean eglSwapBuffers(EGLDisplay d, EGLSurface surface) {
        try {
            CALLD(d, eglSwapBuffers)(surface);
        } CATCH_EGL_EXCEPTIONS
        return EGL_FALSE;
    }
    
    
    EGLBoolean eglTerminate(EGLDisplay d) {
        try {
            CALLD(d, eglTerminate)();
        } CATCH_EGL_EXCEPTIONS
        return EGL_FALSE;
    }
    
    
    EGLBoolean eglWaitGL(void) {
        // TODO call waitGL on the Android display
        return EGL_TRUE;
    }
    
    
    EGLBoolean eglWaitNative(EGLint engine) {
        return EGL_FALSE;
    }
    
    
    
    // EGL 1.1
    EGLBoolean eglBindTexImage(EGLDisplay d, EGLSurface surface, EGLint buffer) {
        try {
            CALLD(d, eglBindTexImage)(surface, buffer);
        } CATCH_EGL_EXCEPTIONS
        return EGL_FALSE;
    }
    
    
    EGLBoolean eglReleaseTexImage(EGLDisplay d, EGLSurface surface, EGLint buffer) {
        try {
            CALLD(d, eglReleaseTexImage)(surface, buffer);
        } CATCH_EGL_EXCEPTIONS
        return EGL_FALSE;
    }
    
    
    EGLBoolean eglSurfaceAttrib(EGLDisplay d, EGLSurface surface, EGLint attribute, EGLint value) {
        try {
            CALLD(d, eglSurfaceAttrib)(surface, attribute, value);
        } CATCH_EGL_EXCEPTIONS
        return EGL_FALSE;
    }
    
    
    EGLBoolean eglSwapInterval(EGLDisplay d, EGLint interval) {
        try {
            CALLD(d, eglSwapInterval)(interval);
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
        // TODO relay to all displays that are initialized
        return EGL_TRUE;
    }
    
    
    EGLBoolean eglWaitClient(void) {
        // TODO call waitGL on the android display
        return EGL_TRUE;
    }
    
    
    // EGL 1.5
    EGLSync eglCreateSync(EGLDisplay d, EGLenum type, const EGLAttrib* attrib_list) {
        try {
            CALLD(d, eglCreateSync)(type, attrib_list);
        } CATCH_EGL_EXCEPTIONS
        return EGL_NO_SYNC;
    }
    
    
    EGLBoolean eglDestroySync(EGLDisplay d, EGLSync sync) {
        try {
            CALLD(d, eglDestroySync)(sync);
        } CATCH_EGL_EXCEPTIONS
        return EGL_FALSE;
    }
    
    
    EGLint eglClientWaitSync(EGLDisplay d, EGLSync sync, EGLint flags, EGLTime timeout) {
        try {
            CALLD(d, eglClientWaitSync)(sync, flags, timeout);
        } CATCH_EGL_EXCEPTIONS
        return EGL_FALSE;
    }
    
    
    EGLBoolean eglGetSyncAttrib(EGLDisplay d, EGLSync sync, EGLint attribute, EGLAttrib* value) {
        try {
            CALLD(d, eglGetSyncAttrib)(sync, attribute, value);
        } CATCH_EGL_EXCEPTIONS
        return EGL_FALSE;
    }
    
    
    EGLImage eglCreateImage(EGLDisplay d, EGLContext ctx, EGLenum target, EGLClientBuffer buffer, const EGLAttrib* attrib_list) {
        try {
            CALLD(d,eglCreateImage)(ctx, target, buffer, attrib_list);
        } CATCH_EGL_EXCEPTIONS
        return EGL_NO_IMAGE;
    }
    
    
    EGLBoolean eglDestroyImage(EGLDisplay d, EGLImage image) {
        try {
            CALLD(d, eglDestroyImage)(image);
        } CATCH_EGL_EXCEPTIONS
        return EGL_FALSE;
    }
    
    
    EGLSurface eglCreatePlatformWindowSurface(EGLDisplay d, EGLConfig config, void* native_window, const EGLAttrib* attrib_list) {
        try {
            CALLD(d, eglCreatePlatformWindowSurface)(config, native_window, attrib_list);
        } CATCH_EGL_EXCEPTIONS
        return EGL_NO_SURFACE;
    }
    
    
    EGLSurface eglCreatePlatformPixmapSurface(EGLDisplay d, EGLConfig config, void* native_pixmap, const EGLAttrib* attrib_list) {
        try {
            CALLD(d, eglCreatePlatformPixmapSurface)(config, native_pixmap, attrib_list);
        } CATCH_EGL_EXCEPTIONS
        return EGL_NO_SURFACE;
    }
    
    
    EGLBoolean eglWaitSync(EGLDisplay d, EGLSync sync, EGLint flags) {
        try {
            CALLD(d, eglWaitSync)(sync, flags);
        } CATCH_EGL_EXCEPTIONS
        return EGL_FALSE;
    }
    
    
}

