#pragma once

#define EGL_EGL_PROTOTYPES 0
#include <EGL/egl.h>

namespace egl_wrapper {
    
    struct Surface {
        enum class Type {
            PBUFFER, HWBUFFER
        };
        
        Type type;
    };
    
    
    struct PBufferSurface : Surface {
        EGLSurface pbuffer;
        int width, height;
    };
    
    
}


