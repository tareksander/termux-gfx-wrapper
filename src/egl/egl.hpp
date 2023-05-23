#pragma once

#define EGL_EGL_PROTOTYPES 0
#include <EGL/egl.h>
#include <EGL/eglext.h>

// my EGL.h doesn't have these definitions, so I copied them from a newer one
#define EGL_PLATFORM_XCB_EXT              0x31DC
#define EGL_PLATFORM_XCB_SCREEN_EXT       0x31DE

#define GL_GLES_PROTOTYPES 1
#include <GLES2/gl2.h>
#include <GLES2/gl2ext.h>
#include <dlfcn.h>

#include "android.hpp"

#include <glvnd/libeglabi.h>
#include <mutex>
#include <unordered_map>
#include <unordered_set>
#include <exception>
#include <memory>

#ifdef X11_PLATFORM
    #include <xcb/xcb.h>
    #include <xcb/xcbext.h>
    #include <xcb/xproto.h>
    #include <xcb/shm.h>
    #include <xcb/present.h>
    #include <xcb/sync.h>
    #include <X11/Xlib-xcb.h>
#endif

#ifdef WAYLAND_PLATFORM
    #include <wayland-client.h>
    #include "hwbuf-client.h"
#endif

// fallback value
#ifndef DEFAULT_DISPLAY_PLAYFORM
    #define DEFAULT_DISPLAY_PLAYFORM X11
#endif


#include "surface.hpp"


#define EGL_EXCEPTIONS(m)                           \
m(NotInitializedException, NOT_INITIALIZED)         \
m(BadAccessException, BAD_ACCESS)                   \
m(BadAllocException, BAD_ALLOC)                     \
m(BadAttributeException, BAD_ATTRIBUTE)             \
m(BadContextException, BAD_CONTEXT)                 \
m(BadConfigException, BAD_CONFIG)                   \
m(BadCurrentSurfaceException, BAD_CURRENT_SURFACE)  \
m(BadDisplayException, BAD_DISPLAY)                 \
m(BadSurfaceException, BAD_SURFACE)                 \
m(BadMatchException, BAD_MATCH)                     \
m(BadParameterException, BAD_PARAMETER)             \
m(BadNativePixmapException, BAD_NATIVE_PIXMAP)      \
m(BadNativeWindowException, BAD_NATIVE_WINDOW)      \
m(ContextLostException, CONTEXT_LOST)               \

#define GLES2_0_FUNCS(m) \
m(glActiveTexture)                          \
m(glAttachShader)                           \
m(glBindAttribLocation)                     \
m(glBindBuffer)                             \
m(glBindFramebuffer)                        \
m(glBindRenderbuffer)                       \
m(glBindTexture)                            \
m(glBlendColor)                             \
m(glBlendEquation)                          \
m(glBlendEquationSeparate)                  \
m(glBlendFunc)                              \
m(glBlendFuncSeparate)                      \
m(glBufferData)                             \
m(glBufferSubData)                          \
m(glCheckFramebufferStatus)                 \
m(glClear)                                  \
m(glClearColor)                             \
m(glClearDepthf)                            \
m(glClearStencil)                           \
m(glColorMask)                              \
m(glCompileShader)                          \
m(glCompressedTexImage2D)                   \
m(glCompressedTexSubImage2D)                \
m(glCopyTexImage2D)                         \
m(glCopyTexSubImage2D)                      \
m(glCreateProgram)                          \
m(glCreateShader)                           \
m(glCullFace)                               \
m(glDeleteBuffers)                          \
m(glDeleteFramebuffers)                     \
m(glDeleteProgram)                          \
m(glDeleteRenderbuffers)                    \
m(glDeleteShader)                           \
m(glDeleteTextures)                         \
m(glDepthFunc)                              \
m(glDepthMask)                              \
m(glDepthRangef)                            \
m(glDetachShader)                           \
m(glDisable)                                \
m(glDisableVertexAttribArray)               \
m(glDrawArrays)                             \
m(glDrawElements)                           \
m(glEnable)                                 \
m(glEnableVertexAttribArray)                \
m(glFinish)                                 \
m(glFlush)                                  \
m(glFramebufferRenderbuffer)                \
m(glFramebufferTexture2D)                   \
m(glFrontFace)                              \
m(glGenBuffers)                             \
m(glGenerateMipmap)                         \
m(glGenFramebuffers)                        \
m(glGenRenderbuffers)                       \
m(glGenTextures)                            \
m(glGetActiveAttrib)                        \
m(glGetActiveUniform)                       \
m(glGetAttachedShaders)                     \
m(glGetAttribLocation)                      \
m(glGetBooleanv)                            \
m(glGetBufferParameteriv)                   \
m(glGetError)                               \
m(glGetFloatv)                              \
m(glGetFramebufferAttachmentParameteriv)    \
m(glGetIntegerv)                            \
m(glGetProgramiv)                           \
m(glGetProgramInfoLog)                      \
m(glGetRenderbufferParameteriv)             \
m(glGetShaderiv)                            \
m(glGetShaderInfoLog)                       \
m(glGetShaderPrecisionFormat)               \
m(glGetShaderSource)                        \
m(glGetString)                              \
m(glGetTexParameterfv)                      \
m(glGetTexParameteriv)                      \
m(glGetUniformfv)                           \
m(glGetUniformiv)                           \
m(glGetUniformLocation)                     \
m(glGetVertexAttribfv)                      \
m(glGetVertexAttribiv)                      \
m(glGetVertexAttribPointerv)                \
m(glHint)                                   \
m(glIsBuffer)                               \
m(glIsEnabled)                              \
m(glIsFramebuffer)                          \
m(glIsProgram)                              \
m(glIsRenderbuffer)                         \
m(glIsShader)                               \
m(glIsTexture)                              \
m(glLineWidth)                              \
m(glLinkProgram)                            \
m(glPixelStorei)                            \
m(glPolygonOffset)                          \
m(glReadPixels)                             \
m(glReleaseShaderCompiler)                  \
m(glRenderbufferStorage)                    \
m(glSampleCoverage)                         \
m(glScissor)                                \
m(glShaderBinary)                           \
m(glShaderSource)                           \
m(glStencilFunc)                            \
m(glStencilFuncSeparate)                    \
m(glStencilMask)                            \
m(glStencilMaskSeparate)                    \
m(glStencilOp)                              \
m(glStencilOpSeparate)                      \
m(glTexImage2D)                             \
m(glTexParameterf)                          \
m(glTexParameterfv)                         \
m(glTexParameteri)                          \
m(glTexParameteriv)                         \
m(glTexSubImage2D)                          \
m(glUniform1f)                              \
m(glUniform1fv)                             \
m(glUniform1i)                              \
m(glUniform1iv)                             \
m(glUniform2f)                              \
m(glUniform2fv)                             \
m(glUniform2i)                              \
m(glUniform2iv)                             \
m(glUniform3f)                              \
m(glUniform3fv)                             \
m(glUniform3i)                              \
m(glUniform3iv)                             \
m(glUniform4f)                              \
m(glUniform4fv)                             \
m(glUniform4i)                              \
m(glUniform4iv)                             \
m(glUniformMatrix2fv)                       \
m(glUniformMatrix3fv)                       \
m(glUniformMatrix4fv)                       \
m(glUseProgram)                             \
m(glValidateProgram)                        \
m(glVertexAttrib1f)                         \
m(glVertexAttrib1fv)                        \
m(glVertexAttrib2f)                         \
m(glVertexAttrib2fv)                        \
m(glVertexAttrib3f)                         \
m(glVertexAttrib3fv)                        \
m(glVertexAttrib4f)                         \
m(glVertexAttrib4fv)                        \
m(glVertexAttribPointer)                    \
m(glViewport)

namespace egl_wrapper {
    /**
     * @brief Environment variable to override the default display from build time.
     * 
     * Valid values: NATIVE, WAYLAND, X11
     * 
     */
    inline const char* const TERMUX_EGL_TYPE_ENV = "TERMUX_EGL_DEFAULT";
    
    
    inline const char* const PLATFORMS_STRING = 
    "EGL_KHR_platform_android"
        #ifdef X11_PLATFORM
        " EGL_KHR_platform_x11 EGL_EXT_platform_x11 EGL_EXT_platform_xcb"
        #endif
        #ifdef WAYLAND_PLATFORM
        " EGL_KHR_platform_wayland EGL_EXT_platform_wayland"
        #endif
    ;
    
    /**
     * @brief Possible display platforms.
     * 
     */
    enum class DisplayType {
        NATIVE,
        WAYLAND,
        X11,
    };
    
    struct SmartEGLContext final {
        EGLContext c;
        
        operator EGLContext() {
            return c;
        }
        
        SmartEGLContext() : c{EGL_NO_CONTEXT} {}
        SmartEGLContext(EGLContext c) : c{c} {}
        ~SmartEGLContext();
        SmartEGLContext(const SmartEGLContext& o) = delete;
        SmartEGLContext& operator=(const SmartEGLContext& o) = delete;
        SmartEGLContext& operator=(SmartEGLContext&& o);
        SmartEGLContext(SmartEGLContext&& o) {
            *this = std::move(o);
        }
    };
    
    struct SmartEGLImage final {
        EGLImage i;
        
        operator EGLImage() {
            return i;
        }
        
        SmartEGLImage() : i{EGL_NO_IMAGE} {}
        SmartEGLImage(EGLImage i) : i{i} {}
        ~SmartEGLImage();
        SmartEGLImage(const SmartEGLImage& o) = delete;
        SmartEGLImage& operator=(const SmartEGLImage& o) = delete;
        SmartEGLImage& operator=(SmartEGLImage&& o);
        SmartEGLImage(SmartEGLImage&& o) {
            *this = std::move(o);
        }
    };
    
    struct Context {
        /// The context API: EGL_OPENGL_ES_API or EGL_OPENGL_API
        EGLint api;
        /// The Android EGLContext
        SmartEGLContext nativeContext;
        
        Context(EGLint api, EGLContext nativeContext) : api{api}, nativeContext{nativeContext} {}
        
        
        static inline EGLContext getContext(Context* c) {
            if (c == EGL_NO_CONTEXT) return EGL_NO_CONTEXT;
            return c->nativeContext;
        }
    };
    
    /**
     * @brief Generic EGLDisplay backend that handles all functions dispatched by libglvnd.
     * 
     */
    struct EGLDisplayBackend {
        /**
         * Lock for reading and writing.
         * Has to be set before calling any method (even the destructor through delete).
         * When throwing in the constructor, it's not necessary (or possible) to lock this, but the constructor
         * always only get called on one thread, so it's fine.
         */
        std::mutex lock;
        
        // The type of the display.
        const DisplayType type;
        
        
        
        
        EGLDisplayBackend(const EGLDisplayBackend&) = delete;
        EGLDisplayBackend(EGLDisplayBackend&&) = delete;
        EGLDisplayBackend& operator=(const EGLDisplayBackend&) = delete;
        EGLDisplayBackend& operator=(EGLDisplayBackend&&) = delete;
        
        
        EGLDisplayBackend(DisplayType type) : type{type} {};
        virtual ~EGLDisplayBackend() {};
        
        virtual EGLBoolean eglChooseConfig(const EGLint* attrib_list, EGLConfig* configs, EGLint config_size, EGLint* num_config) = 0;
        virtual EGLBoolean eglCopyBuffers(EGLSurface surface, EGLNativePixmapType target) = 0;
        virtual EGLContext eglCreateContext(EGLConfig config, EGLContext share_context, const EGLint* attrib_list) = 0;
        virtual EGLSurface eglCreatePbufferSurface(EGLConfig config, const EGLint* attrib_list) = 0;
        virtual EGLSurface eglCreatePixmapSurface(EGLConfig config, EGLNativePixmapType pixmap, const EGLint* attrib_list) = 0;
        virtual EGLSurface eglCreateWindowSurface(EGLConfig config, EGLNativeWindowType win, const EGLint* attrib_list) = 0;
        virtual EGLBoolean eglDestroyContext(EGLContext ctx) = 0;
        virtual EGLBoolean eglDestroySurface(EGLSurface surface) = 0;
        virtual EGLBoolean eglGetConfigAttrib(EGLConfig config, EGLint attribute, EGLint* value) = 0;
        virtual EGLBoolean eglGetConfigs(EGLConfig* configs, EGLint config_size, EGLint* num_config) = 0;
        virtual EGLBoolean eglInitialize(EGLint* major, EGLint* minor) = 0;
        virtual EGLBoolean eglMakeCurrent(EGLSurface draw, EGLSurface read, EGLContext ctx) = 0;
        virtual EGLBoolean eglQueryContext(EGLContext ctx, EGLint attribute, EGLint* value) = 0;
        virtual const char* eglQueryString(EGLint name) = 0;
        virtual EGLBoolean eglQuerySurface(EGLSurface surface, EGLint attribute, EGLint* value) = 0;
        virtual EGLBoolean eglSwapBuffers(EGLSurface surface) = 0;
        virtual EGLBoolean eglTerminate() = 0;
        
        // EGL 1.1
        virtual EGLBoolean eglBindTexImage(EGLSurface surface, EGLint buffer) = 0;
        virtual EGLBoolean eglReleaseTexImage(EGLSurface surface, EGLint buffer) = 0;
        virtual EGLBoolean eglSurfaceAttrib(EGLSurface surface, EGLint attribute, EGLint value) = 0;
        virtual EGLBoolean eglSwapInterval(EGLint interval) = 0;
        
        // EGL 1.2
        virtual EGLSurface eglCreatePbufferFromClientBuffer(EGLenum buftype, EGLClientBuffer buffer, EGLConfig config, const EGLint* attrib_list) = 0;
        virtual EGLBoolean eglReleaseThread(void) = 0;
        
        // EGL 1.4
        virtual EGLContext eglGetCurrentContext(void) = 0;
        
        // EGL 1.5
        virtual EGLSync eglCreateSync(EGLenum type, const EGLAttrib* attrib_list) = 0;
        virtual EGLBoolean eglDestroySync(EGLSync sync) = 0;
        virtual EGLint eglClientWaitSync(EGLSync sync, EGLint flags, EGLTime timeout) = 0;
        virtual EGLBoolean eglGetSyncAttrib(EGLSync sync, EGLint attribute, EGLAttrib* value) = 0;
        virtual EGLImage eglCreateImage(EGLContext ctx, EGLenum target, EGLClientBuffer buffer, const EGLAttrib* attrib_list) = 0;
        virtual EGLBoolean eglDestroyImage(EGLImage image) = 0;
        virtual EGLSurface eglCreatePlatformWindowSurface(EGLConfig config, void* native_window, const EGLAttrib* attrib_list) = 0;
        virtual EGLSurface eglCreatePlatformPixmapSurface(EGLConfig config, void* native_pixmap, const EGLAttrib* attrib_list) = 0;
        virtual EGLBoolean eglWaitSync(EGLSync sync, EGLint flags) = 0;
        
        
        
        
    };
    
    /**
     * @brief Passthrough to the Android EGL library, if a program needs direct access to the Android EGL platform (e.g. a Termux:GUI progam that uses HardwareBuffers itself).
     * 
     */
    struct AndroidDisplay : EGLDisplayBackend {
        
        /// @brief Whether EGL 1.5 is available
        bool egl15 = true;
        
        AndroidDisplay() : EGLDisplayBackend(DisplayType::NATIVE) {}
        
        
        // EGL 1.0
        EGLBoolean eglChooseConfig(const EGLint* attrib_list, EGLConfig* configs, EGLint config_size, EGLint* num_config);
        EGLBoolean eglCopyBuffers(EGLSurface surface, EGLNativePixmapType target);
        EGLContext eglCreateContext(EGLConfig config, EGLContext share_context, const EGLint* attrib_list);
        EGLSurface eglCreatePbufferSurface(EGLConfig config, const EGLint* attrib_list);
        EGLSurface eglCreatePixmapSurface(EGLConfig config, EGLNativePixmapType pixmap, const EGLint* attrib_list);
        EGLSurface eglCreateWindowSurface(EGLConfig config, EGLNativeWindowType win, const EGLint* attrib_list);
        EGLBoolean eglDestroyContext(EGLContext ctx);
        EGLBoolean eglDestroySurface(EGLSurface surface);
        EGLBoolean eglGetConfigAttrib(EGLConfig config, EGLint attribute, EGLint* value);
        EGLBoolean eglGetConfigs(EGLConfig* configs, EGLint config_size, EGLint* num_config);
        EGLBoolean eglInitialize(EGLint* major, EGLint* minor);
        EGLBoolean eglMakeCurrent(EGLSurface draw, EGLSurface read, EGLContext ctx);
        EGLBoolean eglQueryContext(EGLContext ctx, EGLint attribute, EGLint* value);
        const char* eglQueryString(EGLint name);
        EGLBoolean eglQuerySurface(EGLSurface surface, EGLint attribute, EGLint* value);
        EGLBoolean eglSwapBuffers(EGLSurface surface);
        EGLBoolean eglTerminate();
        
        // EGL 1.1
        EGLBoolean eglBindTexImage(EGLSurface surface, EGLint buffer);
        EGLBoolean eglReleaseTexImage(EGLSurface surface, EGLint buffer);
        EGLBoolean eglSurfaceAttrib(EGLSurface surface, EGLint attribute, EGLint value);
        EGLBoolean eglSwapInterval(EGLint interval);
        
        // EGL 1.2
        EGLSurface eglCreatePbufferFromClientBuffer(EGLenum buftype, EGLClientBuffer buffer, EGLConfig config, const EGLint* attrib_list);
        EGLBoolean eglReleaseThread(void);
        
        // EGL 1.4
        EGLContext eglGetCurrentContext(void);
        
        // EGL 1.5
        EGLSync eglCreateSync(EGLenum type, const EGLAttrib* attrib_list);
        EGLBoolean eglDestroySync(EGLSync sync);
        EGLint eglClientWaitSync(EGLSync sync, EGLint flags, EGLTime timeout);
        EGLBoolean eglGetSyncAttrib(EGLSync sync, EGLint attribute, EGLAttrib* value);
        EGLImage eglCreateImage(EGLContext ctx, EGLenum target, EGLClientBuffer buffer, const EGLAttrib* attrib_list);
        EGLBoolean eglDestroyImage(EGLImage image);
        EGLSurface eglCreatePlatformWindowSurface(EGLConfig config, void* native_window, const EGLAttrib* attrib_list);
        EGLSurface eglCreatePlatformPixmapSurface(EGLConfig config, void* native_pixmap, const EGLAttrib* attrib_list);
        EGLBoolean eglWaitSync(EGLSync sync, EGLint flags);
        
        // TODO EGL_image, EGL_image_base, ANDROID_image_native_buffer, ANDROID_get_native_client_buffer
        
        
    };
    
    
    #ifdef X11_PLATFORM
        /**
         * @brief Backend for rendering to an X11 server.
         * 
         */
        struct X11Display : EGLDisplayBackend {
            
            xcb_connection_t* xcbC{};
            bool ownsConnection{};
            
            bool init{};
            int screenIndex{};
            xcb_screen_t* screen{};
            xcb_visualtype_t vis{};
            
            
            struct WindowSurface : Surface {
                xcb_connection_t* xcbC = nullptr;
                EGLConfig conf;
                /// The window
                xcb_window_t w = -1;
                // width and height of the window.
                int wWidth = 1;
                int wHeight = 1;
                /// special event id
                uint32_t eid = -1;
                xcb_special_event_t* ev = nullptr;
                
                /// The pixmap that get presented to the window on eglSwapBuffers
                xcb_pixmap_t p = -1;
                void* pData = nullptr;
                int pFD = -1;
                int pWidth = 0;
                int pHeight = 0;
                int seg = -1;
                
                bool notifyNeeded = false;
                
               virtual ~WindowSurface();
                
            };
            
            
            /// The active EGLContexts for this display
            std::unordered_set<Context*> contexts;
            
            /// The active EGLSurfaces for this display
            std::unordered_set<Surface*> surfaces;
            
            
            
            
            X11Display(Display* x11Display, const EGLAttrib *attrib_list);
            X11Display(xcb_connection_t* cbC, const EGLAttrib *attrib_list);
            virtual ~X11Display();
            
            // EGL 1.0
            EGLBoolean eglChooseConfig(const EGLint* attrib_list, EGLConfig* configs, EGLint config_size, EGLint* num_config);
            EGLBoolean eglCopyBuffers(EGLSurface surface, EGLNativePixmapType target);
            EGLContext eglCreateContext(EGLConfig config, EGLContext share_context, const EGLint* attrib_list);
            EGLSurface eglCreatePbufferSurface(EGLConfig config, const EGLint* attrib_list);
            EGLSurface eglCreatePixmapSurface(EGLConfig config, EGLNativePixmapType pixmap, const EGLint* attrib_list);
            EGLSurface eglCreateWindowSurface(EGLConfig config, EGLNativeWindowType win, const EGLint* attrib_list);
            EGLBoolean eglDestroyContext(EGLContext ctx);
            EGLBoolean eglDestroySurface(EGLSurface surface);
            EGLBoolean eglGetConfigAttrib(EGLConfig config, EGLint attribute, EGLint* value);
            EGLBoolean eglGetConfigs(EGLConfig* configs, EGLint config_size, EGLint* num_config);
            EGLBoolean eglInitialize(EGLint* major, EGLint* minor);
            EGLBoolean eglMakeCurrent(EGLSurface draw, EGLSurface read, EGLContext ctx);
            EGLBoolean eglQueryContext(EGLContext ctx, EGLint attribute, EGLint* value);
            const char* eglQueryString(EGLint name);
            EGLBoolean eglQuerySurface(EGLSurface surface, EGLint attribute, EGLint* value);
            EGLBoolean eglSwapBuffers(EGLSurface surface);
            EGLBoolean eglTerminate();
            
            // EGL 1.1
            EGLBoolean eglBindTexImage(EGLSurface surface, EGLint buffer);
            EGLBoolean eglReleaseTexImage(EGLSurface surface, EGLint buffer);
            EGLBoolean eglSurfaceAttrib(EGLSurface surface, EGLint attribute, EGLint value);
            EGLBoolean eglSwapInterval(EGLint interval);
            
            // EGL 1.2
            EGLSurface eglCreatePbufferFromClientBuffer(EGLenum buftype, EGLClientBuffer buffer, EGLConfig config, const EGLint* attrib_list);
            EGLBoolean eglReleaseThread(void);
            
            // EGL 1.4
            EGLContext eglGetCurrentContext(void);
            
            // EGL 1.5
            EGLSync eglCreateSync(EGLenum type, const EGLAttrib* attrib_list);
            EGLBoolean eglDestroySync(EGLSync sync);
            EGLint eglClientWaitSync(EGLSync sync, EGLint flags, EGLTime timeout);
            EGLBoolean eglGetSyncAttrib(EGLSync sync, EGLint attribute, EGLAttrib* value);
            EGLImage eglCreateImage(EGLContext ctx, EGLenum target, EGLClientBuffer buffer, const EGLAttrib* attrib_list);
            EGLBoolean eglDestroyImage(EGLImage image);
            EGLSurface eglCreatePlatformWindowSurface(EGLConfig config, void* native_window, const EGLAttrib* attrib_list);
            EGLSurface eglCreatePlatformPixmapSurface(EGLConfig config, void* native_pixmap, const EGLAttrib* attrib_list);
            EGLBoolean eglWaitSync(EGLSync sync, EGLint flags);
            
            
            
            private:
            void checkExtensions() const;
            
        };
    #endif
    
    /**
     * @brief EGL errors are returned via exceptions, which get turned into error codes at the boundary to client code.
     * 
     */
    namespace Exceptions {
        struct NoDisplayException         : std::exception {};
        struct NotInitializedException    : std::exception {};
        struct BadAccessException         : std::exception {};
        using BadAllocException = std::bad_alloc;
        struct BadAttributeException      : std::exception {};
        struct BadContextException        : std::exception {};
        struct BadConfigException         : std::exception {};
        struct BadCurrentSurfaceException : std::exception {};
        struct BadDisplayException        : std::exception {};
        struct BadSurfaceException        : std::exception {};
        struct BadMatchException          : std::exception {};
        struct BadParameterException      : std::exception {};
        struct BadNativePixmapException   : std::exception {};
        struct BadNativeWindowException   : std::exception {};
        struct ContextLostException       : std::exception {};
    }
    
    /// There is only one Android EGLDisplay, so no list is needed.
    extern AndroidDisplay androidDisplay;
    
    
    // lock for xlibDisplayList, xcbDisplayList and waylandDisplayList
    extern std::mutex displayListLock;
    #ifdef X11_PLATFORM
        extern std::unordered_map<void*, X11Display*> xlibDisplayList;
        extern std::unordered_map<void*, X11Display*> xcbDisplayList;
    #endif
    #ifdef WAYLAND_PLATFORM
        extern std::unordered_map<void*, WaylandDisplay*> waylandDisplayList;
    #endif

    // semi-constants, set on initialisation.
    /**
     * @brief Pointer to the dlopen'd libEGL.so
     * 
     */
    extern void* nativeEGLLibrary;
    /**
     * @brief Pointer to the dlopen'd libGLESv2.so
     * 
     */
    extern void* nativeGLES2Library;
    /**
     * @brief libandroid function collection.
     * 
     */
    extern libAndroidHelper::LibAndroid libandroid;
    /**
     * @brief libglvnd exports.
     * 
     */
    extern const __EGLapiExports* glvnd;
    /**
     * @brief libglvnd vendor pointer for this library.
     * 
     */
    extern __EGLvendorInfo *thisVendor;
    /**
     * @brief Whether HardwareBuffer rendering is available.
     * 
     */
    extern bool hwbufferRenderingAvailable;
    /**
     * @brief The default display platform, for eglGetDisplay.
     * 
     */
    extern DisplayType defaultDisplayType;
    /**
     * @brief The Android EGLDisplay.
     * 
     */
    extern EGLDisplay nativeDisplay;
    
    
    // Turns out this lock isn't needed, libglvnd does locking itself when checking its index table and calling getDispatchAddress or setDispatchIndex
    /// @brief Lock for dispatchIndexMap.
    //extern std::mutex dispatchLock;
    
    //extern std::unordered_map<std::string, int> dispatchIndexMap;
    
    extern int eglCreateImageKHRIndex;
    extern int eglDestroyImageKHRIndex;

    /// @brief The last EGL error of the library for a thread
    extern thread_local EGLint lastError;

    EGLDisplay getPlatformDisplay(EGLenum platform, void* nativeDisplay, const EGLAttrib* attrib_list);
    EGLBoolean getSupportsAPI(EGLenum api);
    void* getProcAddress(const char* procName);
    void* getDispatchAddress(const char* procName);
    void setDispatchIndex(const char* procName, int index);
    const char* getVendorString(int name);
    
    /**
     * @brief Functions that dispatch to the EGLDisplayBackend.
     * 
     */
    namespace dispatch {
        // EGL 1.0
        EGLBoolean eglChooseConfig(EGLDisplay dpy, const EGLint* attrib_list, EGLConfig* configs, EGLint config_size, EGLint* num_config);
        EGLBoolean eglCopyBuffers(EGLDisplay dpy, EGLSurface surface, EGLNativePixmapType target);
        EGLContext eglCreateContext(EGLDisplay dpy, EGLConfig config, EGLContext share_context, const EGLint* attrib_list);
        EGLSurface eglCreatePbufferSurface(EGLDisplay dpy, EGLConfig config, const EGLint* attrib_list);
        EGLSurface eglCreatePixmapSurface(EGLDisplay dpy, EGLConfig config, EGLNativePixmapType pixmap, const EGLint* attrib_list);
        EGLSurface eglCreateWindowSurface(EGLDisplay dpy, EGLConfig config, EGLNativeWindowType win, const EGLint* attrib_list);
        EGLBoolean eglDestroyContext(EGLDisplay dpy, EGLContext ctx);
        EGLBoolean eglDestroySurface(EGLDisplay dpy, EGLSurface surface);
        EGLBoolean eglGetConfigAttrib(EGLDisplay dpy, EGLConfig config, EGLint attribute, EGLint* value);
        EGLBoolean eglGetConfigs(EGLDisplay dpy, EGLConfig* configs, EGLint config_size, EGLint* num_config);
        // no dispatch needed
        //EGLDisplay eglGetCurrentDisplay(void);
        // no dispatch needed
        //EGLSurface eglGetCurrentSurface(EGLint readdraw);
        // no dispatch needed
        //EGLDisplay eglGetDisplay(EGLNativeDisplayType display_id);
        EGLint eglGetError(void);
        // no dispatch needed
        //__eglMustCastToProperFunctionPointerType eglGetProcAddress(const char* procname);
        EGLBoolean eglInitialize(EGLDisplay dpy, EGLint* major, EGLint* minor);
        EGLBoolean eglMakeCurrent(EGLDisplay dpy, EGLSurface draw, EGLSurface read, EGLContext ctx);
        EGLBoolean eglQueryContext(EGLDisplay dpy, EGLContext ctx, EGLint attribute, EGLint* value);
        const char* eglQueryString(EGLDisplay dpy, EGLint name);
        EGLBoolean eglQuerySurface(EGLDisplay dpy, EGLSurface surface, EGLint attribute, EGLint* value);
        EGLBoolean eglSwapBuffers(EGLDisplay dpy, EGLSurface surface);
        EGLBoolean eglTerminate(EGLDisplay dpy);
        EGLBoolean eglWaitGL(void);
        EGLBoolean eglWaitNative(EGLint engine);
        
        // EGL 1.1
        EGLBoolean eglBindTexImage(EGLDisplay dpy, EGLSurface surface, EGLint buffer);
        EGLBoolean eglReleaseTexImage(EGLDisplay dpy, EGLSurface surface, EGLint buffer);
        EGLBoolean eglSurfaceAttrib(EGLDisplay dpy, EGLSurface surface, EGLint attribute, EGLint value);
        EGLBoolean eglSwapInterval(EGLDisplay dpy, EGLint interval);
        
        // EGL 1.2
        EGLBoolean eglBindAPI(EGLenum api);
        // no dispatch needed
        //EGLenum eglQueryAPI(void);
        EGLSurface eglCreatePbufferFromClientBuffer(EGLDisplay dpy, EGLenum buftype, EGLClientBuffer buffer, EGLConfig config, const EGLint* attrib_list);
        EGLBoolean eglReleaseThread(void);
        EGLBoolean eglWaitClient(void);
        
        // EGL 1.4
        // no dispatch needed
        //EGLContext eglGetCurrentContext(void);
        
        // EGL 1.5
        EGLSync eglCreateSync(EGLDisplay dpy, EGLenum type, const EGLAttrib* attrib_list);
        EGLBoolean eglDestroySync(EGLDisplay dpy, EGLSync sync);
        EGLint eglClientWaitSync(EGLDisplay dpy, EGLSync sync, EGLint flags, EGLTime timeout);
        EGLBoolean eglGetSyncAttrib(EGLDisplay dpy, EGLSync sync, EGLint attribute, EGLAttrib* value);
        EGLImage eglCreateImage(EGLDisplay dpy, EGLContext ctx, EGLenum target, EGLClientBuffer buffer, const EGLAttrib* attrib_list);
        EGLBoolean eglDestroyImage(EGLDisplay dpy, EGLImage image);
        // no dispatch needed
        //EGLDisplay eglGetPlatformDisplay(EGLenum platform, void* native_display, const EGLAttrib* attrib_list);
        EGLSurface eglCreatePlatformWindowSurface(EGLDisplay dpy, EGLConfig config, void* native_window, const EGLAttrib* attrib_list);
        EGLSurface eglCreatePlatformPixmapSurface(EGLDisplay dpy, EGLConfig config, void* native_pixmap, const EGLAttrib* attrib_list);
        EGLBoolean eglWaitSync(EGLDisplay dpy, EGLSync sync, EGLint flags);
        
        
        // extensions
        
        // EGL_KHR_image_base
        EGLImageKHR eglCreateImageKHR(EGLDisplay dpy, EGLContext ctx, EGLenum target, EGLClientBuffer buffer, const EGLint* attrib_list);
        EGLBoolean eglDestroyImageKHR(EGLDisplay dpy, EGLImageKHR image);
        
        // EGL_ANDROID_create_native_client_buffer
        EGLClientBuffer eglCreateNativeClientBufferANDROID(const EGLint* attrib_list);
        
        
        // EGL_ANDROID_get_native_client_buffer
        EGLClientBuffer eglGetNativeClientBufferANDROID(const struct AHardwareBuffer* buffer);
        
        
        
        // GLES2.0

#define FUNC(name) __typeof__(name) name;
        GLES2_0_FUNCS(FUNC)
#undef FUNC
        
        // TODO GL_OES_EGL_IMAGE, GL_OES_EGL_IMAGE_EXTERNAL
    }
    
    
    
    // EGL 1.0 functions
    extern PFNEGLCHOOSECONFIGPROC real_eglChooseConfig;
    extern PFNEGLCOPYBUFFERSPROC real_eglCopyBuffers;
    extern PFNEGLCREATECONTEXTPROC real_eglCreateContext;
    extern PFNEGLCREATEPBUFFERSURFACEPROC real_eglCreatePbufferSurface;
    extern PFNEGLCREATEPIXMAPSURFACEPROC real_eglCreatePixmapSurface;
    extern PFNEGLCREATEWINDOWSURFACEPROC real_eglCreateWindowSurface;
    extern PFNEGLDESTROYCONTEXTPROC real_eglDestroyContext;
    extern PFNEGLDESTROYSURFACEPROC real_eglDestroySurface;
    extern PFNEGLGETCONFIGATTRIBPROC real_eglGetConfigAttrib;
    extern PFNEGLGETCONFIGSPROC real_eglGetConfigs;
    extern PFNEGLGETCURRENTDISPLAYPROC real_eglGetCurrentDisplay;
    extern PFNEGLGETCURRENTSURFACEPROC real_eglGetCurrentSurface;
    extern PFNEGLGETDISPLAYPROC real_eglGetDisplay;
    extern PFNEGLGETERRORPROC real_eglGetError;
    extern PFNEGLGETPROCADDRESSPROC real_eglGetProcAddress;
    extern PFNEGLINITIALIZEPROC real_eglInitialize;
    extern PFNEGLMAKECURRENTPROC real_eglMakeCurrent;
    extern PFNEGLQUERYCONTEXTPROC real_eglQueryContext;
    extern PFNEGLQUERYSTRINGPROC real_eglQueryString;
    extern PFNEGLQUERYSURFACEPROC real_eglQuerySurface;
    extern PFNEGLSWAPBUFFERSPROC real_eglSwapBuffers;
    extern PFNEGLTERMINATEPROC real_eglTerminate;
    extern PFNEGLWAITGLPROC real_eglWaitGL;
    extern PFNEGLWAITNATIVEPROC real_eglWaitNative;
    
    // EGL 1.1 functions
    
    extern PFNEGLBINDTEXIMAGEPROC real_eglBindTexImage;
    extern PFNEGLRELEASETEXIMAGEPROC real_eglReleaseTexImage;
    extern PFNEGLSURFACEATTRIBPROC real_eglSurfaceAttrib;
    extern PFNEGLSWAPINTERVALPROC real_eglSwapInterval;
    
    
    // EGL 1.2 functions
    
    extern PFNEGLBINDAPIPROC real_eglBindAPI;
    extern PFNEGLQUERYAPIPROC real_eglQueryAPI;
    extern PFNEGLCREATEPBUFFERFROMCLIENTBUFFERPROC real_eglCreatePbufferFromClientBuffer;
    extern PFNEGLRELEASETHREADPROC real_eglReleaseThread;
    extern PFNEGLWAITCLIENTPROC real_eglWaitClient;
    
    // EGL 1.4 functions
    
    extern PFNEGLGETCURRENTCONTEXTPROC real_eglGetCurrentContext;

    // EGL 1.5 functions
    
    extern PFNEGLCREATESYNCPROC real_eglCreateSync;
    extern PFNEGLDESTROYSYNCPROC real_eglDestroySync;
    extern PFNEGLCLIENTWAITSYNCPROC real_eglClientWaitSync;
    extern PFNEGLGETSYNCATTRIBPROC real_eglGetSyncAttrib;
    extern PFNEGLCREATEIMAGEPROC real_eglCreateImage;
    extern PFNEGLDESTROYIMAGEPROC real_eglDestroyImage;
    extern PFNEGLGETPLATFORMDISPLAYPROC real_eglGetPlatformDisplay;
    extern PFNEGLCREATEPLATFORMWINDOWSURFACEPROC real_eglCreatePlatformWindowSurface;
    extern PFNEGLCREATEPLATFORMPIXMAPSURFACEPROC real_eglCreatePlatformPixmapSurface;
    extern PFNEGLWAITSYNCPROC real_eglWaitSync;
    
    // EGL_KHR_image_base
    extern EGLImageKHR (*real_eglCreateImageKHR)(EGLDisplay dpy, EGLContext ctx, EGLenum target, EGLClientBuffer buffer, const EGLint* attrib_list);
    extern EGLBoolean (*real_eglDestroyImageKHR)(EGLDisplay dpy, EGLImageKHR image);
    
    // EGL_ANDROID_create_native_client_buffer
    extern EGLClientBuffer (*real_eglCreateNativeClientBufferANDROID)(const EGLint* attrib_list);
    
    
    // EGL_ANDROID_get_native_client_buffer
    extern EGLClientBuffer (*real_eglGetNativeClientBufferANDROID)(const struct AHardwareBuffer* buffer);
    
    
    // GLES2.0 functions

#define FUNC(name) extern __typeof__(name)* real_ ## name;
    GLES2_0_FUNCS(FUNC)
#undef FUNC

}

/**
 * @brief Converts exceptions to EGL error codes.
 * 
 */

#define EGL_CATCH_EXCEPTION(name, type) catch(const egl_wrapper::Exceptions::name&) { lastError = (EGL_ ## type); }
#define CATCH_EGL_EXCEPTIONS EGL_EXCEPTIONS(EGL_CATCH_EXCEPTION)
