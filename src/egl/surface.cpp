#include "surface.hpp"
#include "egl.hpp"

namespace egl_wrapper {
    SurfaceBackend::~SurfaceBackend() = default;
    
    SmartEGLImage::~SmartEGLImage() {
        if (i != EGL_NO_IMAGE) {
            real_eglDestroyImageKHR(nativeDisplay, i);
        }
    }
    
    SmartEGLImage& SmartEGLImage::operator=(SmartEGLImage&& o) {
        if (this != &o) {
            i = o.i;
            o.i = EGL_NO_IMAGE_KHR;
        }
        return *this;
    }
    
    SmartEGLSurface::~SmartEGLSurface() {
        if (s != EGL_NO_SURFACE)
            real_eglDestroySurface(nativeDisplay, s);
    }
    
    SmartEGLSurface& SmartEGLSurface::operator=(SmartEGLSurface&& o)  noexcept {
        if (this != &o) {
            if (s != EGL_NO_SURFACE) {
                real_eglDestroySurface(nativeDisplay, s);
            }
            s = o.s;
            o.s = EGL_NO_SURFACE;
        }
        return *this;
    }
    
    SmartHardwareBuffer::~SmartHardwareBuffer() {
        if (hb != nullptr) {
            libandroid.AHardwareBuffer_release(hb);
        }
    }
    
    SmartHardwareBuffer& SmartHardwareBuffer::operator=(SmartHardwareBuffer&& o) noexcept {
        if (this != &o) {
            if (hb != nullptr) {
                libandroid.AHardwareBuffer_release(hb);
            }
            hb = o.hb;
            o.hb = nullptr;
        }
        return *this;
    }
    
    SmartHardwareBuffer& SmartHardwareBuffer::operator=(const SmartHardwareBuffer& o) {
        if (this != &o) {
            if (hb != nullptr) {
                libandroid.AHardwareBuffer_release(hb);
            }
            hb = o.hb;
            if (hb != nullptr)
                libandroid.AHardwareBuffer_acquire((AHardwareBuffer*)hb);
        }
        return *this;
    }
    
    SmartHardwareBuffer::SmartHardwareBuffer(const SmartHardwareBuffer& o) {
        *this = o;
    }
    
    
    HardwareBufferSurfaceBackend::~HardwareBufferSurfaceBackend() {
        if (gl.lastContext != EGL_NO_CONTEXT) {
            EGLSurface draw = real_eglGetCurrentSurface(EGL_DRAW);
            EGLSurface read = real_eglGetCurrentSurface(EGL_READ);
            EGLContext ctx = real_eglGetCurrentContext();
            
            real_eglMakeCurrent(nativeDisplay, dummy, dummy, gl.lastContext);
            // if making the context current doesn't work, it's probably destroyed already, so no cleanup necessary
            if (real_eglGetError() != EGL_SUCCESS) return;
            real_glDeleteRenderbuffers(2, (GLuint*)gl.renderbuffer);
            real_glDeleteRenderbuffers(2, (GLuint*)gl.renderbufferDepth);
            real_glDeleteFramebuffers(2, (GLuint*)gl.framebuffer);
            
            real_eglMakeCurrent(nativeDisplay, draw, read, ctx);
        }
    }
    
}
