#include "egl.hpp"


#include <cstdio>
#include <cstdlib>
#include <cstring>


#include <string>
#include <vector>
#include <thread>

#include <sys/socket.h>
#include <unistd.h>
#include <sys/mman.h>


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
    void* nativeGLES2Library = nullptr;
    void* nativeEGLLibrary = nullptr;
    libAndroidHelper::LibAndroid libandroid;
    const __EGLapiExports* glvnd = nullptr;
    __EGLvendorInfo *thisVendor = nullptr;
    bool hwbufferRenderingAvailable = true;
    bool hwbufferDMABUFAvailable = false;
    bool hwbufferBGRAvailable = false;
    DisplayType defaultDisplayType = DisplayType::DEFAULT_DISPLAY_PLAYFORM;
    EGLDisplay nativeDisplay = EGL_NO_DISPLAY;
    X11Mode x11Mode = X11Mode::BLOCK;
    
    
    int eglCreateImageKHRIndex = -1;
    int eglDestroyImageKHRIndex = -1;
    
    PFNEGLCHOOSECONFIGPROC real_eglChooseConfig = nullptr;
    PFNEGLCOPYBUFFERSPROC real_eglCopyBuffers = nullptr;
    PFNEGLCREATECONTEXTPROC real_eglCreateContext = nullptr;
    PFNEGLCREATEPBUFFERSURFACEPROC real_eglCreatePbufferSurface = nullptr;
    PFNEGLCREATEPIXMAPSURFACEPROC real_eglCreatePixmapSurface = nullptr;
    PFNEGLCREATEWINDOWSURFACEPROC real_eglCreateWindowSurface = nullptr;
    PFNEGLDESTROYCONTEXTPROC real_eglDestroyContext = nullptr;
    PFNEGLDESTROYSURFACEPROC real_eglDestroySurface = nullptr;
    PFNEGLGETCONFIGATTRIBPROC real_eglGetConfigAttrib = nullptr;
    PFNEGLGETCONFIGSPROC real_eglGetConfigs = nullptr;
    PFNEGLGETCURRENTDISPLAYPROC real_eglGetCurrentDisplay = nullptr;
    PFNEGLGETCURRENTSURFACEPROC real_eglGetCurrentSurface = nullptr;
    PFNEGLGETDISPLAYPROC real_eglGetDisplay = nullptr;
    PFNEGLGETERRORPROC real_eglGetError = nullptr;
    PFNEGLGETPROCADDRESSPROC real_eglGetProcAddress = nullptr;
    PFNEGLINITIALIZEPROC real_eglInitialize = nullptr;
    PFNEGLMAKECURRENTPROC real_eglMakeCurrent = nullptr;
    PFNEGLQUERYCONTEXTPROC real_eglQueryContext = nullptr;
    PFNEGLQUERYSTRINGPROC real_eglQueryString = nullptr;
    PFNEGLQUERYSURFACEPROC real_eglQuerySurface = nullptr;
    PFNEGLSWAPBUFFERSPROC real_eglSwapBuffers = nullptr;
    PFNEGLTERMINATEPROC real_eglTerminate = nullptr;
    PFNEGLWAITGLPROC real_eglWaitGL = nullptr;
    PFNEGLWAITNATIVEPROC real_eglWaitNative = nullptr;
    PFNEGLBINDTEXIMAGEPROC real_eglBindTexImage = nullptr;
    PFNEGLRELEASETEXIMAGEPROC real_eglReleaseTexImage = nullptr;
    PFNEGLSURFACEATTRIBPROC real_eglSurfaceAttrib = nullptr;
    PFNEGLSWAPINTERVALPROC real_eglSwapInterval = nullptr;
    PFNEGLBINDAPIPROC real_eglBindAPI = nullptr;
    PFNEGLQUERYAPIPROC real_eglQueryAPI = nullptr;
    PFNEGLCREATEPBUFFERFROMCLIENTBUFFERPROC real_eglCreatePbufferFromClientBuffer = nullptr;
    PFNEGLRELEASETHREADPROC real_eglReleaseThread = nullptr;
    PFNEGLWAITCLIENTPROC real_eglWaitClient = nullptr;
    PFNEGLGETCURRENTCONTEXTPROC real_eglGetCurrentContext = nullptr;
    PFNEGLCREATESYNCPROC real_eglCreateSync = nullptr;
    PFNEGLDESTROYSYNCPROC real_eglDestroySync = nullptr;
    PFNEGLCLIENTWAITSYNCPROC real_eglClientWaitSync = nullptr;
    PFNEGLGETSYNCATTRIBPROC real_eglGetSyncAttrib = nullptr;
    PFNEGLCREATEIMAGEPROC real_eglCreateImage = nullptr;
    PFNEGLDESTROYIMAGEPROC real_eglDestroyImage = nullptr;
    PFNEGLGETPLATFORMDISPLAYPROC real_eglGetPlatformDisplay = nullptr;
    PFNEGLCREATEPLATFORMWINDOWSURFACEPROC real_eglCreatePlatformWindowSurface = nullptr;
    PFNEGLCREATEPLATFORMPIXMAPSURFACEPROC real_eglCreatePlatformPixmapSurface = nullptr;
    PFNEGLWAITSYNCPROC real_eglWaitSync = nullptr;
    
    
    EGLImageKHR (*real_eglCreateImageKHR)(EGLDisplay dpy, EGLContext ctx, EGLenum target, EGLClientBuffer buffer, const EGLint *attrib_list) = nullptr;
    EGLBoolean (*real_eglDestroyImageKHR)(EGLDisplay dpy, EGLImageKHR image) = nullptr;
    
    
    EGLClientBuffer (*real_eglCreateNativeClientBufferANDROID)(const EGLint* attrib_list) = nullptr;
    
    
    EGLClientBuffer (*real_eglGetNativeClientBufferANDROID)(const struct AHardwareBuffer* buffer) = nullptr;

#define FUNC(name) __typeof__(name)* real_ ## name = nullptr;
    GLES2_0_FUNCS(FUNC)
    GLES2_0_FUNCS_OPT(FUNC)
#undef FUNC

    thread_local EGLint lastError = EGL_SUCCESS;
    
    /**
     * @brief Looks up an GLES function and errors if not found.
     * 
     */
    #define GLFUNC(fname) real_ ## fname = (decltype(real_ ## fname)) dlsym(nativeGLES2Library, #fname); \
    if (real_ ## fname == nullptr) { \
        fprintf(stderr, "ERROR: Could not find GLES function '" #fname "'"); \
        fflush(stderr); \
        return EGL_FALSE; \
    }
    
    #define GLFUNC_OPT(fname) real_ ## fname = (decltype(real_ ## fname)) dlsym(nativeGLES2Library, #fname);
    
    /**
     * @brief Looks up an EGL function and errors if not found.
     * 
     */
    #define EGLFUNC(fname) real_ ## fname = (decltype(real_ ## fname)) dlsym(nativeEGLLibrary, #fname); \
    if (real_ ## fname == nullptr) { \
        fprintf(stderr, "ERROR: Could not find EGL function '" #fname "'"); \
        fflush(stderr); \
        return EGL_FALSE; \
    }
    
    /**
     * @brief Looks up an EGL function and sets to nullptr if not found.
     * 
     */
    #define EGLFUNC_OPT(fname) real_ ## fname = (decltype(real_ ## fname)) dlsym(nativeEGLLibrary, #fname);
    
    
    /**
     * @brief Perform library initialisation, should be called by eglMain at libglvnd initialisation.
     * 
     * @return EGLBoolean Whether the initialisation was successful.
     */
    EGLBoolean egl_wrapper_init()  {
        // search for the native libEGL.so
        nativeEGLLibrary = dlopen("/system/lib64/libEGL.so", RTLD_LAZY | RTLD_LOCAL);
        nativeGLES2Library = dlopen("/system/lib64/libGLESv2.so", RTLD_LAZY | RTLD_LOCAL);
        if (nativeEGLLibrary == nullptr) {
            nativeEGLLibrary = dlopen("/system/lib/libEGL.so", RTLD_LAZY | RTLD_LOCAL);
            if (nativeGLES2Library != nullptr) dlclose(nativeGLES2Library);
            nativeGLES2Library = dlopen("/system/lib/libGLESv2.so", RTLD_LAZY | RTLD_LOCAL);
        }
        if (nativeEGLLibrary == nullptr || nativeGLES2Library == nullptr) {
            fprintf(stderr, "ERROR: Could not find Android libEGL.so or libGLESv2.so in /system/lib64 or /system/lib\n");
            fflush(stderr);
            return EGL_FALSE;
        }

        // get all EGL functions
        EGLFUNC(eglChooseConfig)
        EGLFUNC(eglCopyBuffers)
        EGLFUNC(eglCreateContext)
        EGLFUNC(eglCreatePbufferSurface)
        EGLFUNC(eglCreatePixmapSurface)
        EGLFUNC(eglCreateWindowSurface)
        EGLFUNC(eglDestroyContext)
        EGLFUNC(eglDestroySurface)
        EGLFUNC(eglGetConfigAttrib)
        EGLFUNC(eglGetConfigs)
        EGLFUNC(eglGetCurrentDisplay)
        EGLFUNC(eglGetCurrentSurface)
        EGLFUNC(eglGetDisplay)
        EGLFUNC(eglGetError)
        EGLFUNC(eglGetProcAddress)
        EGLFUNC(eglInitialize)
        EGLFUNC(eglMakeCurrent)
        EGLFUNC(eglQueryContext)
        EGLFUNC(eglQueryString)
        EGLFUNC(eglQuerySurface)
        EGLFUNC(eglSwapBuffers)
        EGLFUNC(eglTerminate)
        EGLFUNC(eglWaitGL)
        EGLFUNC(eglWaitNative)
        
        EGLFUNC(eglBindTexImage)
        EGLFUNC(eglReleaseTexImage)
        EGLFUNC(eglSurfaceAttrib)
        EGLFUNC(eglSwapInterval)
        
        EGLFUNC(eglBindAPI)
        EGLFUNC(eglQueryAPI)
        EGLFUNC(eglCreatePbufferFromClientBuffer)
        EGLFUNC(eglReleaseThread)
        EGLFUNC(eglWaitClient)
        
        EGLFUNC(eglGetCurrentContext)
        
        EGLFUNC_OPT(eglCreateSync)
        EGLFUNC_OPT(eglDestroySync)
        EGLFUNC_OPT(eglClientWaitSync)
        EGLFUNC_OPT(eglGetSyncAttrib)
        EGLFUNC_OPT(eglCreateImage)
        EGLFUNC_OPT(eglDestroyImage)
        EGLFUNC_OPT(eglGetPlatformDisplay)
        EGLFUNC_OPT(eglCreatePlatformWindowSurface)
        EGLFUNC_OPT(eglCreatePlatformPixmapSurface)
        EGLFUNC_OPT(eglWaitSync)
        
        EGLFUNC_OPT(eglCreateImageKHR)
        EGLFUNC_OPT(eglDestroyImageKHR)
        
        EGLFUNC_OPT(eglCreateNativeClientBufferANDROID)
        
        EGLFUNC_OPT(eglGetNativeClientBufferANDROID)

        GLES2_0_FUNCS(GLFUNC)
        GLES2_0_FUNCS_OPT(GLFUNC_OPT)
        
        // check needed EGL functions
        if ( // EGL 1.0-1.4 functions
            real_eglChooseConfig == nullptr ||
            real_eglCopyBuffers == nullptr ||
            real_eglCreateContext == nullptr ||
            real_eglCreatePbufferSurface == nullptr ||
            real_eglCreatePixmapSurface == nullptr ||
            real_eglCreateWindowSurface == nullptr ||
            real_eglDestroyContext == nullptr ||
            real_eglDestroySurface == nullptr ||
            real_eglGetConfigAttrib == nullptr ||
            real_eglGetConfigs == nullptr ||
            real_eglGetCurrentDisplay == nullptr ||
            real_eglGetCurrentSurface == nullptr ||
            real_eglGetDisplay == nullptr ||
            real_eglGetError == nullptr ||
            real_eglGetProcAddress == nullptr ||
            real_eglInitialize == nullptr ||
            real_eglMakeCurrent == nullptr ||
            real_eglQueryContext == nullptr ||
            real_eglQueryString == nullptr ||
            real_eglQuerySurface == nullptr ||
            real_eglSwapBuffers == nullptr ||
            real_eglTerminate == nullptr ||
            real_eglWaitGL == nullptr ||
            real_eglWaitNative == nullptr ||
            real_eglBindTexImage == nullptr ||
            real_eglReleaseTexImage == nullptr ||
            real_eglSurfaceAttrib == nullptr ||
            real_eglSwapInterval == nullptr ||
            real_eglBindAPI == nullptr ||
            real_eglQueryAPI == nullptr ||
            real_eglCreatePbufferFromClientBuffer == nullptr ||
            real_eglReleaseThread == nullptr ||
            real_eglWaitClient == nullptr ||
            real_eglGetCurrentContext == nullptr
            ) {
            fprintf(stderr, "ERROR: Core EGL functions missing from Android EGL\n");
            fflush(stderr);
            return EGL_FALSE;
        }
        
        if ( // EGL 1.5 functions
            real_eglCreateSync == nullptr ||
            real_eglDestroySync == nullptr ||
            real_eglClientWaitSync == nullptr ||
            real_eglGetSyncAttrib == nullptr ||
            real_eglCreateImage == nullptr ||
            real_eglDestroyImage == nullptr ||
            real_eglGetPlatformDisplay == nullptr ||
            real_eglCreatePlatformWindowSurface == nullptr ||
            real_eglCreatePlatformPixmapSurface == nullptr ||
            real_eglWaitSync == nullptr
            ) {
            androidDisplay.egl15 = false;
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
        if (minor < 5) {
            androidDisplay.egl15 = false;
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
            if (exts_raw == nullptr) {
                // fprintf(stderr, "ERROR: EGL client extensions not supported, EGL emulation not possible\n");
                // fflush(stderr);
                // return EGL_FALSE;
                hwbufferRenderingAvailable = false;
                break;
            }
            std::string exts(exts_raw);
            if (exts.find("EGL_KHR_image_base") == std::string::npos) {
                // fprintf(stderr, "ERROR: EGL_KHR_image_base extension not supported, EGL emulation not possible\n");
                fflush(stderr);
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
                // fprintf(stderr, "ERROR: no EGLConfig\n");
                // fflush(stderr);
                // return EGL_FALSE;
                hwbufferRenderingAvailable = false;
                break;
            }
            EGLint context_attributes[] = { EGL_CONTEXT_MAJOR_VERSION, 2, EGL_CONTEXT_MINOR_VERSION, 0, EGL_NONE };
            EGLContext ctx = real_eglCreateContext(nativeDisplay, conf, EGL_NO_CONTEXT, context_attributes);
            if (ctx == EGL_NO_CONTEXT) {
                // fprintf(stderr, "ERROR: no EGLContext\n");
                // fflush(stderr);
                // return EGL_FALSE;
                hwbufferRenderingAvailable = false;
                break;
            }
            EGLint pbAttrib[] = { EGL_WIDTH, 1, EGL_HEIGHT, 1, EGL_NONE };
            EGLSurface pbs = EGL_NO_SURFACE;
            if ((pbs = real_eglCreatePbufferSurface(nativeDisplay, conf, pbAttrib)) == EGL_NO_SURFACE) {
                real_eglDestroyContext(nativeDisplay, ctx);
                // fprintf(stderr, "ERROR: no PBuffer\n");
                // fflush(stderr);
                break;
            }
            
            if (real_eglMakeCurrent(nativeDisplay, pbs, pbs, ctx) == EGL_FALSE) {
                // fprintf(stderr, "ERROR: make current: 0x%x\n", real_eglGetError());
                // fflush(stderr);
                // return EGL_FALSE;
                hwbufferRenderingAvailable = false;
                real_eglDestroyContext(nativeDisplay, ctx);
                real_eglDestroySurface(nativeDisplay, pbs);
                break;
            }
            // check needed GLES extensions: GL_OES_EGL_image and GL_OES_framebuffer_object
            
            auto real_glGetString = (PFNGLGETSTRINGPROC) real_eglGetProcAddress("glGetString");
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
            // not needed for GLES2
            // if (gl_exts.find("GL_OES_framebuffer_object") == std::string::npos) {
            //     fprintf(stderr, "ERROR: GL_OES_framebuffer_object extension not supported, EGL emulation not possible\n");
            //     fflush(stderr);
            //     hwbufferRenderingAvailable = false;
            //     real_eglDestroyContext(nativeDisplay, ctx);
            //     real_eglDestroySurface(nativeDisplay, pbs);
            //     break;
            // }
            real_eglMakeCurrent(nativeDisplay, EGL_NO_SURFACE, EGL_NO_SURFACE, EGL_NO_CONTEXT);
            real_eglDestroyContext(nativeDisplay, ctx);
            real_eglDestroySurface(nativeDisplay, pbs);
            break;
        }
        
        if (getenv(TERMUX_EGL_DISABLE_HWBUF_ENV) != NULL) {
            hwbufferRenderingAvailable = 0;
        }
        
        //fprintf(stderr, "hb: %d\n", (int) hwbufferRenderingAvailable);
        
        if (hwbufferRenderingAvailable) {
            SmartHardwareBuffer shb{allocHB(AHARDWAREBUFFER_FORMAT_R8G8B8A8_UNORM, 2, 2, HBUsage)};
            if (shb != nullptr) {
                int fd = HBDMABUF(shb);
                if (fd != -1) {
                    void* adr = mmap(nullptr, 4 * 4, PROT_READ|PROT_WRITE, MAP_SHARED, fd, 0);
                    if (adr != MAP_FAILED) {
                        munmap(adr, 4 * 4);
                        hwbufferDMABUFAvailable = true;
                    } else {
                        //fprintf(stderr, "could not map\n");
                    }
                    close(fd);
                } else {
                    //fprintf(stderr, "invalid fd\n");
                }
            } else {
                //fprintf(stderr, "could not create hardware buffer\n");
            }
            SmartHardwareBuffer shb2{allocHB(HAL_PIXEL_FORMAT_BGRA_8888, 2, 2, HBUsage)};
            if (shb2 != nullptr) {
                hwbufferBGRAvailable = true;
            }
        }
        //fprintf(stderr, "DMABUF: %d\n", (int) hwbufferDMABUFAvailable);
        //fflush(stderr);
        
        //fprintf(stderr, "HAL_PIXEL_FORMAT_BGRA_8888: %d\n", (int) hwbufferBGRAvailable);
        
        // get the default platform backend from the env variable, if set
        const char* priority_backend = getenv(TERMUX_EGL_TYPE_ENV);
        if (priority_backend != nullptr) {
            std::string pb{priority_backend};
            if (pb == "NATIVE") {
                defaultDisplayType = DisplayType::NATIVE;
            } else
            #ifdef WAYLAND_PLATFORM
                if (pb == "WAYLAND") {
                    defaultDisplayType = DisplayType::WAYLAND;
                } else
            #endif
            #ifdef X11_PLATFORM
                if (pb == "X11") {
                    defaultDisplayType = DisplayType::X11;
                } else
            #endif
            {
                fprintf(stderr, "ERROR: Invalid value for 'TERMUX_EGL_DEFAULT' environment variable: %s\n", priority_backend);
                fflush(stderr);
                return EGL_FALSE;
            }
        }
        
        #ifdef X11_PLATFORM
            const char* envX11Mode = getenv(TERMUX_EGL_X11_MODE_ENV);
            if (envX11Mode != nullptr) {
                std::string mode{envX11Mode};
                if (mode == "BLOCK") {
                    x11Mode = X11Mode::BLOCK;
                } else if (mode == "IDLE") {
                    x11Mode = X11Mode::IDLE;
                }  else {
                    fprintf(stderr, "ERROR: Invalid value for 'TERMUX_EGL_X11_MODE' environment variable: %s\n", envX11Mode);
                    fflush(stderr);
                    return EGL_FALSE;
                }
            }
        #endif
        
        return EGL_TRUE;
    }
    
    
    /**
     * @brief Deinitialize the library.
     * Terminate the native EGL display if possible, unload libEGL.so and libandroid.so.
     * 
     */
    __attribute__((destructor)) static void egl_wrapper_deinit() {
        if (nativeEGLLibrary != nullptr) {
            if (real_eglTerminate != nullptr && nativeDisplay != EGL_NO_DISPLAY) {
                real_eglTerminate(nativeDisplay);
            }
            
            dlclose(nativeEGLLibrary);
            nativeEGLLibrary = nullptr;
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
            //fprintf(stderr, "Creating xlib display\n");
            //fflush(stderr);
            auto d = new X11Display((::Display*) nativeDisplay, attrib_list);
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
        //fprintf(stderr, "getPlatformDisplay\n");
        //fflush(stderr);
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
        if (procName == nullptr) return nullptr;
        //fprintf(stdout, "getProcAddress: %s\n", procName);
        //fflush(stdout);
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
        GENERATE_DISPATCH_IF(eglGetError)
        GENERATE_DISPATCH_IF(eglCreateSync)
        GENERATE_DISPATCH_IF(eglDestroySync)
        GENERATE_DISPATCH_IF(eglClientWaitSync)
        GENERATE_DISPATCH_IF(eglGetSyncAttrib)
        GENERATE_DISPATCH_IF(eglCreateImage)
        GENERATE_DISPATCH_IF(eglDestroyImage)
        GENERATE_DISPATCH_IF(eglCreatePlatformWindowSurface)
        GENERATE_DISPATCH_IF(eglCreatePlatformPixmapSurface)
        GENERATE_DISPATCH_IF(eglWaitSync)

        GLES2_0_FUNCS(GENERATE_DISPATCH_IF)
        return nullptr;
    }
    
    void* getDispatchAddress(const char* procName) {
        if (procName == nullptr) return nullptr;
        GENERATE_DISPATCH_IF(eglCreateImageKHR)
        GENERATE_DISPATCH_IF(eglDestroyImageKHR)
        GENERATE_DISPATCH_IF(eglCreateNativeClientBufferANDROID)
        GENERATE_DISPATCH_IF(eglGetNativeClientBufferANDROID)
        return nullptr;
    }
    
    #define DISPATCH_EXT_INDEX(func) if (strcmp(#func, procName) == 0) func ## Index = index;
    
    void setDispatchIndex(const char* procName, int index) {
        if (procName == nullptr) return;
        DISPATCH_EXT_INDEX(eglCreateImageKHR)
        DISPATCH_EXT_INDEX(eglDestroyImageKHR)
    }
    
    const char* getVendorString(int name) {
        if (name != __EGL_VENDOR_STRING_PLATFORM_EXTENSIONS) return nullptr;
        return PLATFORMS_STRING;
    }
    
    
    SmartEGLContext::~SmartEGLContext() {
        if (c != EGL_NO_CONTEXT) {
            real_eglDestroyContext(nativeDisplay, c);
        }
    }
    
    SmartEGLContext& SmartEGLContext::operator=(SmartEGLContext&& o) {
        if (this != &o) {
            if (c != EGL_NO_CONTEXT) {
                real_eglDestroyContext(nativeDisplay, c);
            }
            c = o.c;
            o.c = EGL_NO_CONTEXT;
        }
        return *this;
    }
    


    int HBDMABUF(AHardwareBuffer* hb) {
        //fprintf(stderr, "test\n");
        int socks[2];
        if (socketpair(AF_UNIX, SOCK_STREAM, 0, socks) == 0) {
            try {
                std::thread sender{[&] {
                    libandroid.AHardwareBuffer_sendHandleToUnixSocket(hb, socks[1]);
                    close(socks[1]);
                }};
                struct msghdr msg{};
                msg.msg_name = nullptr;
                msg.msg_namelen = 0;
                struct iovec iov{};
                char iobuf[100];
                iov.iov_base = iobuf;
                iov.iov_len = sizeof(iobuf);
                msg.msg_iov = &iov;
                msg.msg_iovlen = 1;
                constexpr int CONTROLLEN = CMSG_SPACE(sizeof(int)*50);
                union {
                    cmsghdr _; // for alignment
                    char controlBuffer[CONTROLLEN];
                } controlBufferUnion;
                memset(&controlBufferUnion, 0, CONTROLLEN);
                msg.msg_control = &controlBufferUnion;
                msg.msg_controllen = sizeof(controlBufferUnion);
                const int fdindex = 0;
                int recfd = -1;
                errno = 0;
                while (recvmsg(socks[0], &msg, 0) > 0) {
                    for (struct cmsghdr* cmsg = CMSG_FIRSTHDR(&msg); cmsg != nullptr; cmsg = CMSG_NXTHDR(&msg, cmsg)) {
                        if (cmsg->cmsg_level == SOL_SOCKET && cmsg->cmsg_type == SCM_RIGHTS && recfd == -1) {
                            //fprintf(stderr, "len: %ul\n", cmsg->cmsg_len/4);
                            memcpy(&recfd, CMSG_DATA(cmsg) + sizeof(int) * fdindex, sizeof(recfd));
                        }
                    }
                }
                //fprintf(stderr, "errno: %d\n", errno);
                //fflush(stderr);
                close(socks[0]);
                sender.join();
                return recfd;
            } catch (...) {
                close(socks[0]);
            }
        }
        //fprintf(stderr, "errno: %d\n", errno);
        //fflush(stderr);
        return -1;
    }
    
    AHardwareBuffer* allocHB(AHardwareBuffer_Format format, uint32_t width, uint32_t height, uint64_t usage) {
        AHardwareBuffer_Desc desc{};
        desc.format = format;
        desc.width = width;
        desc.height = height;
        desc.usage = usage;
        desc.layers = 1;
        AHardwareBuffer* out;
        if (libandroid.AHardwareBuffer_allocate(&desc, &out) == 0)
            return out;
        else
            return nullptr;
    }

}

extern "C" [[maybe_unused]]
EGLBoolean __egl_Main(uint32_t version, const __EGLapiExports *exports, __EGLvendorInfo *vendor, __EGLapiImports *imports) {
    //fprintf(stdout, "eglMain\n");
    //fflush(stdout);
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



