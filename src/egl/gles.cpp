#include "egl.hpp"


#define IF_ANDROID if (glvnd->getCurrentDisplay() != NULL && reinterpret_cast<EGLDisplayBackend*>(glvnd->getCurrentDisplay())->type == DisplayType::NATIVE) 

namespace egl_wrapper::dispatch {
    void glActiveTexture(GLenum texture) {
        real_glActiveTexture(texture);
    }

    void glAttachShader(GLuint program, GLuint shader) {
        real_glAttachShader(program, shader);
    }

    void glBindAttribLocation(GLuint program, GLuint index, const GLchar* name) {
        real_glBindAttribLocation(program, index, name);
    }

    void glBindBuffer(GLenum target, GLuint buffer) {
        real_glBindBuffer(target, buffer);
    }

    void glBindFramebuffer(GLenum target, GLuint framebuffer) {
        real_glBindFramebuffer(target, framebuffer);
    }

    void glBindRenderbuffer(GLenum target, GLuint renderbuffer) {
        real_glBindRenderbuffer(target, renderbuffer);
    }

    void glBindTexture(GLenum target, GLuint texture) {
        real_glBindTexture(target, texture);
    }

    void glBlendColor(GLfloat red, GLfloat green, GLfloat blue, GLfloat alpha) {
        real_glBlendColor(red, green, blue, alpha);
    }

    void glBlendEquation(GLenum mode) {
        real_glBlendEquation(mode);
    }
    
    void glBlendEquationSeparate(GLenum modeRGB, GLenum modeAlpha) {
        real_glBlendEquationSeparate(modeRGB, modeAlpha);
    }
    
    void glBlendFunc(GLenum sfactor, GLenum dfactor) {
        real_glBlendFunc(sfactor, dfactor);
    }

    void glBlendFuncSeparate(GLenum sfactorRGB, GLenum dfactorRGB, GLenum sfactorAlpha, GLenum dfactorAlpha) {
        real_glBlendFuncSeparate(sfactorRGB, dfactorRGB, sfactorAlpha, dfactorAlpha);
    }
    
    void glBufferData(GLenum target, GLsizeiptr size, const void* data, GLenum usage) {
        real_glBufferData(target, size, data, usage);
    }

    void glBufferSubData(GLenum target, GLintptr offset, GLsizeiptr size, const void* data) {
        real_glBufferSubData(target, offset, size, data);
    }
    
    GLenum glCheckFramebufferStatus(GLenum target) {
        return real_glCheckFramebufferStatus(target);
    }
    
    void glClear(GLbitfield mask) {
        real_glClear(mask);
    }

    void glClearColor(GLfloat red, GLfloat green, GLfloat blue, GLfloat alpha) {
        real_glClearColor(red, green, blue, alpha);
    }

    void glClearDepthf(GLfloat d) {
        real_glClearDepthf(d);
    }
    
    void glClearStencil(GLint s) {
        real_glClearStencil(s);
    }
    
    void glColorMask(GLboolean red, GLboolean green, GLboolean blue, GLboolean alpha) {
        real_glColorMask(red, green, blue, alpha);
    }
    
    void glCompileShader(GLuint shader) {
        real_glCompileShader(shader);
    }

    void glCompressedTexImage2D(GLenum target, GLint level, GLenum internalformat, GLsizei width, GLsizei height, GLint border, GLsizei imageSize, const void* data) {
        real_glCompressedTexImage2D(target, level, internalformat, width, height, border, imageSize, data);
    }
    
    void glCompressedTexSubImage2D(GLenum target, GLint level, GLint xoffset, GLint yoffset, GLsizei width, GLsizei height, GLenum format, GLsizei imageSize, const void* data) {
        real_glCompressedTexSubImage2D(target, level, xoffset, yoffset, width, height, format, imageSize, data);
    }
    
    void glCopyTexImage2D(GLenum target, GLint level, GLenum internalformat, GLint x, GLint y, GLsizei width, GLsizei height, GLint border) {
        real_glCopyTexImage2D(target, level, internalformat, x, y, width, height, border);
    }

    void glCopyTexSubImage2D(GLenum target, GLint level, GLint xoffset, GLint yoffset, GLint x, GLint y, GLsizei width, GLsizei height) {
        real_glCopyTexSubImage2D(target, level, xoffset, yoffset, x, y, width, height);
    }
    
    GLuint glCreateProgram(void) {
        return real_glCreateProgram();
    }

    GLuint glCreateShader(GLenum type) {
        return real_glCreateShader(type);
    }

    void glCullFace(GLenum mode) {
        real_glCullFace(mode);
    }
    
    void glDeleteBuffers(GLsizei n, const GLuint* buffers) {
        real_glDeleteBuffers(n, buffers);
    }
    
    void glDeleteFramebuffers(GLsizei n, const GLuint* framebuffers) {
        real_glDeleteFramebuffers(n, framebuffers);
    }

    void glDeleteProgram(GLuint program) {
        real_glDeleteProgram(program);
    }

    void glDeleteRenderbuffers(GLsizei n, const GLuint* renderbuffers) {
        real_glDeleteRenderbuffers(n, renderbuffers);
    }

    void glDeleteShader(GLuint shader) {
        real_glDeleteShader(shader);
    }
    
    void glDeleteTextures(GLsizei n, const GLuint* textures) {
        real_glDeleteTextures(n, textures);
    }

    void glDepthFunc(GLenum func) {
        real_glDepthFunc(func);
    }

    void glDepthMask(GLboolean flag) {
        real_glDepthMask(flag);
    }

    void glDepthRangef(GLfloat n, GLfloat f) {
        real_glDepthRangef(n, f);
    }
    
    void glDetachShader(GLuint program, GLuint shader) {
        real_glDetachShader(program, shader);
    }

    void glDisable(GLenum cap) {
        real_glDisable(cap);
    }
    
    void glDisableVertexAttribArray(GLuint index) {
        real_glDisableVertexAttribArray(index);
    }
    
    void glDrawArrays(GLenum mode, GLint first, GLsizei count) {
        real_glDrawArrays(mode, first, count);
    }
    
    void glDrawElements(GLenum mode, GLsizei count, GLenum type, const void* indices) {
        real_glDrawElements(mode, count, type, indices);
    }

    void glEnable(GLenum cap) {
        real_glEnable(cap);
    }

    void glEnableVertexAttribArray(GLuint index) {
        real_glEnableVertexAttribArray(index);
    }

    void glFinish(void) {
        real_glFinish();
    }

    void glFlush(void) {
        real_glFlush();
    }

    void glFramebufferRenderbuffer(GLenum target, GLenum attachment, GLenum renderbuffertarget, GLuint renderbuffer) {
        real_glFramebufferRenderbuffer(target, attachment, renderbuffertarget, renderbuffer);
    }

    void glFramebufferTexture2D(GLenum target, GLenum attachment, GLenum textarget, GLuint texture, GLint level) {
        real_glFramebufferTexture2D(target, attachment, textarget, texture, level);
    }

    void glFrontFace(GLenum mode) {
        real_glFrontFace(mode);
    }

    void glGenBuffers(GLsizei n, GLuint* buffers) {
        real_glGenBuffers(n, buffers);
    }

    void glGenerateMipmap(GLenum target) {
        real_glGenerateMipmap(target);
    }
    
    void glGenFramebuffers(GLsizei n, GLuint* framebuffers) {
        real_glGenFramebuffers(n, framebuffers);
    }
    
    void glGenRenderbuffers(GLsizei n, GLuint* renderbuffers) {
        real_glGenRenderbuffers(n, renderbuffers);
    }
    
    void glGenTextures(GLsizei n, GLuint* textures) {
        real_glGenTextures(n, textures);
    }
    
    void glGetActiveAttrib(GLuint program, GLuint index, GLsizei bufSize, GLsizei* length, GLint* size, GLenum* type, GLchar* name) {
        real_glGetActiveAttrib(program, index, bufSize, length, size, type, name);
    }
    
    void glGetActiveUniform(GLuint program, GLuint index, GLsizei bufSize, GLsizei* length, GLint* size, GLenum* type, GLchar* name) {
        real_glGetActiveUniform(program, index, bufSize, length, size, type, name);
    }

    void glGetAttachedShaders(GLuint program, GLsizei maxCount, GLsizei* count, GLuint* shaders) {
        real_glGetAttachedShaders(program, maxCount, count, shaders);
    }
    
    GLint glGetAttribLocation(GLuint program, const GLchar* name) {
        return real_glGetAttribLocation(program, name);
    }
    
    void glGetBooleanv(GLenum pname, GLboolean* data) {
        real_glGetBooleanv(pname, data);
    }
    
    void glGetBufferParameteriv(GLenum target, GLenum pname, GLint* params) {
        real_glGetBufferParameteriv(target, pname, params);
    }

    GLenum glGetError(void) {
        return real_glGetError();
    }
    
    void glGetFloatv(GLenum pname, GLfloat* data) {
        real_glGetFloatv(pname, data);
    }

    void glGetFramebufferAttachmentParameteriv(GLenum target, GLenum attachment, GLenum pname, GLint* params) {
        real_glGetFramebufferAttachmentParameteriv(target, attachment, pname, params);
    }
    
    void glGetIntegerv(GLenum pname, GLint* data) {
        real_glGetIntegerv(pname, data);
    }

    void glGetProgramiv(GLuint program, GLenum pname, GLint* params) {
        real_glGetProgramiv(program, pname, params);
    }
    
    void glGetProgramInfoLog(GLuint program, GLsizei bufSize, GLsizei* length, GLchar* infoLog) {
        real_glGetProgramInfoLog(program, bufSize, length, infoLog);
    }
    
    void glGetRenderbufferParameteriv(GLenum target, GLenum pname, GLint* params) {
        real_glGetRenderbufferParameteriv(target, pname, params);
    }
    
    void glGetShaderiv(GLuint shader, GLenum pname, GLint* params) {
        real_glGetShaderiv(shader, pname, params);
    }
    
    void glGetShaderInfoLog(GLuint shader, GLsizei bufSize, GLsizei* length, GLchar* infoLog) {
        real_glGetShaderInfoLog(shader, bufSize, length, infoLog);
    }

    void glGetShaderPrecisionFormat(GLenum shadertype, GLenum precisiontype, GLint* range, GLint* precision) {
        real_glGetShaderPrecisionFormat(shadertype, precisiontype, range, precision);
    }
    
    void glGetShaderSource(GLuint shader, GLsizei bufSize, GLsizei* length, GLchar* source) {
        real_glGetShaderSource(shader, bufSize, length, source);
    }

    const GLubyte* glGetString(GLenum name) {
        if (name == GL_EXTENSIONS) {
            return (const GLubyte*) ""; // TODO wrap extensions
        }
        if (name == GL_VENDOR) {
            static std::string vendor = [] {
                return std::string("termux-gfx-wrapper (") + (char*) real_glGetString(GL_VENDOR) + ")";
            }();
            return (const GLubyte*) vendor.c_str();
        }
        if (name == GL_RENDERER) {
            static std::string renderer = [] {
                return std::string("termux-gfx-wrapper (") + (char*) real_glGetString(GL_RENDERER) + ")";
            }();
            return (const GLubyte*) renderer.c_str();
        }
        return real_glGetString(name);
    }
    
    void glGetTexParameterfv(GLenum target, GLenum pname, GLfloat* params) {
        real_glGetTexParameterfv(target, pname, params);
    }
    
    void glGetTexParameteriv(GLenum target, GLenum pname, GLint* params) {
        real_glGetTexParameteriv(target, pname, params);
    }
    
    void glGetUniformfv(GLuint program, GLint location, GLfloat* params) {
        real_glGetUniformfv(program, location, params);
    }
    
    void glGetUniformiv(GLuint program, GLint location, GLint* params) {
        real_glGetUniformiv(program, location, params);
    }

    GLint glGetUniformLocation(GLuint program, const GLchar* name) {
        return real_glGetUniformLocation(program, name);
    }

    void glGetVertexAttribfv(GLuint index, GLenum pname, GLfloat* params) {
        real_glGetVertexAttribfv(index, pname, params);
    }

    void glGetVertexAttribiv(GLuint index, GLenum pname, GLint* params) {
        real_glGetVertexAttribiv(index, pname, params);
    }
    
    void glGetVertexAttribPointerv(GLuint index, GLenum pname, void** pointer) {
        real_glGetVertexAttribPointerv(index, pname, pointer);
    }

    void glHint(GLenum target, GLenum mode) {
        real_glHint(target, mode);
    }

    GLboolean glIsBuffer(GLuint buffer) {
        return real_glIsBuffer(buffer);
    }
    
    GLboolean glIsEnabled(GLenum cap) {
        return real_glIsEnabled(cap);
    }
    
    GLboolean glIsFramebuffer(GLuint framebuffer) {
        return real_glIsFramebuffer(framebuffer);
    }

    GLboolean glIsProgram(GLuint program) {
        return real_glIsProgram(program);
    }

    GLboolean glIsRenderbuffer(GLuint renderbuffer) {
        return real_glIsRenderbuffer(renderbuffer);
    }

    GLboolean glIsShader(GLuint shader) {
        return real_glIsShader(shader);
    }

    GLboolean glIsTexture(GLuint texture) {
        return real_glIsTexture(texture);
    }
    
    void glLineWidth(GLfloat width) {
        real_glLineWidth(width);
    }

    void glLinkProgram(GLuint program) {
        real_glLinkProgram(program);
    }

    void glPixelStorei(GLenum pname, GLint param) {
        real_glPixelStorei(pname, param);
    }

    void glPolygonOffset(GLfloat factor, GLfloat units) {
        real_glPolygonOffset(factor, units);
    }

    void glReadPixels(GLint x, GLint y, GLsizei width, GLsizei height, GLenum format, GLenum type, void* pixels) {
        real_glReadPixels(x, y, width, height, format, type, pixels);
    }

    void glReleaseShaderCompiler(void) {
        real_glReleaseShaderCompiler();
    }

    void glRenderbufferStorage(GLenum target, GLenum internalformat, GLsizei width, GLsizei height) {
        real_glRenderbufferStorage(target, internalformat, width, height);
    }

    void glSampleCoverage(GLfloat value, GLboolean invert) {
        real_glSampleCoverage(value, invert);
    }

    void glScissor(GLint x, GLint y, GLsizei width, GLsizei height) {
        real_glScissor(x, y, width, height);
    }

    void glShaderBinary(GLsizei count, const GLuint* shaders, GLenum binaryformat, const void* binary, GLsizei length) {
        real_glShaderBinary(count, shaders, binaryformat, binary, length);
    }

    void glShaderSource(GLuint shader, GLsizei count, const GLchar* const* string, const GLint* length) {
        real_glShaderSource(shader, count, string, length);
    }

    void glStencilFunc(GLenum func, GLint ref, GLuint mask) {
        real_glStencilFunc(func, ref, mask);
    }

    void glStencilFuncSeparate(GLenum face, GLenum func, GLint ref, GLuint mask) {
        real_glStencilFuncSeparate(face, func, ref, mask);
    }

    void glStencilMask(GLuint mask) {
        real_glStencilMask(mask);
    }

    void glStencilMaskSeparate(GLenum face, GLuint mask) {
        real_glStencilMaskSeparate(face, mask);
    }

    void glStencilOp(GLenum fail, GLenum zfail, GLenum zpass) {
        real_glStencilOp(fail, zfail, zpass);
    }

    void glStencilOpSeparate(GLenum face, GLenum sfail, GLenum dpfail, GLenum dppass) {
        real_glStencilOpSeparate(face, sfail, dpfail, dppass);
    }

    void glTexImage2D(GLenum target, GLint level, GLint internalformat, GLsizei width, GLsizei height, GLint border, GLenum format, GLenum type, const void* pixels) {
        real_glTexImage2D(target, level, internalformat, width, height, border, format, type, pixels);
    }

    void glTexParameterf(GLenum target, GLenum pname, GLfloat param) {
        real_glTexParameterf(target, pname, param);
    }

    void glTexParameterfv(GLenum target, GLenum pname, const GLfloat* params) {
        real_glTexParameterfv(target, pname, params);
    }

    void glTexParameteri(GLenum target, GLenum pname, GLint param) {
        real_glTexParameteri(target, pname, param);
    }

    void glTexParameteriv(GLenum target, GLenum pname, const GLint* params) {
        real_glTexParameteriv(target, pname, params);
    }

    void glTexSubImage2D(GLenum target, GLint level, GLint xoffset, GLint yoffset, GLsizei width, GLsizei height, GLenum format, GLenum type, const void* pixels) {
        real_glTexSubImage2D(target, level, xoffset, yoffset, width, height, format, type, pixels);
    }

    void glUniform1f(GLint location, GLfloat v0) {
        real_glUniform1f(location, v0);
    }

    void glUniform1fv(GLint location, GLsizei count, const GLfloat* value) {
        real_glUniform1fv(location, count, value);
    }

    void glUniform1i(GLint location, GLint v0) {
        real_glUniform1i(location, v0);
    }

    void glUniform1iv(GLint location, GLsizei count, const GLint* value) {
        real_glUniform1iv(location, count, value);
    }

    void glUniform2f(GLint location, GLfloat v0, GLfloat v1) {
        real_glUniform2f(location, v0, v1);
    }

    void glUniform2fv(GLint location, GLsizei count, const GLfloat* value) {
        real_glUniform2fv(location, count, value);
    }

    void glUniform2i(GLint location, GLint v0, GLint v1) {
        real_glUniform2i(location, v0, v1);
    }

    void glUniform2iv(GLint location, GLsizei count, const GLint* value) {
        real_glUniform2iv(location, count, value);
    }

    void glUniform3f(GLint location, GLfloat v0, GLfloat v1, GLfloat v2) {
        real_glUniform3f(location, v0, v1, v2);
    }

    void glUniform3fv(GLint location, GLsizei count, const GLfloat* value) {
        real_glUniform3fv(location, count, value);
    }

    void glUniform3i(GLint location, GLint v0, GLint v1, GLint v2) {
        real_glUniform3i(location, v0, v1, v2);
    }

    void glUniform3iv(GLint location, GLsizei count, const GLint* value) {
        real_glUniform3iv(location, count, value);
    }

    void glUniform4f(GLint location, GLfloat v0, GLfloat v1, GLfloat v2, GLfloat v3) {
        real_glUniform4f(location, v0, v1, v2, v3);
    }

    void glUniform4fv(GLint location, GLsizei count, const GLfloat* value) {
        real_glUniform4fv(location, count, value);
    }

    void glUniform4i(GLint location, GLint v0, GLint v1, GLint v2, GLint v3) {
        real_glUniform4i(location, v0, v1, v2, v3);
    }

    void glUniform4iv(GLint location, GLsizei count, const GLint* value) {
        real_glUniform4iv(location, count, value);
    }

    void glUniformMatrix2fv(GLint location, GLsizei count, GLboolean transpose, const GLfloat* value) {
        real_glUniformMatrix2fv(location, count, transpose, value);
    }

    void glUniformMatrix3fv(GLint location, GLsizei count, GLboolean transpose, const GLfloat* value) {
        real_glUniformMatrix3fv(location, count, transpose, value);
    }

    void glUniformMatrix4fv(GLint location, GLsizei count, GLboolean transpose, const GLfloat* value) {
        real_glUniformMatrix4fv(location, count, transpose, value);
    }

    void glUseProgram(GLuint program) {
        real_glUseProgram(program);
    }

    void glValidateProgram(GLuint program) {
        real_glValidateProgram(program);
    }

    void glVertexAttrib1f(GLuint index, GLfloat x) {
        real_glVertexAttrib1f(index, x);
    }

    void glVertexAttrib1fv(GLuint index, const GLfloat* v) {
        real_glVertexAttrib1fv(index, v);
    }

    void glVertexAttrib2f(GLuint index, GLfloat x, GLfloat y) {
        real_glVertexAttrib2f(index, x, y);
    }
    
    void glVertexAttrib2fv(GLuint index, const GLfloat* v) {
        real_glVertexAttrib2fv(index, v);
    }
    
    void glVertexAttrib3f(GLuint index, GLfloat x, GLfloat y, GLfloat z) {
        real_glVertexAttrib3f(index, x, y, z);
    }
    
    void glVertexAttrib3fv(GLuint index, const GLfloat* v) {
        real_glVertexAttrib3fv(index, v);
    }
    
    void glVertexAttrib4f(GLuint index, GLfloat x, GLfloat y, GLfloat z, GLfloat w) {
        real_glVertexAttrib4f(index, x, y, z, w);
    }

    void glVertexAttrib4fv(GLuint index, const GLfloat* v) {
        real_glVertexAttrib4fv(index, v);
    }

    void glVertexAttribPointer(GLuint index, GLint size, GLenum type, GLboolean normalized, GLsizei stride, const void* pointer) {
        real_glVertexAttribPointer(index, size, type, normalized, stride, pointer);
    }

    void glViewport(GLint x, GLint y, GLsizei width, GLsizei height) {
        real_glViewport(x, y, width, height);
    }
}






