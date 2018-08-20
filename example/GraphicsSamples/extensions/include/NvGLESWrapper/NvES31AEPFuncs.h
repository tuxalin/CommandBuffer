//----------------------------------------------------------------------------------
// File:        NvGLESWrapper/NvES31AEPFuncs.h
// SDK Version: v3.00 
// Email:       gameworks@nvidia.com
// Site:        http://developer.nvidia.com/
//
// Copyright (c) 2014-2015, NVIDIA CORPORATION. All rights reserved.
//
// Redistribution and use in source and binary forms, with or without
// modification, are permitted provided that the following conditions
// are met:
//  * Redistributions of source code must retain the above copyright
//    notice, this list of conditions and the following disclaimer.
//  * Redistributions in binary form must reproduce the above copyright
//    notice, this list of conditions and the following disclaimer in the
//    documentation and/or other materials provided with the distribution.
//  * Neither the name of NVIDIA CORPORATION nor the names of its
//    contributors may be used to endorse or promote products derived
//    from this software without specific prior written permission.
//
// THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS ``AS IS'' AND ANY
// EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
// IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR
// PURPOSE ARE DISCLAIMED.  IN NO EVENT SHALL THE COPYRIGHT OWNER OR
// CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL,
// EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO,
// PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR
// PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY
// OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
// (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
// OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
//
//----------------------------------------------------------------------------------
NV_ES31_FUNC(void, glBlendBarrierKHR, (void), ());

NV_ES31_FUNC(void, glDebugMessageControlKHR, (GLenum source, GLenum type, GLenum severity, GLsizei count, const GLuint *ids, GLboolean enabled), (source, type, severity, count, ids, enabled));
NV_ES31_FUNC(void, glDebugMessageInsertKHR, (GLenum source, GLenum type, GLuint id, GLenum severity, GLsizei length, const GLchar *buf), (source, type, id, severity, length, buf));
NV_ES31_FUNC(void, glDebugMessageCallbackKHR, (GLDEBUGPROCKHR callback, const void *userParam), (callback, userParam));
NV_ES31_FUNC_RET(GLuint, glGetDebugMessageLogKHR, (GLuint count, GLsizei bufSize, GLenum *sources, GLenum *types, GLuint *ids, GLenum *severities, GLsizei *lengths, GLchar *messageLog), (count, bufSize, sources, types, ids, severities, lengths, messageLog));
NV_ES31_FUNC(void, glPushDebugGroupKHR, (GLenum source, GLuint id, GLsizei length, const GLchar *message), (source, id, length, message));
NV_ES31_FUNC(void, glPopDebugGroupKHR, (void), ());
NV_ES31_FUNC(void, glObjectLabelKHR, (GLenum identifier, GLuint name, GLsizei length, const GLchar *label), (identifier, name, length, label));
NV_ES31_FUNC(void, glGetObjectLabelKHR, (GLenum identifier, GLuint name, GLsizei bufSize, GLsizei *length, GLchar *label), (identifier, name, bufSize, length, label));
NV_ES31_FUNC(void, glObjectPtrLabelKHR, (const void *ptr, GLsizei length, const GLchar *label), (ptr, length, label));
NV_ES31_FUNC(void, glGetObjectPtrLabelKHR, (const void *ptr, GLsizei bufSize, GLsizei *length, GLchar *label), (ptr, bufSize, length, label));
NV_ES31_FUNC(void, glGetPointervKHR, (GLenum pname, void **params), (pname, params));


NV_ES31_FUNC(void, glMinSampleShadingOES, (GLfloat value), (value));





NV_ES31_FUNC(void, glTexStorage3DMultisampleOES, (GLenum target, GLsizei samples, GLenum internalformat, GLsizei width, GLsizei height, GLsizei depth, GLboolean fixedsamplelocations), (target, samples, internalformat, width, height, depth, fixedsamplelocations));

NV_ES31_FUNC(void, glCopyImageSubDataEXT, (GLuint srcName, GLenum srcTarget, GLint srcLevel, GLint srcX, GLint srcY, GLint srcZ, GLuint dstName, GLenum dstTarget, GLint dstLevel, GLint dstX, GLint dstY, GLint dstZ, GLsizei srcWidth, GLsizei srcHeight, GLsizei srcDepth), (srcName, srcTarget, srcLevel, srcX, srcY, srcZ, dstName, dstTarget, dstLevel, dstX, dstY, dstZ, srcWidth, srcHeight, srcDepth));

NV_ES31_FUNC(void, glEnableiEXT, (GLenum target, GLuint index), (target, index));
NV_ES31_FUNC(void, glDisableiEXT, (GLenum target, GLuint index), (target, index));
NV_ES31_FUNC(void, glBlendEquationiEXT, (GLuint buf, GLenum mode), (buf, mode));
NV_ES31_FUNC(void, glBlendEquationSeparateiEXT, (GLuint buf, GLenum modeRGB, GLenum modeAlpha), (buf, modeRGB, modeAlpha));
NV_ES31_FUNC(void, glBlendFunciEXT, (GLuint buf, GLenum src, GLenum dst), (buf, src, dst));
NV_ES31_FUNC(void, glBlendFuncSeparateiEXT, (GLuint buf, GLenum srcRGB, GLenum dstRGB, GLenum srcAlpha, GLenum dstAlpha), (buf, srcRGB, dstRGB, srcAlpha, dstAlpha));
NV_ES31_FUNC(void, glColorMaskiEXT, (GLuint index, GLboolean r, GLboolean g, GLboolean b, GLboolean a), (index, r, g, b, a));
NV_ES31_FUNC_RET(GLboolean, glIsEnablediEXT, (GLenum target, GLuint index), (target, index));

NV_ES31_FUNC(void, glFramebufferTextureEXT, (GLenum target, GLenum attachment, GLuint texture, GLint level), (target, attachment, texture, level));

NV_ES31_FUNC(void, glProgramParameteriEXT, (GLuint program, GLenum pname, GLint value), (program, pname, value));


NV_ES31_FUNC(void, glPrimitiveBoundingBoxEXT, (GLfloat minX, GLfloat minY, GLfloat minZ, GLfloat minW, GLfloat maxX, GLfloat maxY, GLfloat maxZ, GLfloat maxW), (minX, minY, minZ, minW, maxX, maxY, maxZ, maxW));


NV_ES31_FUNC(void, glPatchParameteriEXT, (GLenum pname, GLint value), (pname, value));

NV_ES31_FUNC(void, glTexParameterIivEXT, (GLenum target, GLenum pname, const GLint *params), (target, pname, params));
NV_ES31_FUNC(void, glTexParameterIuivEXT, (GLenum target, GLenum pname, const GLuint *params), (target, pname, params));
NV_ES31_FUNC(void, glGetTexParameterIivEXT, (GLenum target, GLenum pname, GLint *params), (target, pname, params));
NV_ES31_FUNC(void, glGetTexParameterIuivEXT, (GLenum target, GLenum pname, GLuint *params), (target, pname, params));
NV_ES31_FUNC(void, glSamplerParameterIivEXT, (GLuint sampler, GLenum pname, const GLint *param), (sampler, pname, param));
NV_ES31_FUNC(void, glSamplerParameterIuivEXT, (GLuint sampler, GLenum pname, const GLuint *param), (sampler, pname, param));
NV_ES31_FUNC(void, glGetSamplerParameterIivEXT, (GLuint sampler, GLenum pname, GLint *params), (sampler, pname, params));
NV_ES31_FUNC(void, glGetSamplerParameterIuivEXT, (GLuint sampler, GLenum pname, GLuint *params), (sampler, pname, params));

NV_ES31_FUNC(void, glTexBufferEXT, (GLenum target, GLenum internalformat, GLuint buffer), (target, internalformat, buffer));
NV_ES31_FUNC(void, glTexBufferRangeEXT, (GLenum target, GLenum internalformat, GLuint buffer, GLintptr offset, GLsizeiptr size), (target, internalformat, buffer, offset, size));



