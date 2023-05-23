#include "surface.hpp"
#include "egl.hpp"

namespace egl_wrapper {
    SurfaceBackend::~SurfaceBackend() = default;
    
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
}
