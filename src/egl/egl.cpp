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
    void* nativeGLES2Library = NULL;
    void* nativeEGLLibrary = NULL;
    libAndroidHelper::LibAndroid libandroid;
    const __EGLapiExports* glvnd = NULL;
    __EGLvendorInfo *thisVendor = NULL;
    bool hwbufferRenderingAvailable = true;
    DisplayType defaultDisplayType = DisplayType::DEFAULT_DISPLAY_PLAYFORM;
    EGLDisplay nativeDisplay = EGL_NO_DISPLAY;
    
    
    int eglCreateImageKHRIndex = -1;
    int eglDestroyImageKHRIndex = -1;
    
    
    
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
    
    
    EGLImageKHR (*real_eglCreateImageKHR)(EGLDisplay dpy, EGLContext ctx, EGLenum target, EGLClientBuffer buffer, const EGLint *attrib_list) = NULL;
    EGLBoolean (*real_eglDestroyImageKHR)(EGLDisplay dpy, EGLImageKHR image) = NULL;
    
    
    EGLClientBuffer (*real_eglCreateNativeClientBufferANDROID)(const EGLint* attrib_list) = NULL;
    
    
    EGLClientBuffer (*real_eglGetNativeClientBufferANDROID)(const struct AHardwareBuffer* buffer) = NULL;
    
    
    void (*real_glActiveTexture)(GLenum texture) = NULL;
    void (*real_glAttachShader)(GLuint program, GLuint shader) = NULL;
    void (*real_glBindAttribLocation)(GLuint program, GLuint index, const GLchar* name) = NULL;
    void (*real_glBindBuffer)(GLenum target, GLuint buffer) = NULL;
    void (*real_glBindFramebuffer)(GLenum target, GLuint framebuffer) = NULL;
    void (*real_glBindRenderbuffer)(GLenum target, GLuint renderbuffer) = NULL;
    void (*real_glBindTexture)(GLenum target, GLuint texture) = NULL;
    void (*real_glBlendColor)(GLfloat red, GLfloat green, GLfloat blue, GLfloat alpha) = NULL;
    void (*real_glBlendEquation)(GLenum mode) = NULL;
    void (*real_glBlendEquationSeparate)(GLenum modeRGB, GLenum modeAlpha) = NULL;
    void (*real_glBlendFunc)(GLenum sfactor, GLenum dfactor) = NULL;
    void (*real_glBlendFuncSeparate)(GLenum sfactorRGB, GLenum dfactorRGB, GLenum sfactorAlpha, GLenum dfactorAlpha) = NULL;
    void (*real_glBufferData)(GLenum target, GLsizeiptr size, const void* data, GLenum usage) = NULL;
    void (*real_glBufferSubData)(GLenum target, GLintptr offset, GLsizeiptr size, const void* data) = NULL;
    GLenum (*real_glCheckFramebufferStatus)(GLenum target) = NULL;
    void (*real_glClear)(GLbitfield mask) = NULL;
    void (*real_glClearColor)(GLfloat red, GLfloat green, GLfloat blue, GLfloat alpha) = NULL;
    void (*real_glClearDepthf)(GLfloat d) = NULL;
    void (*real_glClearStencil)(GLint s) = NULL;
    void (*real_glColorMask)(GLboolean red, GLboolean green, GLboolean blue, GLboolean alpha) = NULL;
    void (*real_glCompileShader)(GLuint shader) = NULL;
    void (*real_glCompressedTexImage2D)(GLenum target, GLint level, GLenum internalformat, GLsizei width, GLsizei height, GLint border, GLsizei imageSize, const void* data) = NULL;
    void (*real_glCompressedTexSubImage2D)(GLenum target, GLint level, GLint xoffset, GLint yoffset, GLsizei width, GLsizei height, GLenum format, GLsizei imageSize, const void* data) = NULL;
    void (*real_glCopyTexImage2D)(GLenum target, GLint level, GLenum internalformat, GLint x, GLint y, GLsizei width, GLsizei height, GLint border) = NULL;
    void (*real_glCopyTexSubImage2D)(GLenum target, GLint level, GLint xoffset, GLint yoffset, GLint x, GLint y, GLsizei width, GLsizei height) = NULL;
    GLuint (*real_glCreateProgram)(void) = NULL;
    GLuint (*real_glCreateShader)(GLenum type) = NULL;
    void (*real_glCullFace)(GLenum mode) = NULL;
    void (*real_glDeleteBuffers)(GLsizei n, const GLuint* buffers) = NULL;
    void (*real_glDeleteFramebuffers)(GLsizei n, const GLuint* framebuffers) = NULL;
    void (*real_glDeleteProgram)(GLuint program) = NULL;
    void (*real_glDeleteRenderbuffers)(GLsizei n, const GLuint* renderbuffers) = NULL;
    void (*real_glDeleteShader)(GLuint shader) = NULL;
    void (*real_glDeleteTextures)(GLsizei n, const GLuint* textures) = NULL;
    void (*real_glDepthFunc)(GLenum func) = NULL;
    void (*real_glDepthMask)(GLboolean flag) = NULL;
    void (*real_glDepthRangef)(GLfloat n, GLfloat f) = NULL;
    void (*real_glDetachShader)(GLuint program, GLuint shader) = NULL;
    void (*real_glDisable)(GLenum cap) = NULL;
    void (*real_glDisableVertexAttribArray)(GLuint index) = NULL;
    void (*real_glDrawArrays)(GLenum mode, GLint first, GLsizei count) = NULL;
    void (*real_glDrawElements)(GLenum mode, GLsizei count, GLenum type, const void* indices) = NULL;
    void (*real_glEnable)(GLenum cap) = NULL;
    void (*real_glEnableVertexAttribArray)(GLuint index) = NULL;
    void (*real_glFinish)(void) = NULL;
    void (*real_glFlush)(void) = NULL;
    void (*real_glFramebufferRenderbuffer)(GLenum target, GLenum attachment, GLenum renderbuffertarget, GLuint renderbuffer) = NULL;
    void (*real_glFramebufferTexture2D)(GLenum target, GLenum attachment, GLenum textarget, GLuint texture, GLint level) = NULL;
    void (*real_glFrontFace)(GLenum mode) = NULL;
    void (*real_glGenBuffers)(GLsizei n, GLuint* buffers) = NULL;
    void (*real_glGenerateMipmap)(GLenum target) = NULL;
    void (*real_glGenFramebuffers)(GLsizei n, GLuint* framebuffers) = NULL;
    void (*real_glGenRenderbuffers)(GLsizei n, GLuint* renderbuffers) = NULL;
    void (*real_glGenTextures)(GLsizei n, GLuint* textures) = NULL;
    void (*real_glGetActiveAttrib)(GLuint program, GLuint index, GLsizei bufSize, GLsizei* length, GLint* size, GLenum* type, GLchar* name) = NULL;
    void (*real_glGetActiveUniform)(GLuint program, GLuint index, GLsizei bufSize, GLsizei* length, GLint* size, GLenum* type, GLchar* name) = NULL;
    void (*real_glGetAttachedShaders)(GLuint program, GLsizei maxCount, GLsizei* count, GLuint* shaders) = NULL;
    GLint (*real_glGetAttribLocation)(GLuint program, const GLchar* name) = NULL;
    void (*real_glGetBooleanv)(GLenum pname, GLboolean* data) = NULL;
    void (*real_glGetBufferParameteriv)(GLenum target, GLenum pname, GLint* params) = NULL;
    GLenum (*real_glGetError)(void) = NULL;
    void (*real_glGetFloatv)(GLenum pname, GLfloat* data) = NULL;
    void (*real_glGetFramebufferAttachmentParameteriv)(GLenum target, GLenum attachment, GLenum pname, GLint* params) = NULL;
    void (*real_glGetIntegerv)(GLenum pname, GLint* data) = NULL;
    void (*real_glGetProgramiv)(GLuint program, GLenum pname, GLint* params) = NULL;
    void (*real_glGetProgramInfoLog)(GLuint program, GLsizei bufSize, GLsizei* length, GLchar* infoLog) = NULL;
    void (*real_glGetRenderbufferParameteriv)(GLenum target, GLenum pname, GLint* params) = NULL;
    void (*real_glGetShaderiv)(GLuint shader, GLenum pname, GLint* params) = NULL;
    void (*real_glGetShaderInfoLog)(GLuint shader, GLsizei bufSize, GLsizei* length, GLchar* infoLog) = NULL;
    void (*real_glGetShaderPrecisionFormat)(GLenum shadertype, GLenum precisiontype, GLint* range, GLint* precision) = NULL;
    void (*real_glGetShaderSource)(GLuint shader, GLsizei bufSize, GLsizei* length, GLchar* source) = NULL;
    const GLubyte* (*real_glGetString)(GLenum name) = NULL;
    void (*real_glGetTexParameterfv)(GLenum target, GLenum pname, GLfloat* params) = NULL;
    void (*real_glGetTexParameteriv)(GLenum target, GLenum pname, GLint* params) = NULL;
    void (*real_glGetUniformfv)(GLuint program, GLint location, GLfloat* params) = NULL;
    void (*real_glGetUniformiv)(GLuint program, GLint location, GLint* params) = NULL;
    GLint (*real_glGetUniformLocation)(GLuint program, const GLchar* name) = NULL;
    void (*real_glGetVertexAttribfv)(GLuint index, GLenum pname, GLfloat* params) = NULL;
    void (*real_glGetVertexAttribiv)(GLuint index, GLenum pname, GLint* params) = NULL;
    void (*real_glGetVertexAttribPointerv)(GLuint index, GLenum pname, void** pointer) = NULL;
    void (*real_glHint)(GLenum target, GLenum mode) = NULL;
    GLboolean (*real_glIsBuffer)(GLuint buffer) = NULL;
    GLboolean (*real_glIsEnabled)(GLenum cap) = NULL;
    GLboolean (*real_glIsFramebuffer)(GLuint framebuffer) = NULL;
    GLboolean (*real_glIsProgram)(GLuint program) = NULL;
    GLboolean (*real_glIsRenderbuffer)(GLuint renderbuffer) = NULL;
    GLboolean (*real_glIsShader)(GLuint shader) = NULL;
    GLboolean (*real_glIsTexture)(GLuint texture) = NULL;
    void (*real_glLineWidth)(GLfloat width) = NULL;
    void (*real_glLinkProgram)(GLuint program) = NULL;
    void (*real_glPixelStorei)(GLenum pname, GLint param) = NULL;
    void (*real_glPolygonOffset)(GLfloat factor, GLfloat units) = NULL;
    void (*real_glReadPixels)(GLint x, GLint y, GLsizei width, GLsizei height, GLenum format, GLenum type, void* pixels) = NULL;
    void (*real_glReleaseShaderCompiler)(void) = NULL;
    void (*real_glRenderbufferStorage)(GLenum target, GLenum internalformat, GLsizei width, GLsizei height) = NULL;
    void (*real_glSampleCoverage)(GLfloat value, GLboolean invert) = NULL;
    void (*real_glScissor)(GLint x, GLint y, GLsizei width, GLsizei height) = NULL;
    void (*real_glShaderBinary)(GLsizei count, const GLuint* shaders, GLenum binaryformat, const void* binary, GLsizei length) = NULL;
    void (*real_glShaderSource)(GLuint shader, GLsizei count, const GLchar* const* string, const GLint* length) = NULL;
    void (*real_glStencilFunc)(GLenum func, GLint ref, GLuint mask) = NULL;
    void (*real_glStencilFuncSeparate)(GLenum face, GLenum func, GLint ref, GLuint mask) = NULL;
    void (*real_glStencilMask)(GLuint mask) = NULL;
    void (*real_glStencilMaskSeparate)(GLenum face, GLuint mask) = NULL;
    void (*real_glStencilOp)(GLenum fail, GLenum zfail, GLenum zpass) = NULL;
    void (*real_glStencilOpSeparate)(GLenum face, GLenum sfail, GLenum dpfail, GLenum dppass) = NULL;
    void (*real_glTexImage2D)(GLenum target, GLint level, GLint internalformat, GLsizei width, GLsizei height, GLint border, GLenum format, GLenum type, const void* pixels) = NULL;
    void (*real_glTexParameterf)(GLenum target, GLenum pname, GLfloat param) = NULL;
    void (*real_glTexParameterfv)(GLenum target, GLenum pname, const GLfloat* params) = NULL;
    void (*real_glTexParameteri)(GLenum target, GLenum pname, GLint param) = NULL;
    void (*real_glTexParameteriv)(GLenum target, GLenum pname, const GLint* params) = NULL;
    void (*real_glTexSubImage2D)(GLenum target, GLint level, GLint xoffset, GLint yoffset, GLsizei width, GLsizei height, GLenum format, GLenum type, const void* pixels) = NULL;
    void (*real_glUniform1f)(GLint location, GLfloat v0) = NULL;
    void (*real_glUniform1fv)(GLint location, GLsizei count, const GLfloat* value) = NULL;
    void (*real_glUniform1i)(GLint location, GLint v0) = NULL;
    void (*real_glUniform1iv)(GLint location, GLsizei count, const GLint* value) = NULL;
    void (*real_glUniform2f)(GLint location, GLfloat v0, GLfloat v1) = NULL;
    void (*real_glUniform2fv)(GLint location, GLsizei count, const GLfloat* value) = NULL;
    void (*real_glUniform2i)(GLint location, GLint v0, GLint v1) = NULL;
    void (*real_glUniform2iv)(GLint location, GLsizei count, const GLint* value) = NULL;
    void (*real_glUniform3f)(GLint location, GLfloat v0, GLfloat v1, GLfloat v2) = NULL;
    void (*real_glUniform3fv)(GLint location, GLsizei count, const GLfloat* value) = NULL;
    void (*real_glUniform3i)(GLint location, GLint v0, GLint v1, GLint v2) = NULL;
    void (*real_glUniform3iv)(GLint location, GLsizei count, const GLint* value) = NULL;
    void (*real_glUniform4f)(GLint location, GLfloat v0, GLfloat v1, GLfloat v2, GLfloat v3) = NULL;
    void (*real_glUniform4fv)(GLint location, GLsizei count, const GLfloat* value) = NULL;
    void (*real_glUniform4i)(GLint location, GLint v0, GLint v1, GLint v2, GLint v3) = NULL;
    void (*real_glUniform4iv)(GLint location, GLsizei count, const GLint* value) = NULL;
    void (*real_glUniformMatrix2fv)(GLint location, GLsizei count, GLboolean transpose, const GLfloat* value) = NULL;
    void (*real_glUniformMatrix3fv)(GLint location, GLsizei count, GLboolean transpose, const GLfloat* value) = NULL;
    void (*real_glUniformMatrix4fv)(GLint location, GLsizei count, GLboolean transpose, const GLfloat* value) = NULL;
    void (*real_glUseProgram)(GLuint program) = NULL;
    void (*real_glValidateProgram)(GLuint program) = NULL;
    void (*real_glVertexAttrib1f)(GLuint index, GLfloat x) = NULL;
    void (*real_glVertexAttrib1fv)(GLuint index, const GLfloat* v) = NULL;
    void (*real_glVertexAttrib2f)(GLuint index, GLfloat x, GLfloat y) = NULL;
    void (*real_glVertexAttrib2fv)(GLuint index, const GLfloat* v) = NULL;
    void (*real_glVertexAttrib3f)(GLuint index, GLfloat x, GLfloat y, GLfloat z) = NULL;
    void (*real_glVertexAttrib3fv)(GLuint index, const GLfloat* v) = NULL;
    void (*real_glVertexAttrib4f)(GLuint index, GLfloat x, GLfloat y, GLfloat z, GLfloat w) = NULL;
    void (*real_glVertexAttrib4fv)(GLuint index, const GLfloat* v) = NULL;
    void (*real_glVertexAttribPointer)(GLuint index, GLint size, GLenum type, GLboolean normalized, GLsizei stride, const void* pointer) = NULL;
    void (*real_glViewport)(GLint x, GLint y, GLsizei width, GLsizei height) = NULL;
    
    
    
    thread_local EGLint lastError = EGL_SUCCESS;
    
    /**
     * @brief Looks up an GLES function and errors if not found.
     * 
     */
    #define GLFUNC(fname) real_ ## fname = (decltype(real_ ## fname)) dlsym(nativeGLES2Library, #fname); \
    if (real_ ## fname == NULL) { \
        fprintf(stderr, "ERROR: Could not find GLES function '" #fname "'"); \
        fflush(stderr); \
        return EGL_FALSE; \
    }
    
    /**
     * @brief Looks up an EGL function and errors if not found.
     * 
     */
    #define EGLFUNC(fname) real_ ## fname = (decltype(real_ ## fname)) dlsym(nativeEGLLibrary, #fname); \
    if (real_ ## fname == NULL) { \
        fprintf(stderr, "ERROR: Could not find EGL function '" #fname "'"); \
        fflush(stderr); \
        return EGL_FALSE; \
    }
    
    /**
     * @brief Looks up an EGL function and sets to NULL if not found.
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
        if (nativeEGLLibrary == NULL) {
            nativeEGLLibrary = dlopen("/system/lib/libEGL.so", RTLD_LAZY | RTLD_LOCAL);
            if (nativeGLES2Library != NULL) dlclose(nativeGLES2Library);
            nativeGLES2Library = dlopen("/system/lib/libGLESv2.so", RTLD_LAZY | RTLD_LOCAL);
        }
        if (nativeEGLLibrary == NULL || nativeGLES2Library == NULL) {
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
        
        
        
        GLFUNC(glActiveTexture)
        GLFUNC(glAttachShader)
        GLFUNC(glBindAttribLocation)
        GLFUNC(glBindBuffer)
        GLFUNC(glBindFramebuffer)
        GLFUNC(glBindRenderbuffer)
        GLFUNC(glBindTexture)
        GLFUNC(glBlendColor)
        GLFUNC(glBlendEquation)
        GLFUNC(glBlendEquationSeparate)
        GLFUNC(glBlendFunc)
        GLFUNC(glBlendFuncSeparate)
        GLFUNC(glBufferData)
        GLFUNC(glBufferSubData)
        GLFUNC(glCheckFramebufferStatus)
        GLFUNC(glClear)
        GLFUNC(glClearColor)
        GLFUNC(glClearDepthf)
        GLFUNC(glClearStencil)
        GLFUNC(glColorMask)
        GLFUNC(glCompileShader)
        GLFUNC(glCompressedTexImage2D)
        GLFUNC(glCompressedTexSubImage2D)
        GLFUNC(glCopyTexImage2D)
        GLFUNC(glCopyTexSubImage2D)
        GLFUNC(glCreateProgram)
        GLFUNC(glCreateShader)
        GLFUNC(glCullFace)
        GLFUNC(glDeleteBuffers)
        GLFUNC(glDeleteFramebuffers)
        GLFUNC(glDeleteProgram)
        GLFUNC(glDeleteRenderbuffers)
        GLFUNC(glDeleteShader)
        GLFUNC(glDeleteTextures)
        GLFUNC(glDepthFunc)
        GLFUNC(glDepthMask)
        GLFUNC(glDepthRangef)
        GLFUNC(glDetachShader)
        GLFUNC(glDisable)
        GLFUNC(glDisableVertexAttribArray)
        GLFUNC(glDrawArrays)
        GLFUNC(glDrawElements)
        GLFUNC(glEnable)
        GLFUNC(glEnableVertexAttribArray)
        GLFUNC(glFinish)
        GLFUNC(glFlush)
        GLFUNC(glFramebufferRenderbuffer)
        GLFUNC(glFramebufferTexture2D)
        GLFUNC(glFrontFace)
        GLFUNC(glGenBuffers)
        GLFUNC(glGenerateMipmap)
        GLFUNC(glGenFramebuffers)
        GLFUNC(glGenRenderbuffers)
        GLFUNC(glGenTextures)
        GLFUNC(glGetActiveAttrib)
        GLFUNC(glGetActiveUniform)
        GLFUNC(glGetAttachedShaders)
        GLFUNC(glGetAttribLocation)
        GLFUNC(glGetBooleanv)
        GLFUNC(glGetBufferParameteriv)
        GLFUNC(glGetError)
        GLFUNC(glGetFloatv)
        GLFUNC(glGetFramebufferAttachmentParameteriv)
        GLFUNC(glGetIntegerv)
        GLFUNC(glGetProgramiv)
        GLFUNC(glGetProgramInfoLog)
        GLFUNC(glGetRenderbufferParameteriv)
        GLFUNC(glGetShaderiv)
        GLFUNC(glGetShaderInfoLog)
        GLFUNC(glGetShaderPrecisionFormat)
        GLFUNC(glGetShaderSource)
        GLFUNC(glGetString)
        GLFUNC(glGetTexParameterfv)
        GLFUNC(glGetTexParameteriv)
        GLFUNC(glGetUniformfv)
        GLFUNC(glGetUniformiv)
        GLFUNC(glGetUniformLocation)
        GLFUNC(glGetVertexAttribfv)
        GLFUNC(glGetVertexAttribiv)
        GLFUNC(glGetVertexAttribPointerv)
        GLFUNC(glHint)
        GLFUNC(glIsBuffer)
        GLFUNC(glIsEnabled)
        GLFUNC(glIsFramebuffer)
        GLFUNC(glIsProgram)
        GLFUNC(glIsRenderbuffer)
        GLFUNC(glIsShader)
        GLFUNC(glIsTexture)
        GLFUNC(glLineWidth)
        GLFUNC(glLinkProgram)
        GLFUNC(glPixelStorei)
        GLFUNC(glPolygonOffset)
        GLFUNC(glReadPixels)
        GLFUNC(glReleaseShaderCompiler)
        GLFUNC(glRenderbufferStorage)
        GLFUNC(glSampleCoverage)
        GLFUNC(glScissor)
        GLFUNC(glShaderBinary)
        GLFUNC(glShaderSource)
        GLFUNC(glStencilFunc)
        GLFUNC(glStencilFuncSeparate)
        GLFUNC(glStencilMask)
        GLFUNC(glStencilMaskSeparate)
        GLFUNC(glStencilOp)
        GLFUNC(glStencilOpSeparate)
        GLFUNC(glTexImage2D)
        GLFUNC(glTexParameterf)
        GLFUNC(glTexParameterfv)
        GLFUNC(glTexParameteri)
        GLFUNC(glTexParameteriv)
        GLFUNC(glTexSubImage2D)
        GLFUNC(glUniform1f)
        GLFUNC(glUniform1fv)
        GLFUNC(glUniform1i)
        GLFUNC(glUniform1iv)
        GLFUNC(glUniform2f)
        GLFUNC(glUniform2fv)
        GLFUNC(glUniform2i)
        GLFUNC(glUniform2iv)
        GLFUNC(glUniform3f)
        GLFUNC(glUniform3fv)
        GLFUNC(glUniform3i)
        GLFUNC(glUniform3iv)
        GLFUNC(glUniform4f)
        GLFUNC(glUniform4fv)
        GLFUNC(glUniform4i)
        GLFUNC(glUniform4iv)
        GLFUNC(glUniformMatrix2fv)
        GLFUNC(glUniformMatrix3fv)
        GLFUNC(glUniformMatrix4fv)
        GLFUNC(glUseProgram)
        GLFUNC(glValidateProgram)
        GLFUNC(glVertexAttrib1f)
        GLFUNC(glVertexAttrib1fv)
        GLFUNC(glVertexAttrib2f)
        GLFUNC(glVertexAttrib2fv)
        GLFUNC(glVertexAttrib3f)
        GLFUNC(glVertexAttrib3fv)
        GLFUNC(glVertexAttrib4f)
        GLFUNC(glVertexAttrib4fv)
        GLFUNC(glVertexAttribPointer)
        GLFUNC(glViewport)
        
        
        
        
        
        // check needed EGL functions
        if ( // EGL 1.0-1.4 functions
            real_eglChooseConfig == NULL ||
            real_eglCopyBuffers == NULL ||
            real_eglCreateContext == NULL ||
            real_eglCreatePbufferSurface == NULL ||
            real_eglCreatePixmapSurface == NULL ||
            real_eglCreateWindowSurface == NULL ||
            real_eglDestroyContext == NULL ||
            real_eglDestroySurface == NULL ||
            real_eglGetConfigAttrib == NULL ||
            real_eglGetConfigs == NULL ||
            real_eglGetCurrentDisplay == NULL ||
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
            real_eglWaitNative == NULL ||
            real_eglBindTexImage == NULL ||
            real_eglReleaseTexImage == NULL ||
            real_eglSurfaceAttrib == NULL ||
            real_eglSwapInterval == NULL ||
            real_eglBindAPI == NULL ||
            real_eglQueryAPI == NULL ||
            real_eglCreatePbufferFromClientBuffer == NULL ||
            real_eglReleaseThread == NULL ||
            real_eglWaitClient == NULL ||
            real_eglGetCurrentContext == NULL
            ) {
            fprintf(stderr, "ERROR: Core EGL functions missing from Android EGL\n");
            fflush(stderr);
            return EGL_FALSE;
        }
        
        if ( // EGL 1.5 functions
            real_eglCreateSync == NULL ||
            real_eglDestroySync == NULL ||
            real_eglClientWaitSync == NULL ||
            real_eglGetSyncAttrib == NULL ||
            real_eglCreateImage == NULL ||
            real_eglDestroyImage == NULL ||
            real_eglGetPlatformDisplay == NULL ||
            real_eglCreatePlatformWindowSurface == NULL ||
            real_eglCreatePlatformPixmapSurface == NULL ||
            real_eglWaitSync == NULL
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
            //fprintf(stderr, "Creating xlib display\n");
            //fflush(stderr);
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
        if (procName == NULL) return NULL;
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
        
        
        GENERATE_DISPATCH_IF(glActiveTexture)
        GENERATE_DISPATCH_IF(glAttachShader)
        GENERATE_DISPATCH_IF(glBindAttribLocation)
        GENERATE_DISPATCH_IF(glBindBuffer)
        GENERATE_DISPATCH_IF(glBindFramebuffer)
        GENERATE_DISPATCH_IF(glBindRenderbuffer)
        GENERATE_DISPATCH_IF(glBindTexture)
        GENERATE_DISPATCH_IF(glBlendColor)
        GENERATE_DISPATCH_IF(glBlendEquation)
        GENERATE_DISPATCH_IF(glBlendEquationSeparate)
        GENERATE_DISPATCH_IF(glBlendFunc)
        GENERATE_DISPATCH_IF(glBlendFuncSeparate)
        GENERATE_DISPATCH_IF(glBufferData)
        GENERATE_DISPATCH_IF(glBufferSubData)
        GENERATE_DISPATCH_IF(glCheckFramebufferStatus)
        GENERATE_DISPATCH_IF(glClear)
        GENERATE_DISPATCH_IF(glClearColor)
        GENERATE_DISPATCH_IF(glClearDepthf)
        GENERATE_DISPATCH_IF(glClearStencil)
        GENERATE_DISPATCH_IF(glColorMask)
        GENERATE_DISPATCH_IF(glCompileShader)
        GENERATE_DISPATCH_IF(glCompressedTexImage2D)
        GENERATE_DISPATCH_IF(glCompressedTexSubImage2D)
        GENERATE_DISPATCH_IF(glCopyTexImage2D)
        GENERATE_DISPATCH_IF(glCopyTexSubImage2D)
        GENERATE_DISPATCH_IF(glCreateProgram)
        GENERATE_DISPATCH_IF(glCreateShader)
        GENERATE_DISPATCH_IF(glCullFace)
        GENERATE_DISPATCH_IF(glDeleteBuffers)
        GENERATE_DISPATCH_IF(glDeleteFramebuffers)
        GENERATE_DISPATCH_IF(glDeleteProgram)
        GENERATE_DISPATCH_IF(glDeleteRenderbuffers)
        GENERATE_DISPATCH_IF(glDeleteShader)
        GENERATE_DISPATCH_IF(glDeleteTextures)
        GENERATE_DISPATCH_IF(glDepthFunc)
        GENERATE_DISPATCH_IF(glDepthMask)
        GENERATE_DISPATCH_IF(glDepthRangef)
        GENERATE_DISPATCH_IF(glDetachShader)
        GENERATE_DISPATCH_IF(glDisable)
        GENERATE_DISPATCH_IF(glDisableVertexAttribArray)
        GENERATE_DISPATCH_IF(glDrawArrays)
        GENERATE_DISPATCH_IF(glDrawElements)
        GENERATE_DISPATCH_IF(glEnable)
        GENERATE_DISPATCH_IF(glEnableVertexAttribArray)
        GENERATE_DISPATCH_IF(glFinish)
        GENERATE_DISPATCH_IF(glFlush)
        GENERATE_DISPATCH_IF(glFramebufferRenderbuffer)
        GENERATE_DISPATCH_IF(glFramebufferTexture2D)
        GENERATE_DISPATCH_IF(glFrontFace)
        GENERATE_DISPATCH_IF(glGenBuffers)
        GENERATE_DISPATCH_IF(glGenerateMipmap)
        GENERATE_DISPATCH_IF(glGenFramebuffers)
        GENERATE_DISPATCH_IF(glGenRenderbuffers)
        GENERATE_DISPATCH_IF(glGenTextures)
        GENERATE_DISPATCH_IF(glGetActiveAttrib)
        GENERATE_DISPATCH_IF(glGetActiveUniform)
        GENERATE_DISPATCH_IF(glGetAttachedShaders)
        GENERATE_DISPATCH_IF(glGetAttribLocation)
        GENERATE_DISPATCH_IF(glGetBooleanv)
        GENERATE_DISPATCH_IF(glGetBufferParameteriv)
        GENERATE_DISPATCH_IF(glGetError)
        GENERATE_DISPATCH_IF(glGetFloatv)
        GENERATE_DISPATCH_IF(glGetFramebufferAttachmentParameteriv)
        GENERATE_DISPATCH_IF(glGetIntegerv)
        GENERATE_DISPATCH_IF(glGetProgramiv)
        GENERATE_DISPATCH_IF(glGetProgramInfoLog)
        GENERATE_DISPATCH_IF(glGetRenderbufferParameteriv)
        GENERATE_DISPATCH_IF(glGetShaderiv)
        GENERATE_DISPATCH_IF(glGetShaderInfoLog)
        GENERATE_DISPATCH_IF(glGetShaderPrecisionFormat)
        GENERATE_DISPATCH_IF(glGetShaderSource)
        GENERATE_DISPATCH_IF(glGetString)
        GENERATE_DISPATCH_IF(glGetTexParameterfv)
        GENERATE_DISPATCH_IF(glGetTexParameteriv)
        GENERATE_DISPATCH_IF(glGetUniformfv)
        GENERATE_DISPATCH_IF(glGetUniformiv)
        GENERATE_DISPATCH_IF(glGetUniformLocation)
        GENERATE_DISPATCH_IF(glGetVertexAttribfv)
        GENERATE_DISPATCH_IF(glGetVertexAttribiv)
        GENERATE_DISPATCH_IF(glGetVertexAttribPointerv)
        GENERATE_DISPATCH_IF(glHint)
        GENERATE_DISPATCH_IF(glIsBuffer)
        GENERATE_DISPATCH_IF(glIsEnabled)
        GENERATE_DISPATCH_IF(glIsFramebuffer)
        GENERATE_DISPATCH_IF(glIsProgram)
        GENERATE_DISPATCH_IF(glIsRenderbuffer)
        GENERATE_DISPATCH_IF(glIsShader)
        GENERATE_DISPATCH_IF(glIsTexture)
        GENERATE_DISPATCH_IF(glLineWidth)
        GENERATE_DISPATCH_IF(glLinkProgram)
        GENERATE_DISPATCH_IF(glPixelStorei)
        GENERATE_DISPATCH_IF(glPolygonOffset)
        GENERATE_DISPATCH_IF(glReadPixels)
        GENERATE_DISPATCH_IF(glReleaseShaderCompiler)
        GENERATE_DISPATCH_IF(glRenderbufferStorage)
        GENERATE_DISPATCH_IF(glSampleCoverage)
        GENERATE_DISPATCH_IF(glScissor)
        GENERATE_DISPATCH_IF(glShaderBinary)
        GENERATE_DISPATCH_IF(glShaderSource)
        GENERATE_DISPATCH_IF(glStencilFunc)
        GENERATE_DISPATCH_IF(glStencilFuncSeparate)
        GENERATE_DISPATCH_IF(glStencilMask)
        GENERATE_DISPATCH_IF(glStencilMaskSeparate)
        GENERATE_DISPATCH_IF(glStencilOp)
        GENERATE_DISPATCH_IF(glStencilOpSeparate)
        GENERATE_DISPATCH_IF(glTexImage2D)
        GENERATE_DISPATCH_IF(glTexParameterf)
        GENERATE_DISPATCH_IF(glTexParameterfv)
        GENERATE_DISPATCH_IF(glTexParameteri)
        GENERATE_DISPATCH_IF(glTexParameteriv)
        GENERATE_DISPATCH_IF(glTexSubImage2D)
        GENERATE_DISPATCH_IF(glUniform1f)
        GENERATE_DISPATCH_IF(glUniform1fv)
        GENERATE_DISPATCH_IF(glUniform1i)
        GENERATE_DISPATCH_IF(glUniform1iv)
        GENERATE_DISPATCH_IF(glUniform2f)
        GENERATE_DISPATCH_IF(glUniform2fv)
        GENERATE_DISPATCH_IF(glUniform2i)
        GENERATE_DISPATCH_IF(glUniform2iv)
        GENERATE_DISPATCH_IF(glUniform3f)
        GENERATE_DISPATCH_IF(glUniform3fv)
        GENERATE_DISPATCH_IF(glUniform3i)
        GENERATE_DISPATCH_IF(glUniform3iv)
        GENERATE_DISPATCH_IF(glUniform4f)
        GENERATE_DISPATCH_IF(glUniform4fv)
        GENERATE_DISPATCH_IF(glUniform4i)
        GENERATE_DISPATCH_IF(glUniform4iv)
        GENERATE_DISPATCH_IF(glUniformMatrix2fv)
        GENERATE_DISPATCH_IF(glUniformMatrix3fv)
        GENERATE_DISPATCH_IF(glUniformMatrix4fv)
        GENERATE_DISPATCH_IF(glUseProgram)
        GENERATE_DISPATCH_IF(glValidateProgram)
        GENERATE_DISPATCH_IF(glVertexAttrib1f)
        GENERATE_DISPATCH_IF(glVertexAttrib1fv)
        GENERATE_DISPATCH_IF(glVertexAttrib2f)
        GENERATE_DISPATCH_IF(glVertexAttrib2fv)
        GENERATE_DISPATCH_IF(glVertexAttrib3f)
        GENERATE_DISPATCH_IF(glVertexAttrib3fv)
        GENERATE_DISPATCH_IF(glVertexAttrib4f)
        GENERATE_DISPATCH_IF(glVertexAttrib4fv)
        GENERATE_DISPATCH_IF(glVertexAttribPointer)
        GENERATE_DISPATCH_IF(glViewport)
        return NULL;
    }
    
    void* getDispatchAddress(const char* procName) {
        if (procName == NULL) return NULL;
        GENERATE_DISPATCH_IF(eglCreateImageKHR)
        GENERATE_DISPATCH_IF(eglDestroyImageKHR)
        GENERATE_DISPATCH_IF(eglCreateNativeClientBufferANDROID)
        GENERATE_DISPATCH_IF(eglGetNativeClientBufferANDROID)
        return NULL;
    }
    
    #define DISPATCH_EXT_INDEX(func) if (strcmp(#func, procName) == 0) func ## Index = index;
    
    void setDispatchIndex(const char* procName, int index) {
        if (procName == NULL) return;
        DISPATCH_EXT_INDEX(eglCreateImageKHR)
        DISPATCH_EXT_INDEX(eglDestroyImageKHR)
    }
    
    const char* getVendorString(int name) {
        if (name != __EGL_VENDOR_STRING_PLATFORM_EXTENSIONS) return NULL;
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
    
    SmartEGLImage::~SmartEGLImage() {
        if (i != EGL_NO_IMAGE) {
            // TODO use KHR extension
        }
    }
    
    SmartEGLImage& SmartEGLImage::operator=(SmartEGLImage&& o) {
        if (this != &o) {
            
        }
        return *this;
    }
    
}





extern "C" EGLBoolean __egl_Main(uint32_t version, const __EGLapiExports *exports, __EGLvendorInfo *vendor, __EGLapiImports *imports) {
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



