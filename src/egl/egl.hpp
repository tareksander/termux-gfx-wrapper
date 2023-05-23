#pragma once

#define EGL_EGL_PROTOTYPES 0
#include <EGL/egl.h>
#include <EGL/eglext.h>

// my EGL.h doesn't have these definitions, so I copied them from a newer one
#define EGL_PLATFORM_XCB_EXT              0x31DC
#define EGL_PLATFORM_XCB_SCREEN_EXT       0x31DE


#define GL_GLES_PROTOTYPES 0
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
            
            xcb_connection_t* xcbC;
            bool ownsConnection;
            
            bool init = false;
            int screenIndex;
            xcb_screen_t* screen;
            xcb_visualtype_t vis;
            
            
            struct WindowSurface : Surface {
                xcb_connection_t* xcbC = NULL;
                EGLConfig conf;
                /// The window
                xcb_window_t w = -1;
                // width and height of the window.
                int wWidth = 1;
                int wHeight = 1;
                /// special event id
                int eid = -1;
                xcb_special_event_t* ev = NULL;
                
                /// The pixmap that get presented to the window on eglSwapBuffers
                xcb_pixmap_t p = -1;
                void* pData = NULL;
                int pFD = -1;
                int pWidth = 0;
                int pHeight = 0;
                
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
            void checkExtensions();
            
        };
    #endif
    
    /**
     * @brief EGL errors are returned via exceptions, which get turned into error codes at the boundary to client code.
     * 
     */
    namespace Exceptions {
        struct NoDisplayException : std::exception {};
        
        struct NotInitializedException : std::exception {};
        struct BadAccessException : std::exception {};
        using BadAllocException = std::bad_alloc;
        struct BadAttributeException : std::exception {};
        struct BadContextException : std::exception {};
        struct BadConfigException : std::exception {};
        struct BadCurrentSurfaceException : std::exception {};
        struct BadDisplayException : std::exception {};
        struct BadSurfaceException : std::exception {};
        struct BadMatchException : std::exception {};
        struct BadParameterException : std::exception {};
        struct BadNativePixmapException : std::exception {};
        struct BadNativeWindowException : std::exception {};
        struct ContextLostException : std::exception {};
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
        
        void glActiveTexture(GLenum texture);
        void glAttachShader(GLuint program, GLuint shader);
        void glBindAttribLocation(GLuint program, GLuint index, const GLchar* name);
        void glBindBuffer(GLenum target, GLuint buffer);
        void glBindFramebuffer(GLenum target, GLuint framebuffer);
        void glBindRenderbuffer(GLenum target, GLuint renderbuffer);
        void glBindTexture(GLenum target, GLuint texture);
        void glBlendColor(GLfloat red, GLfloat green, GLfloat blue, GLfloat alpha);
        void glBlendEquation(GLenum mode);
        void glBlendEquationSeparate(GLenum modeRGB, GLenum modeAlpha);
        void glBlendFunc(GLenum sfactor, GLenum dfactor);
        void glBlendFuncSeparate(GLenum sfactorRGB, GLenum dfactorRGB, GLenum sfactorAlpha, GLenum dfactorAlpha);
        void glBufferData(GLenum target, GLsizeiptr size, const void* data, GLenum usage);
        void glBufferSubData(GLenum target, GLintptr offset, GLsizeiptr size, const void* data);
        GLenum glCheckFramebufferStatus(GLenum target);
        void glClear(GLbitfield mask);
        void glClearColor(GLfloat red, GLfloat green, GLfloat blue, GLfloat alpha);
        void glClearDepthf(GLfloat d);
        void glClearStencil(GLint s);
        void glColorMask(GLboolean red, GLboolean green, GLboolean blue, GLboolean alpha);
        void glCompileShader(GLuint shader);
        void glCompressedTexImage2D(GLenum target, GLint level, GLenum internalformat, GLsizei width, GLsizei height, GLint border, GLsizei imageSize, const void* data);
        void glCompressedTexSubImage2D(GLenum target, GLint level, GLint xoffset, GLint yoffset, GLsizei width, GLsizei height, GLenum format, GLsizei imageSize, const void* data);
        void glCopyTexImage2D(GLenum target, GLint level, GLenum internalformat, GLint x, GLint y, GLsizei width, GLsizei height, GLint border);
        void glCopyTexSubImage2D(GLenum target, GLint level, GLint xoffset, GLint yoffset, GLint x, GLint y, GLsizei width, GLsizei height);
        GLuint glCreateProgram(void);
        GLuint glCreateShader(GLenum type);
        void glCullFace(GLenum mode);
        void glDeleteBuffers(GLsizei n, const GLuint* buffers);
        void glDeleteFramebuffers(GLsizei n, const GLuint* framebuffers);
        void glDeleteProgram(GLuint program);
        void glDeleteRenderbuffers(GLsizei n, const GLuint* renderbuffers);
        void glDeleteShader(GLuint shader);
        void glDeleteTextures(GLsizei n, const GLuint* textures);
        void glDepthFunc(GLenum func);
        void glDepthMask(GLboolean flag);
        void glDepthRangef(GLfloat n, GLfloat f);
        void glDetachShader(GLuint program, GLuint shader);
        void glDisable(GLenum cap);
        void glDisableVertexAttribArray(GLuint index);
        void glDrawArrays(GLenum mode, GLint first, GLsizei count);
        void glDrawElements(GLenum mode, GLsizei count, GLenum type, const void* indices);
        void glEnable(GLenum cap);
        void glEnableVertexAttribArray(GLuint index);
        void glFinish(void);
        void glFlush(void);
        void glFramebufferRenderbuffer(GLenum target, GLenum attachment, GLenum renderbuffertarget, GLuint renderbuffer);
        void glFramebufferTexture2D(GLenum target, GLenum attachment, GLenum textarget, GLuint texture, GLint level);
        void glFrontFace(GLenum mode);
        void glGenBuffers(GLsizei n, GLuint* buffers);
        void glGenerateMipmap(GLenum target);
        void glGenFramebuffers(GLsizei n, GLuint* framebuffers);
        void glGenRenderbuffers(GLsizei n, GLuint* renderbuffers);
        void glGenTextures(GLsizei n, GLuint* textures);
        void glGetActiveAttrib(GLuint program, GLuint index, GLsizei bufSize, GLsizei* length, GLint* size, GLenum* type, GLchar* name);
        void glGetActiveUniform(GLuint program, GLuint index, GLsizei bufSize, GLsizei* length, GLint* size, GLenum* type, GLchar* name);
        void glGetAttachedShaders(GLuint program, GLsizei maxCount, GLsizei* count, GLuint* shaders);
        GLint glGetAttribLocation(GLuint program, const GLchar* name);
        void glGetBooleanv(GLenum pname, GLboolean* data);
        void glGetBufferParameteriv(GLenum target, GLenum pname, GLint* params);
        GLenum glGetError(void);
        void glGetFloatv(GLenum pname, GLfloat* data);
        void glGetFramebufferAttachmentParameteriv(GLenum target, GLenum attachment, GLenum pname, GLint* params);
        void glGetIntegerv(GLenum pname, GLint* data);
        void glGetProgramiv(GLuint program, GLenum pname, GLint* params);
        void glGetProgramInfoLog(GLuint program, GLsizei bufSize, GLsizei* length, GLchar* infoLog);
        void glGetRenderbufferParameteriv(GLenum target, GLenum pname, GLint* params);
        void glGetShaderiv(GLuint shader, GLenum pname, GLint* params);
        void glGetShaderInfoLog(GLuint shader, GLsizei bufSize, GLsizei* length, GLchar* infoLog);
        void glGetShaderPrecisionFormat(GLenum shadertype, GLenum precisiontype, GLint* range, GLint* precision);
        void glGetShaderSource(GLuint shader, GLsizei bufSize, GLsizei* length, GLchar* source);
        const GLubyte* glGetString(GLenum name);
        void glGetTexParameterfv(GLenum target, GLenum pname, GLfloat* params);
        void glGetTexParameteriv(GLenum target, GLenum pname, GLint* params);
        void glGetUniformfv(GLuint program, GLint location, GLfloat* params);
        void glGetUniformiv(GLuint program, GLint location, GLint* params);
        GLint glGetUniformLocation(GLuint program, const GLchar* name);
        void glGetVertexAttribfv(GLuint index, GLenum pname, GLfloat* params);
        void glGetVertexAttribiv(GLuint index, GLenum pname, GLint* params);
        void glGetVertexAttribPointerv(GLuint index, GLenum pname, void** pointer);
        void glHint(GLenum target, GLenum mode);
        GLboolean glIsBuffer(GLuint buffer);
        GLboolean glIsEnabled(GLenum cap);
        GLboolean glIsFramebuffer(GLuint framebuffer);
        GLboolean glIsProgram(GLuint program);
        GLboolean glIsRenderbuffer(GLuint renderbuffer);
        GLboolean glIsShader(GLuint shader);
        GLboolean glIsTexture(GLuint texture);
        void glLineWidth(GLfloat width);
        void glLinkProgram(GLuint program);
        void glPixelStorei(GLenum pname, GLint param);
        void glPolygonOffset(GLfloat factor, GLfloat units);
        void glReadPixels(GLint x, GLint y, GLsizei width, GLsizei height, GLenum format, GLenum type, void* pixels);
        void glReleaseShaderCompiler(void);
        void glRenderbufferStorage(GLenum target, GLenum internalformat, GLsizei width, GLsizei height);
        void glSampleCoverage(GLfloat value, GLboolean invert);
        void glScissor(GLint x, GLint y, GLsizei width, GLsizei height);
        void glShaderBinary(GLsizei count, const GLuint* shaders, GLenum binaryformat, const void* binary, GLsizei length);
        void glShaderSource(GLuint shader, GLsizei count, const GLchar* const* string, const GLint* length);
        void glStencilFunc(GLenum func, GLint ref, GLuint mask);
        void glStencilFuncSeparate(GLenum face, GLenum func, GLint ref, GLuint mask);
        void glStencilMask(GLuint mask);
        void glStencilMaskSeparate(GLenum face, GLuint mask);
        void glStencilOp(GLenum fail, GLenum zfail, GLenum zpass);
        void glStencilOpSeparate(GLenum face, GLenum sfail, GLenum dpfail, GLenum dppass);
        void glTexImage2D(GLenum target, GLint level, GLint internalformat, GLsizei width, GLsizei height, GLint border, GLenum format, GLenum type, const void* pixels);
        void glTexParameterf(GLenum target, GLenum pname, GLfloat param);
        void glTexParameterfv(GLenum target, GLenum pname, const GLfloat* params);
        void glTexParameteri(GLenum target, GLenum pname, GLint param);
        void glTexParameteriv(GLenum target, GLenum pname, const GLint* params);
        void glTexSubImage2D(GLenum target, GLint level, GLint xoffset, GLint yoffset, GLsizei width, GLsizei height, GLenum format, GLenum type, const void* pixels);
        void glUniform1f(GLint location, GLfloat v0);
        void glUniform1fv(GLint location, GLsizei count, const GLfloat* value);
        void glUniform1i(GLint location, GLint v0);
        void glUniform1iv(GLint location, GLsizei count, const GLint* value);
        void glUniform2f(GLint location, GLfloat v0, GLfloat v1);
        void glUniform2fv(GLint location, GLsizei count, const GLfloat* value);
        void glUniform2i(GLint location, GLint v0, GLint v1);
        void glUniform2iv(GLint location, GLsizei count, const GLint* value);
        void glUniform3f(GLint location, GLfloat v0, GLfloat v1, GLfloat v2);
        void glUniform3fv(GLint location, GLsizei count, const GLfloat* value);
        void glUniform3i(GLint location, GLint v0, GLint v1, GLint v2);
        void glUniform3iv(GLint location, GLsizei count, const GLint* value);
        void glUniform4f(GLint location, GLfloat v0, GLfloat v1, GLfloat v2, GLfloat v3);
        void glUniform4fv(GLint location, GLsizei count, const GLfloat* value);
        void glUniform4i(GLint location, GLint v0, GLint v1, GLint v2, GLint v3);
        void glUniform4iv(GLint location, GLsizei count, const GLint* value);
        void glUniformMatrix2fv(GLint location, GLsizei count, GLboolean transpose, const GLfloat* value);
        void glUniformMatrix3fv(GLint location, GLsizei count, GLboolean transpose, const GLfloat* value);
        void glUniformMatrix4fv(GLint location, GLsizei count, GLboolean transpose, const GLfloat* value);
        void glUseProgram(GLuint program);
        void glValidateProgram(GLuint program);
        void glVertexAttrib1f(GLuint index, GLfloat x);
        void glVertexAttrib1fv(GLuint index, const GLfloat* v);
        void glVertexAttrib2f(GLuint index, GLfloat x, GLfloat y);
        void glVertexAttrib2fv(GLuint index, const GLfloat* v);
        void glVertexAttrib3f(GLuint index, GLfloat x, GLfloat y, GLfloat z);
        void glVertexAttrib3fv(GLuint index, const GLfloat* v);
        void glVertexAttrib4f(GLuint index, GLfloat x, GLfloat y, GLfloat z, GLfloat w);
        void glVertexAttrib4fv(GLuint index, const GLfloat* v);
        void glVertexAttribPointer(GLuint index, GLint size, GLenum type, GLboolean normalized, GLsizei stride, const void* pointer);
        void glViewport(GLint x, GLint y, GLsizei width, GLsizei height);
        
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
    
    extern void (*real_glActiveTexture)(GLenum texture);
    extern void (*real_glAttachShader)(GLuint program, GLuint shader);
    extern void (*real_glBindAttribLocation)(GLuint program, GLuint index, const GLchar* name);
    extern void (*real_glBindBuffer)(GLenum target, GLuint buffer);
    extern void (*real_glBindFramebuffer)(GLenum target, GLuint framebuffer);
    extern void (*real_glBindRenderbuffer)(GLenum target, GLuint renderbuffer);
    extern void (*real_glBindTexture)(GLenum target, GLuint texture);
    extern void (*real_glBlendColor)(GLfloat red, GLfloat green, GLfloat blue, GLfloat alpha);
    extern void (*real_glBlendEquation)(GLenum mode);
    extern void (*real_glBlendEquationSeparate)(GLenum modeRGB, GLenum modeAlpha);
    extern void (*real_glBlendFunc)(GLenum sfactor, GLenum dfactor);
    extern void (*real_glBlendFuncSeparate)(GLenum sfactorRGB, GLenum dfactorRGB, GLenum sfactorAlpha, GLenum dfactorAlpha);
    extern void (*real_glBufferData)(GLenum target, GLsizeiptr size, const void* data, GLenum usage);
    extern void (*real_glBufferSubData)(GLenum target, GLintptr offset, GLsizeiptr size, const void* data);
    extern GLenum (*real_glCheckFramebufferStatus)(GLenum target);
    extern void (*real_glClear)(GLbitfield mask);
    extern void (*real_glClearColor)(GLfloat red, GLfloat green, GLfloat blue, GLfloat alpha);
    extern void (*real_glClearDepthf)(GLfloat d);
    extern void (*real_glClearStencil)(GLint s);
    extern void (*real_glColorMask)(GLboolean red, GLboolean green, GLboolean blue, GLboolean alpha);
    extern void (*real_glCompileShader)(GLuint shader);
    extern void (*real_glCompressedTexImage2D)(GLenum target, GLint level, GLenum internalformat, GLsizei width, GLsizei height, GLint border, GLsizei imageSize, const void* data);
    extern void (*real_glCompressedTexSubImage2D)(GLenum target, GLint level, GLint xoffset, GLint yoffset, GLsizei width, GLsizei height, GLenum format, GLsizei imageSize, const void* data);
    extern void (*real_glCopyTexImage2D)(GLenum target, GLint level, GLenum internalformat, GLint x, GLint y, GLsizei width, GLsizei height, GLint border);
    extern void (*real_glCopyTexSubImage2D)(GLenum target, GLint level, GLint xoffset, GLint yoffset, GLint x, GLint y, GLsizei width, GLsizei height);
    extern GLuint (*real_glCreateProgram)(void);
    extern GLuint (*real_glCreateShader)(GLenum type);
    extern void (*real_glCullFace)(GLenum mode);
    extern void (*real_glDeleteBuffers)(GLsizei n, const GLuint* buffers);
    extern void (*real_glDeleteFramebuffers)(GLsizei n, const GLuint* framebuffers);
    extern void (*real_glDeleteProgram)(GLuint program);
    extern void (*real_glDeleteRenderbuffers)(GLsizei n, const GLuint* renderbuffers);
    extern void (*real_glDeleteShader)(GLuint shader);
    extern void (*real_glDeleteTextures)(GLsizei n, const GLuint* textures);
    extern void (*real_glDepthFunc)(GLenum func);
    extern void (*real_glDepthMask)(GLboolean flag);
    extern void (*real_glDepthRangef)(GLfloat n, GLfloat f);
    extern void (*real_glDetachShader)(GLuint program, GLuint shader);
    extern void (*real_glDisable)(GLenum cap);
    extern void (*real_glDisableVertexAttribArray)(GLuint index);
    extern void (*real_glDrawArrays)(GLenum mode, GLint first, GLsizei count);
    extern void (*real_glDrawElements)(GLenum mode, GLsizei count, GLenum type, const void* indices);
    extern void (*real_glEnable)(GLenum cap);
    extern void (*real_glEnableVertexAttribArray)(GLuint index);
    extern void (*real_glFinish)(void);
    extern void (*real_glFlush)(void);
    extern void (*real_glFramebufferRenderbuffer)(GLenum target, GLenum attachment, GLenum renderbuffertarget, GLuint renderbuffer);
    extern void (*real_glFramebufferTexture2D)(GLenum target, GLenum attachment, GLenum textarget, GLuint texture, GLint level);
    extern void (*real_glFrontFace)(GLenum mode);
    extern void (*real_glGenBuffers)(GLsizei n, GLuint* buffers);
    extern void (*real_glGenerateMipmap)(GLenum target);
    extern void (*real_glGenFramebuffers)(GLsizei n, GLuint* framebuffers);
    extern void (*real_glGenRenderbuffers)(GLsizei n, GLuint* renderbuffers);
    extern void (*real_glGenTextures)(GLsizei n, GLuint* textures);
    extern void (*real_glGetActiveAttrib)(GLuint program, GLuint index, GLsizei bufSize, GLsizei* length, GLint* size, GLenum* type, GLchar* name);
    extern void (*real_glGetActiveUniform)(GLuint program, GLuint index, GLsizei bufSize, GLsizei* length, GLint* size, GLenum* type, GLchar* name);
    extern void (*real_glGetAttachedShaders)(GLuint program, GLsizei maxCount, GLsizei* count, GLuint* shaders);
    extern GLint (*real_glGetAttribLocation)(GLuint program, const GLchar* name);
    extern void (*real_glGetBooleanv)(GLenum pname, GLboolean* data);
    extern void (*real_glGetBufferParameteriv)(GLenum target, GLenum pname, GLint* params);
    extern GLenum (*real_glGetError)(void);
    extern void (*real_glGetFloatv)(GLenum pname, GLfloat* data);
    extern void (*real_glGetFramebufferAttachmentParameteriv)(GLenum target, GLenum attachment, GLenum pname, GLint* params);
    extern void (*real_glGetIntegerv)(GLenum pname, GLint* data);
    extern void (*real_glGetProgramiv)(GLuint program, GLenum pname, GLint* params);
    extern void (*real_glGetProgramInfoLog)(GLuint program, GLsizei bufSize, GLsizei* length, GLchar* infoLog);
    extern void (*real_glGetRenderbufferParameteriv)(GLenum target, GLenum pname, GLint* params);
    extern void (*real_glGetShaderiv)(GLuint shader, GLenum pname, GLint* params);
    extern void (*real_glGetShaderInfoLog)(GLuint shader, GLsizei bufSize, GLsizei* length, GLchar* infoLog);
    extern void (*real_glGetShaderPrecisionFormat)(GLenum shadertype, GLenum precisiontype, GLint* range, GLint* precision);
    extern void (*real_glGetShaderSource)(GLuint shader, GLsizei bufSize, GLsizei* length, GLchar* source);
    extern const GLubyte* (*real_glGetString)(GLenum name);
    extern void (*real_glGetTexParameterfv)(GLenum target, GLenum pname, GLfloat* params);
    extern void (*real_glGetTexParameteriv)(GLenum target, GLenum pname, GLint* params);
    extern void (*real_glGetUniformfv)(GLuint program, GLint location, GLfloat* params);
    extern void (*real_glGetUniformiv)(GLuint program, GLint location, GLint* params);
    extern GLint (*real_glGetUniformLocation)(GLuint program, const GLchar* name);
    extern void (*real_glGetVertexAttribfv)(GLuint index, GLenum pname, GLfloat* params);
    extern void (*real_glGetVertexAttribiv)(GLuint index, GLenum pname, GLint* params);
    extern void (*real_glGetVertexAttribPointerv)(GLuint index, GLenum pname, void** pointer);
    extern void (*real_glHint)(GLenum target, GLenum mode);
    extern GLboolean (*real_glIsBuffer)(GLuint buffer);
    extern GLboolean (*real_glIsEnabled)(GLenum cap);
    extern GLboolean (*real_glIsFramebuffer)(GLuint framebuffer);
    extern GLboolean (*real_glIsProgram)(GLuint program);
    extern GLboolean (*real_glIsRenderbuffer)(GLuint renderbuffer);
    extern GLboolean (*real_glIsShader)(GLuint shader);
    extern GLboolean (*real_glIsTexture)(GLuint texture);
    extern void (*real_glLineWidth)(GLfloat width);
    extern void (*real_glLinkProgram)(GLuint program);
    extern void (*real_glPixelStorei)(GLenum pname, GLint param);
    extern void (*real_glPolygonOffset)(GLfloat factor, GLfloat units);
    extern void (*real_glReadPixels)(GLint x, GLint y, GLsizei width, GLsizei height, GLenum format, GLenum type, void* pixels);
    extern void (*real_glReleaseShaderCompiler)(void);
    extern void (*real_glRenderbufferStorage)(GLenum target, GLenum internalformat, GLsizei width, GLsizei height);
    extern void (*real_glSampleCoverage)(GLfloat value, GLboolean invert);
    extern void (*real_glScissor)(GLint x, GLint y, GLsizei width, GLsizei height);
    extern void (*real_glShaderBinary)(GLsizei count, const GLuint* shaders, GLenum binaryformat, const void* binary, GLsizei length);
    extern void (*real_glShaderSource)(GLuint shader, GLsizei count, const GLchar* const* string, const GLint* length);
    extern void (*real_glStencilFunc)(GLenum func, GLint ref, GLuint mask);
    extern void (*real_glStencilFuncSeparate)(GLenum face, GLenum func, GLint ref, GLuint mask);
    extern void (*real_glStencilMask)(GLuint mask);
    extern void (*real_glStencilMaskSeparate)(GLenum face, GLuint mask);
    extern void (*real_glStencilOp)(GLenum fail, GLenum zfail, GLenum zpass);
    extern void (*real_glStencilOpSeparate)(GLenum face, GLenum sfail, GLenum dpfail, GLenum dppass);
    extern void (*real_glTexImage2D)(GLenum target, GLint level, GLint internalformat, GLsizei width, GLsizei height, GLint border, GLenum format, GLenum type, const void* pixels);
    extern void (*real_glTexParameterf)(GLenum target, GLenum pname, GLfloat param);
    extern void (*real_glTexParameterfv)(GLenum target, GLenum pname, const GLfloat* params);
    extern void (*real_glTexParameteri)(GLenum target, GLenum pname, GLint param);
    extern void (*real_glTexParameteriv)(GLenum target, GLenum pname, const GLint* params);
    extern void (*real_glTexSubImage2D)(GLenum target, GLint level, GLint xoffset, GLint yoffset, GLsizei width, GLsizei height, GLenum format, GLenum type, const void* pixels);
    extern void (*real_glUniform1f)(GLint location, GLfloat v0);
    extern void (*real_glUniform1fv)(GLint location, GLsizei count, const GLfloat* value);
    extern void (*real_glUniform1i)(GLint location, GLint v0);
    extern void (*real_glUniform1iv)(GLint location, GLsizei count, const GLint* value);
    extern void (*real_glUniform2f)(GLint location, GLfloat v0, GLfloat v1);
    extern void (*real_glUniform2fv)(GLint location, GLsizei count, const GLfloat* value);
    extern void (*real_glUniform2i)(GLint location, GLint v0, GLint v1);
    extern void (*real_glUniform2iv)(GLint location, GLsizei count, const GLint* value);
    extern void (*real_glUniform3f)(GLint location, GLfloat v0, GLfloat v1, GLfloat v2);
    extern void (*real_glUniform3fv)(GLint location, GLsizei count, const GLfloat* value);
    extern void (*real_glUniform3i)(GLint location, GLint v0, GLint v1, GLint v2);
    extern void (*real_glUniform3iv)(GLint location, GLsizei count, const GLint* value);
    extern void (*real_glUniform4f)(GLint location, GLfloat v0, GLfloat v1, GLfloat v2, GLfloat v3);
    extern void (*real_glUniform4fv)(GLint location, GLsizei count, const GLfloat* value);
    extern void (*real_glUniform4i)(GLint location, GLint v0, GLint v1, GLint v2, GLint v3);
    extern void (*real_glUniform4iv)(GLint location, GLsizei count, const GLint* value);
    extern void (*real_glUniformMatrix2fv)(GLint location, GLsizei count, GLboolean transpose, const GLfloat* value);
    extern void (*real_glUniformMatrix3fv)(GLint location, GLsizei count, GLboolean transpose, const GLfloat* value);
    extern void (*real_glUniformMatrix4fv)(GLint location, GLsizei count, GLboolean transpose, const GLfloat* value);
    extern void (*real_glUseProgram)(GLuint program);
    extern void (*real_glValidateProgram)(GLuint program);
    extern void (*real_glVertexAttrib1f)(GLuint index, GLfloat x);
    extern void (*real_glVertexAttrib1fv)(GLuint index, const GLfloat* v);
    extern void (*real_glVertexAttrib2f)(GLuint index, GLfloat x, GLfloat y);
    extern void (*real_glVertexAttrib2fv)(GLuint index, const GLfloat* v);
    extern void (*real_glVertexAttrib3f)(GLuint index, GLfloat x, GLfloat y, GLfloat z);
    extern void (*real_glVertexAttrib3fv)(GLuint index, const GLfloat* v);
    extern void (*real_glVertexAttrib4f)(GLuint index, GLfloat x, GLfloat y, GLfloat z, GLfloat w);
    extern void (*real_glVertexAttrib4fv)(GLuint index, const GLfloat* v);
    extern void (*real_glVertexAttribPointer)(GLuint index, GLint size, GLenum type, GLboolean normalized, GLsizei stride, const void* pointer);
    extern void (*real_glViewport)(GLint x, GLint y, GLsizei width, GLsizei height);
    
    
    
    
    
    
    
    
    
}


/**
 * @brief Converts exceptions to EGL error codes.
 * 
 */
#define CATCH_EGL_EXCEPTIONS \
    catch(const egl_wrapper::Exceptions::NotInitializedException&) { lastError = (EGL_NOT_INITIALIZED); } \
    catch(const egl_wrapper::Exceptions::BadAccessException&) { lastError = (EGL_BAD_ACCESS); } \
    catch(const egl_wrapper::Exceptions::BadAllocException&) { lastError = (EGL_BAD_ALLOC); } \
    catch(const egl_wrapper::Exceptions::BadAttributeException&) { lastError = (EGL_BAD_ATTRIBUTE); } \
    catch(const egl_wrapper::Exceptions::BadContextException&) { lastError = (EGL_BAD_CONTEXT); } \
    catch(const egl_wrapper::Exceptions::BadConfigException&) { lastError = (EGL_BAD_CONFIG); } \
    catch(const egl_wrapper::Exceptions::BadCurrentSurfaceException&) { lastError = (EGL_BAD_CURRENT_SURFACE); } \
    catch(const egl_wrapper::Exceptions::BadDisplayException&) { lastError = (EGL_BAD_DISPLAY); } \
    catch(const egl_wrapper::Exceptions::BadSurfaceException&) { lastError = (EGL_BAD_SURFACE); } \
    catch(const egl_wrapper::Exceptions::BadMatchException&) { lastError = (EGL_BAD_MATCH); } \
    catch(const egl_wrapper::Exceptions::BadParameterException&) { lastError = (EGL_BAD_PARAMETER); } \
    catch(const egl_wrapper::Exceptions::BadNativePixmapException&) { lastError = (EGL_BAD_NATIVE_PIXMAP); } \
    catch(const egl_wrapper::Exceptions::BadNativeWindowException&) { lastError = (EGL_BAD_NATIVE_WINDOW); } \
    catch(const egl_wrapper::Exceptions::ContextLostException&) { lastError = (EGL_CONTEXT_LOST); }




