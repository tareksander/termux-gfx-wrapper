#pragma once

#define EGL_EGL_PROTOTYPES 0
#include <EGL/egl.h>
#include <EGL/eglext.h>

#define EGL_PLATFORM_XCB_EXT              0x31DC
#define EGL_PLATFORM_XCB_SCREEN_EXT       0x31DE

#include <GLES2/gl2.h>
#include <GLES2/gl2ext.h>
#include <dlfcn.h>



#include "android.hpp"


#include <glvnd/libeglabi.h>


#include <mutex>
#include <unordered_map>
#include <exception>



#ifdef X11_PLATFORM
    #include <xcb/xcb.h>
    #include <xcb/xproto.h>
    #include <xcb/shm.h>
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
        X11
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
        
        
        
        
        
        
        EGLDisplayBackend(const EGLDisplayBackend&) = delete;
        EGLDisplayBackend(EGLDisplayBackend&&) = delete;
        EGLDisplayBackend& operator=(const EGLDisplayBackend&) = delete;
        EGLDisplayBackend& operator=(EGLDisplayBackend&&) = delete;
        
        
        EGLDisplayBackend() {};
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
        EGLBoolean eglMakeCurrent(EGLSurface draw, EGLSurface read, EGLContext ctx) {
            // You can get the current context from libglvnd, so just light error checking here.
            if ((draw == EGL_NO_SURFACE && read != EGL_NO_SURFACE) || (draw != EGL_NO_SURFACE && read == EGL_NO_SURFACE)) return EGL_FALSE;
            return EGL_TRUE;
        }
        virtual EGLBoolean eglQueryContext(EGLContext ctx, EGLint attribute, EGLint* value) = 0;
        virtual const char* eglQueryString(EGLint name) = 0;
        virtual EGLBoolean eglQuerySurface(EGLSurface surface, EGLint attribute, EGLint* value) = 0;
        virtual EGLBoolean eglSwapBuffers(EGLSurface surface) = 0;
        virtual EGLBoolean eglTerminate() = 0;
        virtual EGLBoolean eglWaitGL(void) = 0;
        virtual EGLBoolean eglWaitNative(EGLint engine) = 0;
        
        // EGL 1.1
        virtual EGLBoolean eglBindTexImage(EGLSurface surface, EGLint buffer) = 0;
        virtual EGLBoolean eglReleaseTexImage(EGLSurface surface, EGLint buffer) = 0;
        virtual EGLBoolean eglSurfaceAttrib(EGLSurface surface, EGLint attribute, EGLint value) = 0;
        virtual EGLBoolean eglSwapInterval(EGLint interval) = 0;
        
        // EGL 1.2
        virtual EGLSurface eglCreatePbufferFromClientBuffer(EGLenum buftype, EGLClientBuffer buffer, EGLConfig config, const EGLint* attrib_list) = 0;
        virtual EGLBoolean eglReleaseThread(void) = 0;
        virtual EGLBoolean eglWaitClient(void) = 0;
        
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
        EGLBoolean eglQueryContext(EGLContext ctx, EGLint attribute, EGLint* value);
        const char* eglQueryString(EGLint name);
        EGLBoolean eglQuerySurface(EGLSurface surface, EGLint attribute, EGLint* value);
        EGLBoolean eglSwapBuffers(EGLSurface surface);
        EGLBoolean eglTerminate();
        EGLBoolean eglWaitGL(void);
        EGLBoolean eglWaitNative(EGLint engine);
        
        // EGL 1.1
        EGLBoolean eglBindTexImage(EGLSurface surface, EGLint buffer);
        EGLBoolean eglReleaseTexImage(EGLSurface surface, EGLint buffer);
        EGLBoolean eglSurfaceAttrib(EGLSurface surface, EGLint attribute, EGLint value);
        EGLBoolean eglSwapInterval(EGLint interval);
        
        // EGL 1.2
        EGLSurface eglCreatePbufferFromClientBuffer(EGLenum buftype, EGLClientBuffer buffer, EGLConfig config, const EGLint* attrib_list);
        EGLBoolean eglReleaseThread(void);
        EGLBoolean eglWaitClient(void);
        
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
            EGLBoolean eglQueryContext(EGLContext ctx, EGLint attribute, EGLint* value);
            const char* eglQueryString(EGLint name);
            EGLBoolean eglQuerySurface(EGLSurface surface, EGLint attribute, EGLint* value);
            EGLBoolean eglSwapBuffers(EGLSurface surface);
            EGLBoolean eglTerminate();
            EGLBoolean eglWaitGL(void);
            EGLBoolean eglWaitNative(EGLint engine);
            
            // EGL 1.1
            EGLBoolean eglBindTexImage(EGLSurface surface, EGLint buffer);
            EGLBoolean eglReleaseTexImage(EGLSurface surface, EGLint buffer);
            EGLBoolean eglSurfaceAttrib(EGLSurface surface, EGLint attribute, EGLint value);
            EGLBoolean eglSwapInterval(EGLint interval);
            
            // EGL 1.2
            EGLSurface eglCreatePbufferFromClientBuffer(EGLenum buftype, EGLClientBuffer buffer, EGLConfig config, const EGLint* attrib_list);
            EGLBoolean eglReleaseThread(void);
            EGLBoolean eglWaitClient(void);
            
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
    
    /// @brief Lock for dispatchIndexMap.
    extern std::mutex dispatchLock;
    extern std::unordered_map<std::string, int> dispatchIndexMap;
    
    
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
        // no dispatch needed
        //EGLint eglGetError(void);
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
        extern PFNEGLDESTROYIMAGEKHRPROC eglDestroyImageKHR;
        
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
    
    
    
    
    
    
    
    
    
    
    
    
}


/**
 * @brief Converts exceptions to EGL error codes.
 * 
 */
#define CATCH_EGL_EXCEPTIONS \
    catch(const egl_wrapper::Exceptions::NotInitializedException&) { glvnd->setEGLError(EGL_NOT_INITIALIZED); } \
    catch(const egl_wrapper::Exceptions::BadAccessException&) { glvnd->setEGLError(EGL_BAD_ACCESS); } \
    catch(const egl_wrapper::Exceptions::BadAllocException&) { glvnd->setEGLError(EGL_BAD_ALLOC); } \
    catch(const egl_wrapper::Exceptions::BadAttributeException&) { glvnd->setEGLError(EGL_BAD_ATTRIBUTE); } \
    catch(const egl_wrapper::Exceptions::BadContextException&) { glvnd->setEGLError(EGL_BAD_CONTEXT); } \
    catch(const egl_wrapper::Exceptions::BadConfigException&) { glvnd->setEGLError(EGL_BAD_CONFIG); } \
    catch(const egl_wrapper::Exceptions::BadCurrentSurfaceException&) { glvnd->setEGLError(EGL_BAD_CURRENT_SURFACE); } \
    catch(const egl_wrapper::Exceptions::BadDisplayException&) { glvnd->setEGLError(EGL_BAD_DISPLAY); } \
    catch(const egl_wrapper::Exceptions::BadSurfaceException&) { glvnd->setEGLError(EGL_BAD_SURFACE); } \
    catch(const egl_wrapper::Exceptions::BadMatchException&) { glvnd->setEGLError(EGL_BAD_MATCH); } \
    catch(const egl_wrapper::Exceptions::BadParameterException&) { glvnd->setEGLError(EGL_BAD_PARAMETER); } \
    catch(const egl_wrapper::Exceptions::BadNativePixmapException&) { glvnd->setEGLError(EGL_BAD_NATIVE_PIXMAP); } \
    catch(const egl_wrapper::Exceptions::BadNativeWindowException&) { glvnd->setEGLError(EGL_BAD_NATIVE_WINDOW); } \
    catch(const egl_wrapper::Exceptions::ContextLostException&) { glvnd->setEGLError(EGL_CONTEXT_LOST); }




