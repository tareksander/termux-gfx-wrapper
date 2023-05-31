#pragma once

#define EGL_EGL_PROTOTYPES 0
#include <EGL/egl.h>
#include <EGL/eglext.h>

#include <GLES2/gl2.h>

#include <memory>

#include "android.hpp"

namespace egl_wrapper {
    
    struct SmartEGLImage final {
        EGLImageKHR i;
        
        operator EGLImage() {
            return i;
        }
        
        SmartEGLImage() : i{EGL_NO_IMAGE} {}
        SmartEGLImage(EGLImageKHR i) : i{i} {}
        ~SmartEGLImage();
        SmartEGLImage(const SmartEGLImage& o) = delete;
        SmartEGLImage& operator=(const SmartEGLImage& o) = delete;
        SmartEGLImage& operator=(SmartEGLImage&& o);
        SmartEGLImage(SmartEGLImage&& o) {
            *this = std::move(o);
        }
    };
    
    struct SmartEGLSurface final {
        EGLSurface s{};
        
        operator EGLSurface() {
            return s;
        }
        
        SmartEGLSurface() : s{EGL_NO_SURFACE} {}
        SmartEGLSurface(EGLSurface s) : s{s} {}
        ~SmartEGLSurface();
        SmartEGLSurface(const SmartEGLSurface& o) = delete;
        SmartEGLSurface& operator=(const SmartEGLSurface& o) = delete;
        SmartEGLSurface& operator=(SmartEGLSurface&& o) noexcept;
        SmartEGLSurface(SmartEGLSurface&& o) {
            *this = std::move(o);
        }
    };
    
    struct SmartHardwareBuffer final {
        AHardwareBuffer* hb;
        
        operator AHardwareBuffer*() {
            return hb;
        }
        
        SmartHardwareBuffer() : hb{nullptr} {}
        explicit SmartHardwareBuffer(AHardwareBuffer* hb) : hb{hb} {}
        ~SmartHardwareBuffer();
        SmartHardwareBuffer(const SmartHardwareBuffer& o);
        SmartHardwareBuffer& operator=(const SmartHardwareBuffer& o);
        SmartHardwareBuffer& operator=(SmartHardwareBuffer&& o) noexcept;
        SmartHardwareBuffer(SmartHardwareBuffer&& o) {
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
        
        SurfaceBackend(Type type) : type{type} {}
        
        virtual ~SurfaceBackend() = 0;
    };

    struct PBufferSurfaceBackend : SurfaceBackend {
        SmartEGLSurface pbuffer = EGL_NO_SURFACE;
        int width, height;
        
        PBufferSurfaceBackend() : SurfaceBackend{SurfaceBackend::Type::PBUFFER} {}
        ~PBufferSurfaceBackend() override = default;
    };
    
    /**
     * Double-buffered HardwareBuffer surface.
     * 
     */
    struct HardwareBufferSurfaceBackend : SurfaceBackend {
        // GL state for HardwareBuffers
        struct HBGLState {
            EGLContext lastContext = EGL_NO_CONTEXT;
            GLint renderbuffer[2] = {-1, -1};
            GLint renderbufferDepth[2] = {-1, -1};
            GLint framebuffer[2] = {-1, -1};
        };
        
        SmartHardwareBuffer buffers[2];
        SmartEGLImage images[2];
        // Regenerate this on makeCurrent with a context
        HBGLState gl;
        //int width = 0, height = 0;
        int current = 0;
        
        /// A dummy pbuffer surface, so you can use makeCurrent with something
        EGLSurface dummy = EGL_NO_SURFACE;
        
        
        HardwareBufferSurfaceBackend() : SurfaceBackend{SurfaceBackend::Type::HWBUFFER} {}
        virtual ~HardwareBufferSurfaceBackend();
    };
    
    
    /// Base class for implementation Surfaces.
    struct Surface {
        enum class Type {
            WINDOW, PIXMAP, PBUFFER
        };
        Type type;
        /// The actual surface
        std::unique_ptr<SurfaceBackend> backend;
        
        
        virtual ~Surface() = default;
        
        static inline EGLSurface getSurface(Surface* s) {
            if (s == EGL_NO_SURFACE) return EGL_NO_SURFACE;
            if (s->backend->type == SurfaceBackend::Type::PBUFFER) {
                return static_cast<PBufferSurfaceBackend*>(s->backend.get())->pbuffer; // NOLINT(cppcoreguidelines-pro-type-static-cast-downcast)
            }
            if (s->backend->type == SurfaceBackend::Type::HWBUFFER) {
                return static_cast<HardwareBufferSurfaceBackend*>(s->backend.get())->dummy; // NOLINT(cppcoreguidelines-pro-type-static-cast-downcast)
            }
            return EGL_NO_SURFACE;
        }
    };
}


