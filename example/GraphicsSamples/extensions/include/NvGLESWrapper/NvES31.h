//----------------------------------------------------------------------------------
// File:        NvGLESWrapper/NvES31.h
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
#ifndef __NvES31_h_
#define __NvES31_h_ 1

#ifdef __cplusplus
extern "C" {
#endif

/* Generated on date 20140805 */


#ifndef GL_ES_VERSION_3_1
#define GL_ES_VERSION_3_1 1
typedef unsigned int GLuint;
#include <KHR/khrplatform.h>
typedef khronos_intptr_t GLintptr;
typedef unsigned int GLenum;
typedef int GLint;
typedef char GLchar;
typedef int GLsizei;
typedef unsigned int GLbitfield;
typedef unsigned char GLboolean;
typedef khronos_float_t GLfloat;
#define GL_COMPUTE_SHADER                 0x91B9
#define GL_MAX_COMPUTE_UNIFORM_BLOCKS     0x91BB
#define GL_MAX_COMPUTE_TEXTURE_IMAGE_UNITS 0x91BC
#define GL_MAX_COMPUTE_IMAGE_UNIFORMS     0x91BD
#define GL_MAX_COMPUTE_SHARED_MEMORY_SIZE 0x8262
#define GL_MAX_COMPUTE_UNIFORM_COMPONENTS 0x8263
#define GL_MAX_COMPUTE_ATOMIC_COUNTER_BUFFERS 0x8264
#define GL_MAX_COMPUTE_ATOMIC_COUNTERS    0x8265
#define GL_MAX_COMBINED_COMPUTE_UNIFORM_COMPONENTS 0x8266
#define GL_MAX_COMPUTE_WORK_GROUP_INVOCATIONS 0x90EB
#define GL_MAX_COMPUTE_WORK_GROUP_COUNT   0x91BE
#define GL_MAX_COMPUTE_WORK_GROUP_SIZE    0x91BF
#define GL_COMPUTE_WORK_GROUP_SIZE        0x8267
#define GL_DISPATCH_INDIRECT_BUFFER       0x90EE
#define GL_DISPATCH_INDIRECT_BUFFER_BINDING 0x90EF
#define GL_COMPUTE_SHADER_BIT             0x00000020
#define GL_DRAW_INDIRECT_BUFFER           0x8F3F
#define GL_DRAW_INDIRECT_BUFFER_BINDING   0x8F43
#define GL_MAX_UNIFORM_LOCATIONS          0x826E
#define GL_FRAMEBUFFER_DEFAULT_WIDTH      0x9310
#define GL_FRAMEBUFFER_DEFAULT_HEIGHT     0x9311
#define GL_FRAMEBUFFER_DEFAULT_SAMPLES    0x9313
#define GL_FRAMEBUFFER_DEFAULT_FIXED_SAMPLE_LOCATIONS 0x9314
#define GL_MAX_FRAMEBUFFER_WIDTH          0x9315
#define GL_MAX_FRAMEBUFFER_HEIGHT         0x9316
#define GL_MAX_FRAMEBUFFER_SAMPLES        0x9318
#define GL_UNIFORM                        0x92E1
#define GL_UNIFORM_BLOCK                  0x92E2
#define GL_PROGRAM_INPUT                  0x92E3
#define GL_PROGRAM_OUTPUT                 0x92E4
#define GL_BUFFER_VARIABLE                0x92E5
#define GL_SHADER_STORAGE_BLOCK           0x92E6
#define GL_ATOMIC_COUNTER_BUFFER          0x92C0
#define GL_TRANSFORM_FEEDBACK_VARYING     0x92F4
#define GL_ACTIVE_RESOURCES               0x92F5
#define GL_MAX_NAME_LENGTH                0x92F6
#define GL_MAX_NUM_ACTIVE_VARIABLES       0x92F7
#define GL_NAME_LENGTH                    0x92F9
#define GL_TYPE                           0x92FA
#define GL_ARRAY_SIZE                     0x92FB
#define GL_OFFSET                         0x92FC
#define GL_BLOCK_INDEX                    0x92FD
#define GL_ARRAY_STRIDE                   0x92FE
#define GL_MATRIX_STRIDE                  0x92FF
#define GL_IS_ROW_MAJOR                   0x9300
#define GL_ATOMIC_COUNTER_BUFFER_INDEX    0x9301
#define GL_BUFFER_BINDING                 0x9302
#define GL_BUFFER_DATA_SIZE               0x9303
#define GL_NUM_ACTIVE_VARIABLES           0x9304
#define GL_ACTIVE_VARIABLES               0x9305
#define GL_REFERENCED_BY_VERTEX_SHADER    0x9306
#define GL_REFERENCED_BY_FRAGMENT_SHADER  0x930A
#define GL_REFERENCED_BY_COMPUTE_SHADER   0x930B
#define GL_TOP_LEVEL_ARRAY_SIZE           0x930C
#define GL_TOP_LEVEL_ARRAY_STRIDE         0x930D
#define GL_LOCATION                       0x930E
#define GL_VERTEX_SHADER_BIT              0x00000001
#define GL_FRAGMENT_SHADER_BIT            0x00000002
#define GL_ALL_SHADER_BITS                0xFFFFFFFF
#define GL_PROGRAM_SEPARABLE              0x8258
#define GL_ACTIVE_PROGRAM                 0x8259
#define GL_PROGRAM_PIPELINE_BINDING       0x825A
#define GL_ATOMIC_COUNTER_BUFFER_BINDING  0x92C1
#define GL_ATOMIC_COUNTER_BUFFER_START    0x92C2
#define GL_ATOMIC_COUNTER_BUFFER_SIZE     0x92C3
#define GL_MAX_VERTEX_ATOMIC_COUNTER_BUFFERS 0x92CC
#define GL_MAX_FRAGMENT_ATOMIC_COUNTER_BUFFERS 0x92D0
#define GL_MAX_COMBINED_ATOMIC_COUNTER_BUFFERS 0x92D1
#define GL_MAX_VERTEX_ATOMIC_COUNTERS     0x92D2
#define GL_MAX_FRAGMENT_ATOMIC_COUNTERS   0x92D6
#define GL_MAX_COMBINED_ATOMIC_COUNTERS   0x92D7
#define GL_MAX_ATOMIC_COUNTER_BUFFER_SIZE 0x92D8
#define GL_MAX_ATOMIC_COUNTER_BUFFER_BINDINGS 0x92DC
#define GL_ACTIVE_ATOMIC_COUNTER_BUFFERS  0x92D9
#define GL_UNSIGNED_INT_ATOMIC_COUNTER    0x92DB
#define GL_MAX_IMAGE_UNITS                0x8F38
#define GL_MAX_VERTEX_IMAGE_UNIFORMS      0x90CA
#define GL_MAX_FRAGMENT_IMAGE_UNIFORMS    0x90CE
#define GL_MAX_COMBINED_IMAGE_UNIFORMS    0x90CF
#define GL_IMAGE_BINDING_NAME             0x8F3A
#define GL_IMAGE_BINDING_LEVEL            0x8F3B
#define GL_IMAGE_BINDING_LAYERED          0x8F3C
#define GL_IMAGE_BINDING_LAYER            0x8F3D
#define GL_IMAGE_BINDING_ACCESS           0x8F3E
#define GL_IMAGE_BINDING_FORMAT           0x906E
#define GL_VERTEX_ATTRIB_ARRAY_BARRIER_BIT 0x00000001
#define GL_ELEMENT_ARRAY_BARRIER_BIT      0x00000002
#define GL_UNIFORM_BARRIER_BIT            0x00000004
#define GL_TEXTURE_FETCH_BARRIER_BIT      0x00000008
#define GL_SHADER_IMAGE_ACCESS_BARRIER_BIT 0x00000020
#define GL_COMMAND_BARRIER_BIT            0x00000040
#define GL_PIXEL_BUFFER_BARRIER_BIT       0x00000080
#define GL_TEXTURE_UPDATE_BARRIER_BIT     0x00000100
#define GL_BUFFER_UPDATE_BARRIER_BIT      0x00000200
#define GL_FRAMEBUFFER_BARRIER_BIT        0x00000400
#define GL_TRANSFORM_FEEDBACK_BARRIER_BIT 0x00000800
#define GL_ATOMIC_COUNTER_BARRIER_BIT     0x00001000
#define GL_ALL_BARRIER_BITS               0xFFFFFFFF
#define GL_IMAGE_2D                       0x904D
#define GL_IMAGE_3D                       0x904E
#define GL_IMAGE_CUBE                     0x9050
#define GL_IMAGE_2D_ARRAY                 0x9053
#define GL_INT_IMAGE_2D                   0x9058
#define GL_INT_IMAGE_3D                   0x9059
#define GL_INT_IMAGE_CUBE                 0x905B
#define GL_INT_IMAGE_2D_ARRAY             0x905E
#define GL_UNSIGNED_INT_IMAGE_2D          0x9063
#define GL_UNSIGNED_INT_IMAGE_3D          0x9064
#define GL_UNSIGNED_INT_IMAGE_CUBE        0x9066
#define GL_UNSIGNED_INT_IMAGE_2D_ARRAY    0x9069
#define GL_IMAGE_FORMAT_COMPATIBILITY_TYPE 0x90C7
#define GL_IMAGE_FORMAT_COMPATIBILITY_BY_SIZE 0x90C8
#define GL_IMAGE_FORMAT_COMPATIBILITY_BY_CLASS 0x90C9
#define GL_READ_ONLY                      0x88B8
#define GL_WRITE_ONLY                     0x88B9
#define GL_READ_WRITE                     0x88BA
#define GL_SHADER_STORAGE_BUFFER          0x90D2
#define GL_SHADER_STORAGE_BUFFER_BINDING  0x90D3
#define GL_SHADER_STORAGE_BUFFER_START    0x90D4
#define GL_SHADER_STORAGE_BUFFER_SIZE     0x90D5
#define GL_MAX_VERTEX_SHADER_STORAGE_BLOCKS 0x90D6
#define GL_MAX_FRAGMENT_SHADER_STORAGE_BLOCKS 0x90DA
#define GL_MAX_COMPUTE_SHADER_STORAGE_BLOCKS 0x90DB
#define GL_MAX_COMBINED_SHADER_STORAGE_BLOCKS 0x90DC
#define GL_MAX_SHADER_STORAGE_BUFFER_BINDINGS 0x90DD
#define GL_MAX_SHADER_STORAGE_BLOCK_SIZE  0x90DE
#define GL_SHADER_STORAGE_BUFFER_OFFSET_ALIGNMENT 0x90DF
#define GL_SHADER_STORAGE_BARRIER_BIT     0x00002000
#define GL_MAX_COMBINED_SHADER_OUTPUT_RESOURCES 0x8F39
#define GL_DEPTH_STENCIL_TEXTURE_MODE     0x90EA
#define GL_STENCIL_INDEX                  0x1901
#define GL_MIN_PROGRAM_TEXTURE_GATHER_OFFSET 0x8E5E
#define GL_MAX_PROGRAM_TEXTURE_GATHER_OFFSET 0x8E5F
#define GL_SAMPLE_POSITION                0x8E50
#define GL_SAMPLE_MASK                    0x8E51
#define GL_SAMPLE_MASK_VALUE              0x8E52
#define GL_TEXTURE_2D_MULTISAMPLE         0x9100
#define GL_MAX_SAMPLE_MASK_WORDS          0x8E59
#define GL_MAX_COLOR_TEXTURE_SAMPLES      0x910E
#define GL_MAX_DEPTH_TEXTURE_SAMPLES      0x910F
#define GL_MAX_INTEGER_SAMPLES            0x9110
#define GL_TEXTURE_BINDING_2D_MULTISAMPLE 0x9104
#define GL_TEXTURE_SAMPLES                0x9106
#define GL_TEXTURE_FIXED_SAMPLE_LOCATIONS 0x9107
#define GL_TEXTURE_WIDTH                  0x1000
#define GL_TEXTURE_HEIGHT                 0x1001
#define GL_TEXTURE_DEPTH                  0x8071
#define GL_TEXTURE_INTERNAL_FORMAT        0x1003
#define GL_TEXTURE_RED_SIZE               0x805C
#define GL_TEXTURE_GREEN_SIZE             0x805D
#define GL_TEXTURE_BLUE_SIZE              0x805E
#define GL_TEXTURE_ALPHA_SIZE             0x805F
#define GL_TEXTURE_DEPTH_SIZE             0x884A
#define GL_TEXTURE_STENCIL_SIZE           0x88F1
#define GL_TEXTURE_SHARED_SIZE            0x8C3F
#define GL_TEXTURE_RED_TYPE               0x8C10
#define GL_TEXTURE_GREEN_TYPE             0x8C11
#define GL_TEXTURE_BLUE_TYPE              0x8C12
#define GL_TEXTURE_ALPHA_TYPE             0x8C13
#define GL_TEXTURE_DEPTH_TYPE             0x8C16
#define GL_TEXTURE_COMPRESSED             0x86A1
#define GL_SAMPLER_2D_MULTISAMPLE         0x9108
#define GL_INT_SAMPLER_2D_MULTISAMPLE     0x9109
#define GL_UNSIGNED_INT_SAMPLER_2D_MULTISAMPLE 0x910A
#define GL_VERTEX_ATTRIB_BINDING          0x82D4
#define GL_VERTEX_ATTRIB_RELATIVE_OFFSET  0x82D5
#define GL_VERTEX_BINDING_DIVISOR         0x82D6
#define GL_VERTEX_BINDING_OFFSET          0x82D7
#define GL_VERTEX_BINDING_STRIDE          0x82D8
#define GL_VERTEX_BINDING_BUFFER          0x8F4F
#define GL_MAX_VERTEX_ATTRIB_RELATIVE_OFFSET 0x82D9
#define GL_MAX_VERTEX_ATTRIB_BINDINGS     0x82DA
#define GL_MAX_VERTEX_ATTRIB_STRIDE       0x82E5
#define glDispatchCompute NvRemap_glDispatchCompute
#define glDispatchComputeIndirect NvRemap_glDispatchComputeIndirect
#define glDrawArraysIndirect NvRemap_glDrawArraysIndirect
#define glDrawElementsIndirect NvRemap_glDrawElementsIndirect
#define glFramebufferParameteri NvRemap_glFramebufferParameteri
#define glGetFramebufferParameteriv NvRemap_glGetFramebufferParameteriv
#define glGetProgramInterfaceiv NvRemap_glGetProgramInterfaceiv
#define glGetProgramResourceIndex NvRemap_glGetProgramResourceIndex
#define glGetProgramResourceName NvRemap_glGetProgramResourceName
#define glGetProgramResourceiv NvRemap_glGetProgramResourceiv
#define glGetProgramResourceLocation NvRemap_glGetProgramResourceLocation
#define glUseProgramStages NvRemap_glUseProgramStages
#define glActiveShaderProgram NvRemap_glActiveShaderProgram
#define glCreateShaderProgramv NvRemap_glCreateShaderProgramv
#define glBindProgramPipeline NvRemap_glBindProgramPipeline
#define glDeleteProgramPipelines NvRemap_glDeleteProgramPipelines
#define glGenProgramPipelines NvRemap_glGenProgramPipelines
#define glIsProgramPipeline NvRemap_glIsProgramPipeline
#define glGetProgramPipelineiv NvRemap_glGetProgramPipelineiv
#define glProgramUniform1i NvRemap_glProgramUniform1i
#define glProgramUniform2i NvRemap_glProgramUniform2i
#define glProgramUniform3i NvRemap_glProgramUniform3i
#define glProgramUniform4i NvRemap_glProgramUniform4i
#define glProgramUniform1ui NvRemap_glProgramUniform1ui
#define glProgramUniform2ui NvRemap_glProgramUniform2ui
#define glProgramUniform3ui NvRemap_glProgramUniform3ui
#define glProgramUniform4ui NvRemap_glProgramUniform4ui
#define glProgramUniform1f NvRemap_glProgramUniform1f
#define glProgramUniform2f NvRemap_glProgramUniform2f
#define glProgramUniform3f NvRemap_glProgramUniform3f
#define glProgramUniform4f NvRemap_glProgramUniform4f
#define glProgramUniform1iv NvRemap_glProgramUniform1iv
#define glProgramUniform2iv NvRemap_glProgramUniform2iv
#define glProgramUniform3iv NvRemap_glProgramUniform3iv
#define glProgramUniform4iv NvRemap_glProgramUniform4iv
#define glProgramUniform1uiv NvRemap_glProgramUniform1uiv
#define glProgramUniform2uiv NvRemap_glProgramUniform2uiv
#define glProgramUniform3uiv NvRemap_glProgramUniform3uiv
#define glProgramUniform4uiv NvRemap_glProgramUniform4uiv
#define glProgramUniform1fv NvRemap_glProgramUniform1fv
#define glProgramUniform2fv NvRemap_glProgramUniform2fv
#define glProgramUniform3fv NvRemap_glProgramUniform3fv
#define glProgramUniform4fv NvRemap_glProgramUniform4fv
#define glProgramUniformMatrix2fv NvRemap_glProgramUniformMatrix2fv
#define glProgramUniformMatrix3fv NvRemap_glProgramUniformMatrix3fv
#define glProgramUniformMatrix4fv NvRemap_glProgramUniformMatrix4fv
#define glProgramUniformMatrix2x3fv NvRemap_glProgramUniformMatrix2x3fv
#define glProgramUniformMatrix3x2fv NvRemap_glProgramUniformMatrix3x2fv
#define glProgramUniformMatrix2x4fv NvRemap_glProgramUniformMatrix2x4fv
#define glProgramUniformMatrix4x2fv NvRemap_glProgramUniformMatrix4x2fv
#define glProgramUniformMatrix3x4fv NvRemap_glProgramUniformMatrix3x4fv
#define glProgramUniformMatrix4x3fv NvRemap_glProgramUniformMatrix4x3fv
#define glValidateProgramPipeline NvRemap_glValidateProgramPipeline
#define glGetProgramPipelineInfoLog NvRemap_glGetProgramPipelineInfoLog
#define glBindImageTexture NvRemap_glBindImageTexture
#define glGetBooleani_v NvRemap_glGetBooleani_v
#define glMemoryBarrier NvRemap_glMemoryBarrier
#define glMemoryBarrierByRegion NvRemap_glMemoryBarrierByRegion
#define glTexStorage2DMultisample NvRemap_glTexStorage2DMultisample
#define glGetMultisamplefv NvRemap_glGetMultisamplefv
#define glSampleMaski NvRemap_glSampleMaski
#define glGetTexLevelParameteriv NvRemap_glGetTexLevelParameteriv
#define glGetTexLevelParameterfv NvRemap_glGetTexLevelParameterfv
#define glBindVertexBuffer NvRemap_glBindVertexBuffer
#define glVertexAttribFormat NvRemap_glVertexAttribFormat
#define glVertexAttribIFormat NvRemap_glVertexAttribIFormat
#define glVertexAttribBinding NvRemap_glVertexAttribBinding
#define glVertexBindingDivisor NvRemap_glVertexBindingDivisor
#endif /* GL_ES_VERSION_3_1 */

#ifdef __cplusplus
}
#endif

#endif
