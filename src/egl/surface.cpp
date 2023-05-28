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
            libandroid.AHardwareBuffer_acquire((AHardwareBuffer*)hb);
        }
        return *this;
    }
    
    SmartHardwareBuffer::SmartHardwareBuffer(const SmartHardwareBuffer& o) {
        *this = o;
    }
}
