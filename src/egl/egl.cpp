#include "egl.hpp"


#include <stdio.h>
#include <stdlib.h>
#include <string.h>


#include <string>
#include <vector>


/**
 * @file
 * @brief libglvnd configuration and general initialisation
 * This file contains library initialisation, deinitialisation and libglvnd setup and glue code to the displays.
 * 
 */
















/**
 * @brief Generate and if statement that returns the correct function pointer for the function name.
 * 
 */
#define GENERATE_DISPATCH_IF(funcname) if (strcmp(#funcname, procName) == 0) return (void*) egl_wrapper::dispatch::funcname;


namespace egl_wrapper {
    
    AndroidDisplay androidDisplay;
    std::mutex displayListLock;
    #ifdef X11_PLATFORM
        std::unordered_map<void*, X11Display*> xlibDisplayList;
        std::unordered_map<void*, X11Display*> xcbDisplayList;
    #endif
    #ifdef WAYLAND_PLATFORM
        std::unordered_map<void*, WaylandDisplay*> waylandDisplayList;
    #endif
    void* nativeEGLLibrary = NULL;
    libAndroidHelper::LibAndroid libandroid;
    const __EGLapiExports* glvnd = NULL;
    __EGLvendorInfo *thisVendor = NULL;
    bool hwbufferRenderingAvailable = true;
    DisplayType defaultDisplayType = DisplayType::DEFAULT_DISPLAY_PLAYFORM;
    EGLDisplay nativeDisplay = EGL_NO_DISPLAY;
    std::mutex dispatchLock;
    std::unordered_map<std::string, int> dispatchIndexMap;
    
    
    PFNEGLCHOOSECONFIGPROC real_eglChooseConfig = NULL;
    PFNEGLCOPYBUFFERSPROC real_eglCopyBuffers = NULL;
    PFNEGLCREATECONTEXTPROC real_eglCreateContext = NULL;
    PFNEGLCREATEPBUFFERSURFACEPROC real_eglCreatePbufferSurface = NULL;
    PFNEGLCREATEPIXMAPSURFACEPROC real_eglCreatePixmapSurface = NULL;
    PFNEGLCREATEWINDOWSURFACEPROC real_eglCreateWindowSurface = NULL;
    PFNEGLDESTROYCONTEXTPROC real_eglDestroyContext = NULL;
    PFNEGLDESTROYSURFACEPROC real_eglDestroySurface = NULL;
    PFNEGLGETCONFIGATTRIBPROC real_eglGetConfigAttrib = NULL;
    PFNEGLGETCONFIGSPROC real_eglGetConfigs = NULL;
    PFNEGLGETCURRENTDISPLAYPROC real_eglGetCurrentDisplay = NULL;
    PFNEGLGETCURRENTSURFACEPROC real_eglGetCurrentSurface = NULL;
    PFNEGLGETDISPLAYPROC real_eglGetDisplay = NULL;
    PFNEGLGETERRORPROC real_eglGetError = NULL;
    PFNEGLGETPROCADDRESSPROC real_eglGetProcAddress = NULL;
    PFNEGLINITIALIZEPROC real_eglInitialize = NULL;
    PFNEGLMAKECURRENTPROC real_eglMakeCurrent = NULL;
    PFNEGLQUERYCONTEXTPROC real_eglQueryContext = NULL;
    PFNEGLQUERYSTRINGPROC real_eglQueryString = NULL;
    PFNEGLQUERYSURFACEPROC real_eglQuerySurface = NULL;
    PFNEGLSWAPBUFFERSPROC real_eglSwapBuffers = NULL;
    PFNEGLTERMINATEPROC real_eglTerminate = NULL;
    PFNEGLWAITGLPROC real_eglWaitGL = NULL;
    PFNEGLWAITNATIVEPROC real_eglWaitNative = NULL;
    PFNEGLBINDTEXIMAGEPROC real_eglBindTexImage = NULL;
    PFNEGLRELEASETEXIMAGEPROC real_eglReleaseTexImage = NULL;
    PFNEGLSURFACEATTRIBPROC real_eglSurfaceAttrib = NULL;
    PFNEGLSWAPINTERVALPROC real_eglSwapInterval = NULL;
    PFNEGLBINDAPIPROC real_eglBindAPI = NULL;
    PFNEGLQUERYAPIPROC real_eglQueryAPI = NULL;
    PFNEGLCREATEPBUFFERFROMCLIENTBUFFERPROC real_eglCreatePbufferFromClientBuffer = NULL;
    PFNEGLRELEASETHREADPROC real_eglReleaseThread = NULL;
    PFNEGLWAITCLIENTPROC real_eglWaitClient = NULL;
    PFNEGLGETCURRENTCONTEXTPROC real_eglGetCurrentContext = NULL;
    PFNEGLCREATESYNCPROC real_eglCreateSync = NULL;
    PFNEGLDESTROYSYNCPROC real_eglDestroySync = NULL;
    PFNEGLCLIENTWAITSYNCPROC real_eglClientWaitSync = NULL;
    PFNEGLGETSYNCATTRIBPROC real_eglGetSyncAttrib = NULL;
    PFNEGLCREATEIMAGEPROC real_eglCreateImage = NULL;
    PFNEGLDESTROYIMAGEPROC real_eglDestroyImage = NULL;
    PFNEGLGETPLATFORMDISPLAYPROC real_eglGetPlatformDisplay = NULL;
    PFNEGLCREATEPLATFORMWINDOWSURFACEPROC real_eglCreatePlatformWindowSurface = NULL;
    PFNEGLCREATEPLATFORMPIXMAPSURFACEPROC real_eglCreatePlatformPixmapSurface = NULL;
    PFNEGLWAITSYNCPROC real_eglWaitSync = NULL;
    
    
    PFNEGLDESTROYIMAGEKHRPROC dispatch::eglDestroyImageKHR = eglDestroyImage;
    
    
    
    /**
     * @brief Perform library initialisation, should be called by eglMain at libglvnd initialisation.
     * 
     * @return EGLBoolean Whether the initialisation was successful.
     */
    EGLBoolean egl_wrapper_init()  {
        // search for the native libEGL.so
        nativeEGLLibrary = dlopen("/system/lib64/libEGL.so", RTLD_LAZY | RTLD_LOCAL);
        if (nativeEGLLibrary == NULL) {
            nativeEGLLibrary = dlopen("/system/lib/libEGL.so", RTLD_LAZY | RTLD_LOCAL);
        }
        if (nativeEGLLibrary == NULL) {
            fprintf(stderr, "ERROR: Could not find Android libEGL.so in /system/lib64 or /system/lib\n");
            fflush(stderr);
            return EGL_FALSE;
        }
        
        // get all EGL functions
        real_eglChooseConfig = (PFNEGLCHOOSECONFIGPROC) dlsym(nativeEGLLibrary, "eglChooseConfig");
        real_eglCopyBuffers = (PFNEGLCOPYBUFFERSPROC) dlsym(nativeEGLLibrary, "eglCopyBuffers");
        real_eglCreateContext = (PFNEGLCREATECONTEXTPROC) dlsym(nativeEGLLibrary, "eglCreateContext");
        real_eglCreatePbufferSurface = (PFNEGLCREATEPBUFFERSURFACEPROC) dlsym(nativeEGLLibrary, "eglCreatePbufferSurface");
        real_eglCreatePixmapSurface = (PFNEGLCREATEPIXMAPSURFACEPROC) dlsym(nativeEGLLibrary, "eglCreatePixmapSurface");
        real_eglCreateWindowSurface = (PFNEGLCREATEWINDOWSURFACEPROC) dlsym(nativeEGLLibrary, "eglCreateWindowSurface");
        real_eglDestroyContext = (PFNEGLDESTROYCONTEXTPROC) dlsym(nativeEGLLibrary, "eglDestroyContext");
        real_eglDestroySurface = (PFNEGLDESTROYSURFACEPROC) dlsym(nativeEGLLibrary, "eglDestroySurface");
        real_eglGetConfigAttrib = (PFNEGLGETCONFIGATTRIBPROC) dlsym(nativeEGLLibrary, "eglGetConfigAttrib");
        real_eglGetConfigs = (PFNEGLGETCONFIGSPROC) dlsym(nativeEGLLibrary, "eglGetConfigs");
        real_eglGetCurrentDisplay = (PFNEGLGETCURRENTDISPLAYPROC) dlsym(nativeEGLLibrary, "eglGetCurrentDisplay");
        real_eglGetCurrentSurface = (PFNEGLGETCURRENTSURFACEPROC) dlsym(nativeEGLLibrary, "eglGetCurrentSurface");
        real_eglGetDisplay = (PFNEGLGETDISPLAYPROC) dlsym(nativeEGLLibrary, "eglGetDisplay");
        real_eglGetError = (PFNEGLGETERRORPROC) dlsym(nativeEGLLibrary, "eglGetError");
        real_eglGetProcAddress = (PFNEGLGETPROCADDRESSPROC) dlsym(nativeEGLLibrary, "eglGetProcAddress");
        real_eglInitialize = (PFNEGLINITIALIZEPROC) dlsym(nativeEGLLibrary, "eglInitialize");
        real_eglMakeCurrent = (PFNEGLMAKECURRENTPROC) dlsym(nativeEGLLibrary, "eglMakeCurrent");
        real_eglQueryContext = (PFNEGLQUERYCONTEXTPROC) dlsym(nativeEGLLibrary, "eglQueryContext");
        real_eglQueryString = (PFNEGLQUERYSTRINGPROC) dlsym(nativeEGLLibrary, "eglQueryString");
        real_eglQuerySurface = (PFNEGLQUERYSURFACEPROC) dlsym(nativeEGLLibrary, "eglQuerySurface");
        real_eglSwapBuffers = (PFNEGLSWAPBUFFERSPROC) dlsym(nativeEGLLibrary, "eglSwapBuffers");
        real_eglTerminate = (PFNEGLTERMINATEPROC) dlsym(nativeEGLLibrary, "eglTerminate");
        real_eglWaitGL = (PFNEGLWAITGLPROC) dlsym(nativeEGLLibrary, "eglWaitGL");
        real_eglWaitNative = (PFNEGLWAITNATIVEPROC) dlsym(nativeEGLLibrary, "eglWaitNative");
        
        real_eglBindTexImage = (PFNEGLBINDTEXIMAGEPROC) dlsym(nativeEGLLibrary, "eglBindTexImage");
        real_eglReleaseTexImage = (PFNEGLRELEASETEXIMAGEPROC) dlsym(nativeEGLLibrary, "eglReleaseTexImage");
        real_eglSurfaceAttrib = (PFNEGLSURFACEATTRIBPROC) dlsym(nativeEGLLibrary, "eglSurfaceAttrib");
        real_eglSwapInterval = (PFNEGLSWAPINTERVALPROC) dlsym(nativeEGLLibrary, "eglSwapInterval");
        
        real_eglBindAPI = (PFNEGLBINDAPIPROC) dlsym(nativeEGLLibrary, "eglBindAPI");
        real_eglQueryAPI = (PFNEGLQUERYAPIPROC) dlsym(nativeEGLLibrary, "eglQueryAPI");
        real_eglCreatePbufferFromClientBuffer = (PFNEGLCREATEPBUFFERFROMCLIENTBUFFERPROC) dlsym(nativeEGLLibrary, "eglCreatePbufferFromClientBuffer");
        real_eglReleaseThread = (PFNEGLRELEASETHREADPROC) dlsym(nativeEGLLibrary, "eglReleaseThread");
        real_eglWaitClient = (PFNEGLWAITCLIENTPROC) dlsym(nativeEGLLibrary, "eglWaitClient");
        
        real_eglGetCurrentContext = (PFNEGLGETCURRENTCONTEXTPROC) dlsym(nativeEGLLibrary, "eglGetCurrentContext");
        
        real_eglCreateSync = (PFNEGLCREATESYNCPROC) dlsym(nativeEGLLibrary, "eglCreateSync");
        real_eglDestroySync = (PFNEGLDESTROYSYNCPROC) dlsym(nativeEGLLibrary, "eglDestroySync");
        real_eglClientWaitSync = (PFNEGLCLIENTWAITSYNCPROC) dlsym(nativeEGLLibrary, "eglClientWaitSync");
        real_eglGetSyncAttrib = (PFNEGLGETSYNCATTRIBPROC) dlsym(nativeEGLLibrary, "eglGetSyncAttrib");
        real_eglCreateImage = (PFNEGLCREATEIMAGEPROC) dlsym(nativeEGLLibrary, "eglCreateImage");
        real_eglDestroyImage = (PFNEGLDESTROYIMAGEPROC) dlsym(nativeEGLLibrary, "eglDestroyImage");
        real_eglGetPlatformDisplay = (PFNEGLGETPLATFORMDISPLAYPROC) dlsym(nativeEGLLibrary, "eglGetPlatformDisplay");
        real_eglCreatePlatformWindowSurface = (PFNEGLCREATEPLATFORMWINDOWSURFACEPROC) dlsym(nativeEGLLibrary, "eglCreatePlatformWindowSurface");
        real_eglCreatePlatformPixmapSurface = (PFNEGLCREATEPLATFORMPIXMAPSURFACEPROC) dlsym(nativeEGLLibrary, "eglCreatePlatformPixmapSurface");
        real_eglWaitSync = (PFNEGLWAITSYNCPROC) dlsym(nativeEGLLibrary, "eglWaitSync");
        
        
        // check needed EGL functions
        if ( // EGL 1.0 functions
            real_eglChooseConfig == NULL ||
            real_eglCreateContext == NULL ||
            real_eglCreatePbufferSurface == NULL ||
            real_eglDestroyContext == NULL ||
            real_eglDestroySurface == NULL ||
            real_eglGetConfigAttrib == NULL ||
            real_eglGetConfigs == NULL ||
            real_eglGetCurrentSurface == NULL ||
            real_eglGetDisplay == NULL ||
            real_eglGetError == NULL ||
            real_eglGetProcAddress == NULL ||
            real_eglInitialize == NULL ||
            real_eglMakeCurrent == NULL ||
            real_eglQueryContext == NULL ||
            real_eglQueryString == NULL ||
            real_eglQuerySurface == NULL ||
            real_eglSwapBuffers == NULL ||
            real_eglTerminate == NULL ||
            real_eglWaitGL == NULL ||
            // EGL 1.1 functions
            real_eglBindTexImage == NULL ||
            real_eglReleaseTexImage == NULL ||
            real_eglSurfaceAttrib == NULL ||
            real_eglSwapInterval == NULL ||
            // EGL 1.2 functions
            real_eglBindAPI == NULL ||
            real_eglQueryAPI == NULL ||
            real_eglReleaseThread == NULL ||
            real_eglWaitClient == NULL ||
            // EGL 1.4 functions
            real_eglGetCurrentContext == NULL
            ) {
            fprintf(stderr, "ERROR: Core EGL functions missing from Android EGL\n");
            fflush(stderr);
            return EGL_FALSE;
        }
        
        
        nativeDisplay = real_eglGetDisplay(EGL_DEFAULT_DISPLAY);
        if (nativeDisplay == EGL_NO_DISPLAY) {
            fprintf(stderr, "ERROR: Could not get EGLDisplay\n");
            fflush(stderr);
            return EGL_FALSE;
        }
        EGLint major, minor;
        if (real_eglInitialize(nativeDisplay, &major, &minor) == EGL_FALSE) {
            fprintf(stderr, "ERROR: Could not init EGLDisplay\n");
            fflush(stderr);
            return EGL_FALSE;
        }
        
        // loop, so we can break out of the block without having to use exceptions
        while (true) {
            libAndroidHelper::init(libandroid);
            // TODO maybe convert the error statements to warning statements that can be silenced by an env variable
            if (! libAndroidHelper::hwbufferAvailable(libandroid)) {
                // fprintf(stderr, "ERROR: Hardware buffers not available, EGL emulation not possible\n");
                // fflush(stderr);
                // return EGL_FALSE;
                hwbufferRenderingAvailable = false;
                break;
            }
            
            const char* exts_raw = real_eglQueryString(nativeDisplay, EGL_EXTENSIONS);
            if (exts_raw == NULL) {
                // fprintf(stderr, "ERROR: EGL client extensions not supported, EGL emulation not possible\n");
                // fflush(stderr);
                // return EGL_FALSE;
                hwbufferRenderingAvailable = false;
                break;
            }
            std::string exts(exts_raw);
            if (exts.find("EGL_KHR_image_base") == std::string::npos) {
                // fprintf(stderr, "ERROR: EGL_KHR_image_base extension not supported, EGL emulation not possible\n");
                // fflush(stderr);
                // return EGL_FALSE;
                hwbufferRenderingAvailable = false;
                break;
            }
            if (exts.find("EGL_ANDROID_image_native_buffer") == std::string::npos) {
                // fprintf(stderr, "ERROR: EGL_ANDROID_image_native_buffer extension not supported, EGL emulation not possible\n");
                // fflush(stderr);
                // return EGL_FALSE;
                hwbufferRenderingAvailable = false;
                break;
            }
            if (exts.find("EGL_ANDROID_get_native_client_buffer") == std::string::npos) {
                // fprintf(stderr, "ERROR: EGL_ANDROID_get_native_client_buffer extension not supported, EGL emulation not possible\n");
                // fflush(stderr);
                // return EGL_FALSE;
                hwbufferRenderingAvailable = false;
                break;
            }
            
            EGLint configSize = 0;
            EGLConfig conf;
            EGLint config_attributes[] = { EGL_RENDERABLE_TYPE, EGL_OPENGL_ES2_BIT, EGL_SURFACE_TYPE, EGL_PBUFFER_BIT, EGL_NONE };
            if (real_eglChooseConfig(nativeDisplay, config_attributes, &conf, 1, &configSize) == EGL_FALSE || configSize == 0) {
                // fprintf(stderr, "ERROR: GLES error, EGL emulation not possible\n");
                // fflush(stderr);
                // return EGL_FALSE;
                hwbufferRenderingAvailable = false;
                break;
            }
            EGLint context_attributes[] = { EGL_CONTEXT_MAJOR_VERSION, 2, EGL_CONTEXT_MINOR_VERSION, 0, EGL_NONE };
            EGLContext ctx = real_eglCreateContext(nativeDisplay, conf, EGL_NO_CONTEXT, context_attributes);
            if (ctx == EGL_NO_CONTEXT) {
                // fprintf(stderr, "ERROR: GLES error, EGL emulation not possible\n");
                // fflush(stderr);
                // return EGL_FALSE;
                hwbufferRenderingAvailable = false;
                break;
            }
            EGLint pbAttrib[] = { EGL_WIDTH, 1, EGL_HEIGHT, 1, EGL_NONE };
            EGLSurface pbs = EGL_NO_SURFACE;
            if (real_eglCreatePbufferSurface(nativeDisplay, conf, pbAttrib) == EGL_FALSE) {
                real_eglDestroyContext(nativeDisplay, ctx);
                break;
            }
            
            if (real_eglMakeCurrent(nativeDisplay, pbs, pbs, ctx) == EGL_FALSE) {
                // fprintf(stderr, "ERROR: GLES error, EGL emulation not possible\n");
                // fflush(stderr);
                // return EGL_FALSE;
                hwbufferRenderingAvailable = false;
                real_eglDestroyContext(nativeDisplay, ctx);
                real_eglDestroySurface(nativeDisplay, pbs);
                break;
            }
            // check needed GLES extensions: GL_OES_EGL_image and GL_OES_framebuffer_object
            
            PFNGLGETSTRINGPROC real_glGetString = (PFNGLGETSTRINGPROC) real_eglGetProcAddress("glGetString");
            std::string gl_exts((char*) real_glGetString(GL_EXTENSIONS));
            if (gl_exts.find("GL_OES_EGL_image") == std::string::npos) {
                // fprintf(stderr, "ERROR: GL_OES_EGL_image extension not supported, EGL emulation not possible\n");
                // fflush(stderr);
                // return EGL_FALSE;
                hwbufferRenderingAvailable = false;
                real_eglDestroyContext(nativeDisplay, ctx);
                real_eglDestroySurface(nativeDisplay, pbs);
                break;
            }
            if (gl_exts.find("GL_OES_framebuffer_object") == std::string::npos) {
                // fprintf(stderr, "ERROR: GL_OES_framebuffer_object extension not supported, EGL emulation not possible\n");
                // fflush(stderr);
                // return EGL_FALSE;
                hwbufferRenderingAvailable = false;
                real_eglDestroyContext(nativeDisplay, ctx);
                real_eglDestroySurface(nativeDisplay, pbs);
                break;
            }
            real_eglMakeCurrent(nativeDisplay, EGL_NO_SURFACE, EGL_NO_SURFACE, EGL_NO_CONTEXT);
            real_eglDestroyContext(nativeDisplay, ctx);
            real_eglDestroySurface(nativeDisplay, pbs);
            break;
        }
        
        // get the default platform backend from the env variable, if set
        const char* priority_backend = getenv(TERMUX_EGL_TYPE_ENV);
        if (priority_backend != NULL) {
            std::string pb{priority_backend};
            if (pb == "NATIVE") {
                defaultDisplayType = DisplayType::NATIVE;
            } else if (pb == "WAYLAND") {
                defaultDisplayType = DisplayType::WAYLAND;
            } else if (pb == "X11") {
                defaultDisplayType = DisplayType::X11;
            } else {
                fprintf(stderr, "ERROR: Invalid value for 'TERMUX_EGL_DEFAULT' environment variable: %s\n", priority_backend);
                fflush(stderr);
                return EGL_FALSE;
            }
        }
        
        
        
        return EGL_TRUE;
    }
    
    
    /**
     * @brief Deinitialize the library.
     * Terminate the native EGL display if possible, unload libEGL.so and libandroid.so.
     * 
     */
    __attribute__((destructor)) static void egl_wrapper_deinit() {
        if (nativeEGLLibrary != NULL) {
            if (real_eglTerminate != NULL && nativeDisplay != EGL_NO_DISPLAY) {
                real_eglTerminate(nativeDisplay);
            }
            
            
            dlclose(nativeEGLLibrary);
            nativeEGLLibrary = NULL;
        }
        // TODO go through each display list and free the entries
        libAndroidHelper::deinit(libandroid);
    }
    
    
    
    
    
    #ifdef X11_PLATFORM
    /**
     * @brief Get the Xlib Display, creating a new one if necessary.
     * EGL specifies that the same parameters must return the same display, so keep the displays in a map.
     */
    static X11Display* getXlibDisplay(void* nativeDisplay, const EGLAttrib *attrib_list) {
        std::unique_lock<std::mutex> lock{displayListLock};
        if (xlibDisplayList.contains(nativeDisplay)) {
            return xlibDisplayList.at(nativeDisplay);
        } else {
            fprintf(stderr, "Creating xlib display\n");
            fflush(stderr);
            X11Display* d = new X11Display((::Display*) nativeDisplay, attrib_list);
            xlibDisplayList[nativeDisplay] = d;
            return d;
        }
    }
    
    /**
     * @brief Get the XCB Display, creating a new one if necessary.
     * EGL specifies that the same parameters must return the same display, so keep the displays in a map.
     */
    static X11Display* getXCBDisplay(void* nativeDisplay, const EGLAttrib *attrib_list) {
        std::unique_lock<std::mutex> lock{displayListLock};
        if (xcbDisplayList.contains(nativeDisplay)) {
            return xcbDisplayList.at(nativeDisplay);
        } else {
            X11Display* d = new X11Display((xcb_connection_t*) nativeDisplay, attrib_list);
            xcbDisplayList[nativeDisplay] = d;
            return d;
        }
    }
    #endif
    
    EGLDisplay getPlatformDisplay(EGLenum platform, void* nativeDisplay, const EGLAttrib* attrib_list) {
        try {
        if (platform == EGL_NONE) {
            // without the platform defined it's impossible to know what nativeDisplay is
            if (nativeDisplay != EGL_DEFAULT_DISPLAY) {
                return EGL_NO_DISPLAY;
            }
            switch (defaultDisplayType) {
                case DisplayType::NATIVE:
                    if (nativeDisplay == EGL_DEFAULT_DISPLAY)
                        return &androidDisplay;
                #ifdef X11_PLATFORM
                case DisplayType::X11:
                    return static_cast<egl_wrapper::EGLDisplayBackend*>(getXlibDisplay(nativeDisplay, attrib_list));
                #endif
                #ifdef WAYLAND_PLATFORM
                case DisplayType::WAYLAND:
                    
                #endif
                default:
                    return EGL_NO_DISPLAY;
            }
        }
        if (platform == EGL_PLATFORM_ANDROID_KHR && nativeDisplay == EGL_DEFAULT_DISPLAY) {
            return &androidDisplay;
        }
        #ifdef X11_PLATFORM
        if (platform == EGL_PLATFORM_X11_KHR) {
            return static_cast<egl_wrapper::EGLDisplayBackend*>(getXlibDisplay(nativeDisplay, attrib_list));
        }
        if (platform == EGL_PLATFORM_XCB_EXT) {
            return static_cast<egl_wrapper::EGLDisplayBackend*>(getXCBDisplay(nativeDisplay, attrib_list));
        }
        #endif
        #ifdef WAYLAND_PLATFORM
            if (platform == EGL_PLATFORM_WAYLAND_KHR) {
                
            }
        #endif
        } CATCH_EGL_EXCEPTIONS
        catch (const Exceptions::NoDisplayException&) {}
        return EGL_NO_DISPLAY;
    }
    
    EGLBoolean getSupportsAPI(EGLenum api) {
        if (api == EGL_OPENGL_ES_API) return true;
        return false;
    }
    
    void* getProcAddress(const char* procName) {
        GENERATE_DISPATCH_IF(eglChooseConfig)
        GENERATE_DISPATCH_IF(eglCopyBuffers)
        GENERATE_DISPATCH_IF(eglCreateContext)
        GENERATE_DISPATCH_IF(eglCreatePbufferSurface)
        GENERATE_DISPATCH_IF(eglCreatePixmapSurface)
        GENERATE_DISPATCH_IF(eglCreateWindowSurface)
        GENERATE_DISPATCH_IF(eglDestroyContext)
        GENERATE_DISPATCH_IF(eglDestroySurface)
        GENERATE_DISPATCH_IF(eglGetConfigAttrib)
        GENERATE_DISPATCH_IF(eglGetConfigs)
        GENERATE_DISPATCH_IF(eglInitialize)
        GENERATE_DISPATCH_IF(eglMakeCurrent)
        GENERATE_DISPATCH_IF(eglQueryContext)
        GENERATE_DISPATCH_IF(eglQueryString)
        GENERATE_DISPATCH_IF(eglQuerySurface)
        GENERATE_DISPATCH_IF(eglSwapBuffers)
        GENERATE_DISPATCH_IF(eglTerminate)
        GENERATE_DISPATCH_IF(eglWaitGL)
        GENERATE_DISPATCH_IF(eglWaitNative)
        GENERATE_DISPATCH_IF(eglBindTexImage)
        GENERATE_DISPATCH_IF(eglReleaseTexImage)
        GENERATE_DISPATCH_IF(eglSurfaceAttrib)
        GENERATE_DISPATCH_IF(eglSwapInterval)
        GENERATE_DISPATCH_IF(eglBindAPI)
        GENERATE_DISPATCH_IF(eglCreatePbufferFromClientBuffer)
        GENERATE_DISPATCH_IF(eglReleaseThread)
        GENERATE_DISPATCH_IF(eglWaitClient)
        GENERATE_DISPATCH_IF(eglCreateSync)
        GENERATE_DISPATCH_IF(eglDestroySync)
        GENERATE_DISPATCH_IF(eglClientWaitSync)
        GENERATE_DISPATCH_IF(eglGetSyncAttrib)
        GENERATE_DISPATCH_IF(eglCreateImage)
        GENERATE_DISPATCH_IF(eglDestroyImage)
        GENERATE_DISPATCH_IF(eglCreatePlatformWindowSurface)
        GENERATE_DISPATCH_IF(eglCreatePlatformPixmapSurface)
        GENERATE_DISPATCH_IF(eglWaitSync)
        
        return NULL;
    }
    
    void* getDispatchAddress(const char* procName) {
        return NULL;
    }
    
    void setDispatchIndex(const char* procName, int index) {
        // could throw with OOM, but not much else we could do here, will abort the program either way.
        std::unique_lock<std::mutex> l{dispatchLock};
        dispatchIndexMap[std::string(procName)] = index;
    }
    
    const char* getVendorString(int name) {
        if (name != __EGL_VENDOR_STRING_PLATFORM_EXTENSIONS) return NULL;
        return PLATFORMS_STRING;
    }



}





extern "C" EGLBoolean __egl_Main(uint32_t version, const __EGLapiExports *exports, __EGLvendorInfo *vendor, __EGLapiImports *imports) {
    fprintf(stdout, "eglMain\n");
    fflush(stdout);
    if (EGL_VENDOR_ABI_GET_MAJOR_VERSION(version) != 0 || EGL_VENDOR_ABI_GET_MINOR_VERSION(version) < 1) {
        fprintf(stderr, "ERROR: EGL vendor ABI mismatch\n");
        fflush(stderr);
        return EGL_FALSE;
    }
    egl_wrapper::glvnd = exports;
    egl_wrapper::thisVendor = vendor;
    EGLBoolean status = egl_wrapper::egl_wrapper_init();
    if (status == EGL_FALSE) return EGL_FALSE;
    imports->getPlatformDisplay = egl_wrapper::getPlatformDisplay;
    imports->getSupportsAPI = egl_wrapper::getSupportsAPI;
    imports->getProcAddress = egl_wrapper::getProcAddress;
    imports->getDispatchAddress = egl_wrapper::getDispatchAddress;
    imports->setDispatchIndex = egl_wrapper::setDispatchIndex;
    imports->getVendorString = egl_wrapper::getVendorString;
    
    return EGL_TRUE;
}



