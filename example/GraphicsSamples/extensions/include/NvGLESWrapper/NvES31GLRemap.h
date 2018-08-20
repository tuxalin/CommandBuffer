//----------------------------------------------------------------------------------
// File:        NvGLESWrapper/NvES31GLRemap.h
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
NV_ES31_WRAP_FUNC(void, glDispatchCompute, glDispatchCompute, (GLuint num_groups_x, GLuint num_groups_y, GLuint num_groups_z), (num_groups_x, num_groups_y, num_groups_z));
NV_ES31_WRAP_FUNC(void, glDispatchComputeIndirect, glDispatchComputeIndirect, (GLintptr indirect), (indirect));
NV_ES31_WRAP_FUNC(void, glDrawArraysIndirect, glDrawArraysIndirect, (GLenum mode, const void *indirect), (mode, indirect));
NV_ES31_WRAP_FUNC(void, glDrawElementsIndirect, glDrawElementsIndirect, (GLenum mode, GLenum type, const void *indirect), (mode, type, indirect));
NV_ES31_WRAP_FUNC(void, glFramebufferParameteri, glFramebufferParameteri, (GLenum target, GLenum pname, GLint param), (target, pname, param));
NV_ES31_WRAP_FUNC(void, glGetFramebufferParameteriv, glGetFramebufferParameteriv, (GLenum target, GLenum pname, GLint *params), (target, pname, params));
NV_ES31_WRAP_FUNC(void, glGetProgramInterfaceiv, glGetProgramInterfaceiv, (GLuint program, GLenum programInterface, GLenum pname, GLint *params), (program, programInterface, pname, params));
NV_ES31_WRAP_FUNC_RET(GLuint, glGetProgramResourceIndex, glGetProgramResourceIndex, (GLuint program, GLenum programInterface, const GLchar *name), (program, programInterface, name));
NV_ES31_WRAP_FUNC(void, glGetProgramResourceName, glGetProgramResourceName, (GLuint program, GLenum programInterface, GLuint index, GLsizei bufSize, GLsizei *length, GLchar *name), (program, programInterface, index, bufSize, length, name));
NV_ES31_WRAP_FUNC(void, glGetProgramResourceiv, glGetProgramResourceiv, (GLuint program, GLenum programInterface, GLuint index, GLsizei propCount, const GLenum *props, GLsizei bufSize, GLsizei *length, GLint *params), (program, programInterface, index, propCount, props, bufSize, length, params));
NV_ES31_WRAP_FUNC_RET(GLint, glGetProgramResourceLocation, glGetProgramResourceLocation, (GLuint program, GLenum programInterface, const GLchar *name), (program, programInterface, name));
NV_ES31_WRAP_FUNC(void, glUseProgramStages, glUseProgramStages, (GLuint pipeline, GLbitfield stages, GLuint program), (pipeline, stages, program));
NV_ES31_WRAP_FUNC(void, glActiveShaderProgram, glActiveShaderProgram, (GLuint pipeline, GLuint program), (pipeline, program));
NV_ES31_WRAP_FUNC_RET(GLuint, glCreateShaderProgramv, glCreateShaderProgramv, (GLenum type, GLsizei count, const GLchar *const*strings), (type, count, strings));
NV_ES31_WRAP_FUNC(void, glBindProgramPipeline, glBindProgramPipeline, (GLuint pipeline), (pipeline));
NV_ES31_WRAP_FUNC(void, glDeleteProgramPipelines, glDeleteProgramPipelines, (GLsizei n, const GLuint *pipelines), (n, pipelines));
NV_ES31_WRAP_FUNC(void, glGenProgramPipelines, glGenProgramPipelines, (GLsizei n, GLuint *pipelines), (n, pipelines));
NV_ES31_WRAP_FUNC_RET(GLboolean, glIsProgramPipeline, glIsProgramPipeline, (GLuint pipeline), (pipeline));
NV_ES31_WRAP_FUNC(void, glGetProgramPipelineiv, glGetProgramPipelineiv, (GLuint pipeline, GLenum pname, GLint *params), (pipeline, pname, params));
NV_ES31_WRAP_FUNC(void, glProgramUniform1i, glProgramUniform1i, (GLuint program, GLint location, GLint v0), (program, location, v0));
NV_ES31_WRAP_FUNC(void, glProgramUniform2i, glProgramUniform2i, (GLuint program, GLint location, GLint v0, GLint v1), (program, location, v0, v1));
NV_ES31_WRAP_FUNC(void, glProgramUniform3i, glProgramUniform3i, (GLuint program, GLint location, GLint v0, GLint v1, GLint v2), (program, location, v0, v1, v2));
NV_ES31_WRAP_FUNC(void, glProgramUniform4i, glProgramUniform4i, (GLuint program, GLint location, GLint v0, GLint v1, GLint v2, GLint v3), (program, location, v0, v1, v2, v3));
NV_ES31_WRAP_FUNC(void, glProgramUniform1ui, glProgramUniform1ui, (GLuint program, GLint location, GLuint v0), (program, location, v0));
NV_ES31_WRAP_FUNC(void, glProgramUniform2ui, glProgramUniform2ui, (GLuint program, GLint location, GLuint v0, GLuint v1), (program, location, v0, v1));
NV_ES31_WRAP_FUNC(void, glProgramUniform3ui, glProgramUniform3ui, (GLuint program, GLint location, GLuint v0, GLuint v1, GLuint v2), (program, location, v0, v1, v2));
NV_ES31_WRAP_FUNC(void, glProgramUniform4ui, glProgramUniform4ui, (GLuint program, GLint location, GLuint v0, GLuint v1, GLuint v2, GLuint v3), (program, location, v0, v1, v2, v3));
NV_ES31_WRAP_FUNC(void, glProgramUniform1f, glProgramUniform1f, (GLuint program, GLint location, GLfloat v0), (program, location, v0));
NV_ES31_WRAP_FUNC(void, glProgramUniform2f, glProgramUniform2f, (GLuint program, GLint location, GLfloat v0, GLfloat v1), (program, location, v0, v1));
NV_ES31_WRAP_FUNC(void, glProgramUniform3f, glProgramUniform3f, (GLuint program, GLint location, GLfloat v0, GLfloat v1, GLfloat v2), (program, location, v0, v1, v2));
NV_ES31_WRAP_FUNC(void, glProgramUniform4f, glProgramUniform4f, (GLuint program, GLint location, GLfloat v0, GLfloat v1, GLfloat v2, GLfloat v3), (program, location, v0, v1, v2, v3));
NV_ES31_WRAP_FUNC(void, glProgramUniform1iv, glProgramUniform1iv, (GLuint program, GLint location, GLsizei count, const GLint *value), (program, location, count, value));
NV_ES31_WRAP_FUNC(void, glProgramUniform2iv, glProgramUniform2iv, (GLuint program, GLint location, GLsizei count, const GLint *value), (program, location, count, value));
NV_ES31_WRAP_FUNC(void, glProgramUniform3iv, glProgramUniform3iv, (GLuint program, GLint location, GLsizei count, const GLint *value), (program, location, count, value));
NV_ES31_WRAP_FUNC(void, glProgramUniform4iv, glProgramUniform4iv, (GLuint program, GLint location, GLsizei count, const GLint *value), (program, location, count, value));
NV_ES31_WRAP_FUNC(void, glProgramUniform1uiv, glProgramUniform1uiv, (GLuint program, GLint location, GLsizei count, const GLuint *value), (program, location, count, value));
NV_ES31_WRAP_FUNC(void, glProgramUniform2uiv, glProgramUniform2uiv, (GLuint program, GLint location, GLsizei count, const GLuint *value), (program, location, count, value));
NV_ES31_WRAP_FUNC(void, glProgramUniform3uiv, glProgramUniform3uiv, (GLuint program, GLint location, GLsizei count, const GLuint *value), (program, location, count, value));
NV_ES31_WRAP_FUNC(void, glProgramUniform4uiv, glProgramUniform4uiv, (GLuint program, GLint location, GLsizei count, const GLuint *value), (program, location, count, value));
NV_ES31_WRAP_FUNC(void, glProgramUniform1fv, glProgramUniform1fv, (GLuint program, GLint location, GLsizei count, const GLfloat *value), (program, location, count, value));
NV_ES31_WRAP_FUNC(void, glProgramUniform2fv, glProgramUniform2fv, (GLuint program, GLint location, GLsizei count, const GLfloat *value), (program, location, count, value));
NV_ES31_WRAP_FUNC(void, glProgramUniform3fv, glProgramUniform3fv, (GLuint program, GLint location, GLsizei count, const GLfloat *value), (program, location, count, value));
NV_ES31_WRAP_FUNC(void, glProgramUniform4fv, glProgramUniform4fv, (GLuint program, GLint location, GLsizei count, const GLfloat *value), (program, location, count, value));
NV_ES31_WRAP_FUNC(void, glProgramUniformMatrix2fv, glProgramUniformMatrix2fv, (GLuint program, GLint location, GLsizei count, GLboolean transpose, const GLfloat *value), (program, location, count, transpose, value));
NV_ES31_WRAP_FUNC(void, glProgramUniformMatrix3fv, glProgramUniformMatrix3fv, (GLuint program, GLint location, GLsizei count, GLboolean transpose, const GLfloat *value), (program, location, count, transpose, value));
NV_ES31_WRAP_FUNC(void, glProgramUniformMatrix4fv, glProgramUniformMatrix4fv, (GLuint program, GLint location, GLsizei count, GLboolean transpose, const GLfloat *value), (program, location, count, transpose, value));
NV_ES31_WRAP_FUNC(void, glProgramUniformMatrix2x3fv, glProgramUniformMatrix2x3fv, (GLuint program, GLint location, GLsizei count, GLboolean transpose, const GLfloat *value), (program, location, count, transpose, value));
NV_ES31_WRAP_FUNC(void, glProgramUniformMatrix3x2fv, glProgramUniformMatrix3x2fv, (GLuint program, GLint location, GLsizei count, GLboolean transpose, const GLfloat *value), (program, location, count, transpose, value));
NV_ES31_WRAP_FUNC(void, glProgramUniformMatrix2x4fv, glProgramUniformMatrix2x4fv, (GLuint program, GLint location, GLsizei count, GLboolean transpose, const GLfloat *value), (program, location, count, transpose, value));
NV_ES31_WRAP_FUNC(void, glProgramUniformMatrix4x2fv, glProgramUniformMatrix4x2fv, (GLuint program, GLint location, GLsizei count, GLboolean transpose, const GLfloat *value), (program, location, count, transpose, value));
NV_ES31_WRAP_FUNC(void, glProgramUniformMatrix3x4fv, glProgramUniformMatrix3x4fv, (GLuint program, GLint location, GLsizei count, GLboolean transpose, const GLfloat *value), (program, location, count, transpose, value));
NV_ES31_WRAP_FUNC(void, glProgramUniformMatrix4x3fv, glProgramUniformMatrix4x3fv, (GLuint program, GLint location, GLsizei count, GLboolean transpose, const GLfloat *value), (program, location, count, transpose, value));
NV_ES31_WRAP_FUNC(void, glValidateProgramPipeline, glValidateProgramPipeline, (GLuint pipeline), (pipeline));
NV_ES31_WRAP_FUNC(void, glGetProgramPipelineInfoLog, glGetProgramPipelineInfoLog, (GLuint pipeline, GLsizei bufSize, GLsizei *length, GLchar *infoLog), (pipeline, bufSize, length, infoLog));
NV_ES31_WRAP_FUNC(void, glBindImageTexture, glBindImageTexture, (GLuint unit, GLuint texture, GLint level, GLboolean layered, GLint layer, GLenum access, GLenum format), (unit, texture, level, layered, layer, access, format));
NV_ES31_WRAP_FUNC(void, glGetBooleani_v, glGetBooleani_v, (GLenum target, GLuint index, GLboolean *data), (target, index, data));
NV_ES31_WRAP_FUNC(void, glMemoryBarrier, glMemoryBarrier, (GLbitfield barriers), (barriers));
NV_ES31_WRAP_FUNC(void, glMemoryBarrierByRegion, glMemoryBarrierByRegion, (GLbitfield barriers), (barriers));
NV_ES31_WRAP_FUNC(void, glTexStorage2DMultisample, glTexStorage2DMultisample, (GLenum target, GLsizei samples, GLenum internalformat, GLsizei width, GLsizei height, GLboolean fixedsamplelocations), (target, samples, internalformat, width, height, fixedsamplelocations));
NV_ES31_WRAP_FUNC(void, glGetMultisamplefv, glGetMultisamplefv, (GLenum pname, GLuint index, GLfloat *val), (pname, index, val));
NV_ES31_WRAP_FUNC(void, glSampleMaski, glSampleMaski, (GLuint maskNumber, GLbitfield mask), (maskNumber, mask));
NV_ES31_WRAP_FUNC(void, glGetTexLevelParameteriv, glGetTexLevelParameteriv, (GLenum target, GLint level, GLenum pname, GLint *params), (target, level, pname, params));
NV_ES31_WRAP_FUNC(void, glGetTexLevelParameterfv, glGetTexLevelParameterfv, (GLenum target, GLint level, GLenum pname, GLfloat *params), (target, level, pname, params));
NV_ES31_WRAP_FUNC(void, glBindVertexBuffer, glBindVertexBuffer, (GLuint bindingindex, GLuint buffer, GLintptr offset, GLsizei stride), (bindingindex, buffer, offset, stride));
NV_ES31_WRAP_FUNC(void, glVertexAttribFormat, glVertexAttribFormat, (GLuint attribindex, GLint size, GLenum type, GLboolean normalized, GLuint relativeoffset), (attribindex, size, type, normalized, relativeoffset));
NV_ES31_WRAP_FUNC(void, glVertexAttribIFormat, glVertexAttribIFormat, (GLuint attribindex, GLint size, GLenum type, GLuint relativeoffset), (attribindex, size, type, relativeoffset));
NV_ES31_WRAP_FUNC(void, glVertexAttribBinding, glVertexAttribBinding, (GLuint attribindex, GLuint bindingindex), (attribindex, bindingindex));
NV_ES31_WRAP_FUNC(void, glVertexBindingDivisor, glVertexBindingDivisor, (GLuint bindingindex, GLuint divisor), (bindingindex, divisor));

