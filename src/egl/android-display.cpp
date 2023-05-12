#include "egl.hpp"

namespace egl_wrapper {
    EGLBoolean AndroidDisplay::eglChooseConfig(const EGLint* attrib_list, EGLConfig* configs, EGLint config_size, EGLint* num_config) {
        
    }
    
    
    EGLBoolean AndroidDisplay::eglCopyBuffers(EGLSurface surface, EGLNativePixmapType target) {
        
    }
    
    
    EGLContext AndroidDisplay::eglCreateContext(EGLConfig config, EGLContext share_context, const EGLint* attrib_list) {
        
    }
    
    
    EGLSurface AndroidDisplay::eglCreatePbufferSurface(EGLConfig config, const EGLint* attrib_list) {
        
    }
    
    
    EGLSurface AndroidDisplay::eglCreatePixmapSurface(EGLConfig config, EGLNativePixmapType pixmap, const EGLint* attrib_list) {
        
    }
    
    
    EGLSurface AndroidDisplay::eglCreateWindowSurface(EGLConfig config, EGLNativeWindowType win, const EGLint* attrib_list) {
        
    }
    
    
    EGLBoolean AndroidDisplay::eglDestroyContext(EGLContext ctx) {
        
    }
    
    
    EGLBoolean AndroidDisplay::eglDestroySurface(EGLSurface surface) {
        
    }
    
    
    EGLBoolean AndroidDisplay::eglGetConfigAttrib(EGLConfig config, EGLint attribute, EGLint* value) {
        
    }
    
    
    EGLBoolean AndroidDisplay::eglGetConfigs(EGLConfig* configs, EGLint config_size, EGLint* num_config) {
        
    }
    
    
    EGLBoolean AndroidDisplay::eglInitialize(EGLint* major, EGLint* minor) {
        
    }
    
    
    EGLBoolean AndroidDisplay::eglQueryContext(EGLContext ctx, EGLint attribute, EGLint* value) {
        
    }
    
    
    const char* AndroidDisplay::eglQueryString(EGLint name) {
        
    }
    
    
    EGLBoolean AndroidDisplay::eglQuerySurface(EGLSurface surface, EGLint attribute, EGLint* value) {
        
    }
    
    
    EGLBoolean AndroidDisplay::eglSwapBuffers(EGLSurface surface) {
        
    }
    
    
    EGLBoolean AndroidDisplay::eglTerminate() {
        
    }
    
    
    EGLBoolean AndroidDisplay::eglWaitGL(void) {
        
    }
    
    
    EGLBoolean AndroidDisplay::eglWaitNative(EGLint engine) {
        
    }
    
    
    EGLBoolean AndroidDisplay::eglBindTexImage(EGLSurface surface, EGLint buffer) {
        
    }
    
    
    EGLBoolean AndroidDisplay::eglReleaseTexImage(EGLSurface surface, EGLint buffer) {
        
    }
    
    
    EGLBoolean AndroidDisplay::eglSurfaceAttrib(EGLSurface surface, EGLint attribute, EGLint value) {
        
    }
    
    
    EGLBoolean AndroidDisplay::eglSwapInterval(EGLint interval) {
        
    }
    
    
    EGLSurface AndroidDisplay::eglCreatePbufferFromClientBuffer(EGLenum buftype, EGLClientBuffer buffer, EGLConfig config, const EGLint* attrib_list) {
        
    }
    
    
    EGLBoolean AndroidDisplay::eglReleaseThread(void) {
        
    }
    
    
    EGLBoolean AndroidDisplay::eglWaitClient(void) {
        
    }
    
    
    EGLContext AndroidDisplay::eglGetCurrentContext(void) {
        
    }
    
    
    EGLSync AndroidDisplay::eglCreateSync(EGLenum type, const EGLAttrib* attrib_list) {
        
    }
    
    
    EGLBoolean AndroidDisplay::eglDestroySync(EGLSync sync) {
        
    }
    
    
    EGLint AndroidDisplay::eglClientWaitSync(EGLSync sync, EGLint flags, EGLTime timeout) {
        
    }
    
    
    EGLBoolean AndroidDisplay::eglGetSyncAttrib(EGLSync sync, EGLint attribute, EGLAttrib* value) {
        
    }
    
    
    EGLImage AndroidDisplay::eglCreateImage(EGLContext ctx, EGLenum target, EGLClientBuffer buffer, const EGLAttrib* attrib_list) {
        
    }
    
    
    EGLBoolean AndroidDisplay::eglDestroyImage(EGLImage image) {
        
    }
    
    
    EGLSurface AndroidDisplay::eglCreatePlatformWindowSurface(EGLConfig config, void* native_window, const EGLAttrib* attrib_list) {
        
    }
    
    
    EGLSurface AndroidDisplay::eglCreatePlatformPixmapSurface(EGLConfig config, void* native_pixmap, const EGLAttrib* attrib_list) {
        
    }
    
    
    EGLBoolean AndroidDisplay::eglWaitSync(EGLSync sync, EGLint flags) {
        
    }
    
    
}

