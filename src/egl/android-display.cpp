#include "egl.hpp"

namespace egl_wrapper {
    
    
    
    
    
    EGLBoolean AndroidDisplay::eglChooseConfig(const EGLint* attrib_list, EGLConfig* configs, EGLint config_size, EGLint* num_config) {
        auto retVal = real_eglChooseConfig(nativeDisplay, attrib_list, configs, config_size, num_config);
        lastError = real_eglGetError();
        return retVal;
    }
    
    
    EGLBoolean AndroidDisplay::eglCopyBuffers(EGLSurface surface, EGLNativePixmapType target) {
        auto retVal = real_eglCopyBuffers(nativeDisplay, surface, target);
        lastError = real_eglGetError();
        return retVal;
    }
    
    
    EGLContext AndroidDisplay::eglCreateContext(EGLConfig config, EGLContext share_context, const EGLint* attrib_list) {
        auto retVal = real_eglCreateContext(nativeDisplay, config, share_context, attrib_list);
        lastError = real_eglGetError();
        return retVal;
    }
    
    
    EGLSurface AndroidDisplay::eglCreatePbufferSurface(EGLConfig config, const EGLint* attrib_list) {
        auto retVal = real_eglCreatePbufferSurface(nativeDisplay, config, attrib_list);
        lastError = real_eglGetError();
        return retVal;
    }
    
    
    EGLSurface AndroidDisplay::eglCreatePixmapSurface(EGLConfig config, EGLNativePixmapType pixmap, const EGLint* attrib_list) {
        auto retVal = real_eglCreatePixmapSurface(nativeDisplay, config, pixmap, attrib_list);
        lastError = real_eglGetError();
        return retVal;
    }
    
    
    EGLSurface AndroidDisplay::eglCreateWindowSurface(EGLConfig config, EGLNativeWindowType win, const EGLint* attrib_list) {
        auto retVal = real_eglCreateWindowSurface(nativeDisplay, config, win, attrib_list);
        lastError = real_eglGetError();
        return retVal;
    }
    
    
    EGLBoolean AndroidDisplay::eglDestroyContext(EGLContext ctx) {
        auto retVal = real_eglDestroyContext(nativeDisplay, ctx);
        lastError = real_eglGetError();
        return retVal;
    }
    
    
    EGLBoolean AndroidDisplay::eglDestroySurface(EGLSurface surface) {
        auto retVal = real_eglDestroySurface(nativeDisplay, surface);
        lastError = real_eglGetError();
        return retVal;
    }
    
    
    EGLBoolean AndroidDisplay::eglGetConfigAttrib(EGLConfig config, EGLint attribute, EGLint* value) {
        auto retVal = real_eglGetConfigAttrib(nativeDisplay, config, attribute, value);
        lastError = real_eglGetError();
        return retVal;
    }
    
    
    EGLBoolean AndroidDisplay::eglGetConfigs(EGLConfig* configs, EGLint config_size, EGLint* num_config) {
        auto retVal = real_eglGetConfigs(nativeDisplay, configs, config_size, num_config);
        lastError = real_eglGetError();
        return retVal;
    }
    
    
    EGLBoolean AndroidDisplay::eglInitialize(EGLint* major, EGLint* minor) {
        auto retVal = real_eglInitialize(nativeDisplay, major, minor);
        lastError = real_eglGetError();
        return retVal;
    }
    
    
    EGLBoolean AndroidDisplay::eglQueryContext(EGLContext ctx, EGLint attribute, EGLint* value) {
        auto retVal = real_eglQueryContext(nativeDisplay, ctx, attribute, value);
        lastError = real_eglGetError();
        return retVal;
    }
    
    
    const char* AndroidDisplay::eglQueryString(EGLint name) {
        auto retVal = real_eglQueryString(nativeDisplay, name);
        lastError = real_eglGetError();
        return retVal;
    }
    
    
    EGLBoolean AndroidDisplay::eglQuerySurface(EGLSurface surface, EGLint attribute, EGLint* value) {
        auto retVal = real_eglQuerySurface(nativeDisplay, surface, attribute, value);
        lastError = real_eglGetError();
        return retVal;
    }
    
    
    EGLBoolean AndroidDisplay::eglSwapBuffers(EGLSurface surface) {
        auto retVal = real_eglSwapBuffers(nativeDisplay, surface);
        lastError = real_eglGetError();
        return retVal;
    }
    
    
    EGLBoolean AndroidDisplay::eglTerminate() {
        // TODO bookkeeping and freeing of EGL resources
    }
    
    
    EGLBoolean AndroidDisplay::eglWaitGL(void) {
        auto retVal = real_eglWaitGL();
        lastError = real_eglGetError();
        return retVal;
    }
    
    
    EGLBoolean AndroidDisplay::eglWaitNative(EGLint engine) {
        auto retVal = real_eglWaitNative(engine);
        lastError = real_eglGetError();
        return retVal;
    }
    
    
    EGLBoolean AndroidDisplay::eglBindTexImage(EGLSurface surface, EGLint buffer) {
        auto retVal = real_eglBindTexImage(nativeDisplay, surface, buffer);
        lastError = real_eglGetError();
        return retVal;
    }
    
    
    EGLBoolean AndroidDisplay::eglReleaseTexImage(EGLSurface surface, EGLint buffer) {
        auto retVal = real_eglReleaseTexImage(nativeDisplay, surface, buffer);
        lastError = real_eglGetError();
        return retVal;
    }
    
    
    EGLBoolean AndroidDisplay::eglSurfaceAttrib(EGLSurface surface, EGLint attribute, EGLint value) {
        auto retVal = real_eglSurfaceAttrib(nativeDisplay, surface, attribute, value);
        lastError = real_eglGetError();
        return retVal;
    }
    
    
    EGLBoolean AndroidDisplay::eglSwapInterval(EGLint interval) {
        auto retVal = real_eglSwapInterval(nativeDisplay, interval);
        lastError = real_eglGetError();
        return retVal;
    }
    
    
    EGLSurface AndroidDisplay::eglCreatePbufferFromClientBuffer(EGLenum buftype, EGLClientBuffer buffer, EGLConfig config, const EGLint* attrib_list) {
        auto retVal = real_eglCreatePbufferFromClientBuffer(nativeDisplay, buftype, buffer, config, attrib_list);
        lastError = real_eglGetError();
        return retVal;
    }
    
    
    EGLBoolean AndroidDisplay::eglReleaseThread(void) {
        auto retVal = real_eglReleaseThread();
        lastError = real_eglGetError();
        return retVal;
    }
    
    
    EGLBoolean AndroidDisplay::eglWaitClient(void) {
        auto retVal = real_eglWaitClient();
        lastError = real_eglGetError();
        return retVal;
    }
    
    
    EGLContext AndroidDisplay::eglGetCurrentContext(void) {
        auto retVal = real_eglGetCurrentContext();
        lastError = real_eglGetError();
        return retVal;
    }
    
    
    EGLSync AndroidDisplay::eglCreateSync(EGLenum type, const EGLAttrib* attrib_list) {
        auto retVal = real_eglCreateSync(nativeDisplay, type, attrib_list);
        lastError = real_eglGetError();
        return retVal;
    }
    
    
    EGLBoolean AndroidDisplay::eglDestroySync(EGLSync sync) {
        auto retVal = real_eglDestroySync(nativeDisplay, sync);
        lastError = real_eglGetError();
        return retVal;
    }
    
    
    EGLint AndroidDisplay::eglClientWaitSync(EGLSync sync, EGLint flags, EGLTime timeout) {
        auto retVal = real_eglClientWaitSync(nativeDisplay, sync, flags, timeout);
        lastError = real_eglGetError();
        return retVal;
    }
    
    
    EGLBoolean AndroidDisplay::eglGetSyncAttrib(EGLSync sync, EGLint attribute, EGLAttrib* value) {
        auto retVal = real_eglGetSyncAttrib(nativeDisplay, sync, attribute, value);
        lastError = real_eglGetError();
        return retVal;
    }
    
    
    EGLImage AndroidDisplay::eglCreateImage(EGLContext ctx, EGLenum target, EGLClientBuffer buffer, const EGLAttrib* attrib_list) {
        auto retVal = real_eglCreateImage(nativeDisplay, ctx, target, buffer, attrib_list);
        lastError = real_eglGetError();
        return retVal;
    }
    
    
    EGLBoolean AndroidDisplay::eglDestroyImage(EGLImage image) {
        auto retVal = real_eglDestroyImage(nativeDisplay, image);
        lastError = real_eglGetError();
        return retVal;
    }
    
    
    EGLSurface AndroidDisplay::eglCreatePlatformWindowSurface(EGLConfig config, void* native_window, const EGLAttrib* attrib_list) {
        auto retVal = real_eglCreatePlatformWindowSurface(nativeDisplay, config, native_window, attrib_list);
        lastError = real_eglGetError();
        return retVal;
    }
    
    
    EGLSurface AndroidDisplay::eglCreatePlatformPixmapSurface(EGLConfig config, void* native_pixmap, const EGLAttrib* attrib_list) {
        auto retVal = real_eglCreatePlatformPixmapSurface(nativeDisplay, config, native_pixmap, attrib_list);
        lastError = real_eglGetError();
        return retVal;
    }
    
    
    EGLBoolean AndroidDisplay::eglWaitSync(EGLSync sync, EGLint flags) {
        auto retVal = real_eglWaitSync(nativeDisplay, sync, flags);
        lastError = real_eglGetError();
        return retVal;
    }
    
    
}

