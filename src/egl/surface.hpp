#pragma once

#define EGL_EGL_PROTOTYPES 0
#include <EGL/egl.h>

#include <memory>

namespace egl_wrapper {
    
    struct SmartEGLSurface final {
        EGLSurface s;
        
        operator EGLSurface() {
            return s;
        }
        
        SmartEGLSurface() : s{EGL_NO_SURFACE} {}
        SmartEGLSurface(EGLSurface s) : s{s} {}
        ~SmartEGLSurface();
        SmartEGLSurface(const SmartEGLSurface& o) = delete;
        SmartEGLSurface& operator=(const SmartEGLSurface& o) = delete;
        SmartEGLSurface& operator=(SmartEGLSurface&& o);
        SmartEGLSurface(SmartEGLSurface&& o) {
            *this = std::move(o);
        }
    };
    
    
    /**
     * Possible backends for a Surface.
     */
    struct SurfaceBackend {
        enum class Type {
            PBUFFER, HWBUFFER
        };
        Type type;
        
        virtual ~SurfaceBackend() = 0;
    };
    
    
    struct PBufferSurfaceBackend : SurfaceBackend {
        SmartEGLSurface pbuffer;
        int width, height;
        virtual ~PBufferSurfaceBackend() {};
    };
    
    
    /// Base class for implementation Surfaces.
    struct Surface {
        enum class Type {
            WINDOW, PIXMAP, PBUFFER
        };
        Type type;
        /// The actual surface
        std::unique_ptr<SurfaceBackend> backend;
        
        
        virtual ~Surface() {}
        
        static inline EGLSurface getSurface(Surface* s) {
            if (s == EGL_NO_SURFACE) return EGL_NO_SURFACE;
            if (s->backend->type == SurfaceBackend::Type::PBUFFER) {
                return static_cast<PBufferSurfaceBackend*>(s->backend.get())->pbuffer;
            }
            return EGL_NO_SURFACE;
        }
    };
    
    
}


