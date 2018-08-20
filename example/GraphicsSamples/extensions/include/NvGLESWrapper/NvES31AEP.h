//----------------------------------------------------------------------------------
// File:        NvGLESWrapper/NvES31AEP.h
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
#ifndef __NvES31AEP_h_
#define __NvES31AEP_h_ 1

#ifdef __cplusplus
extern "C" {
#endif

/* Generated on date 20140805 */


#ifndef GL_KHR_blend_equation_advanced
#define GL_KHR_blend_equation_advanced 1
#define GL_BLEND_ADVANCED_COHERENT_KHR    0x9285
#define GL_MULTIPLY_KHR                   0x9294
#define GL_SCREEN_KHR                     0x9295
#define GL_OVERLAY_KHR                    0x9296
#define GL_DARKEN_KHR                     0x9297
#define GL_LIGHTEN_KHR                    0x9298
#define GL_COLORDODGE_KHR                 0x9299
#define GL_COLORBURN_KHR                  0x929A
#define GL_HARDLIGHT_KHR                  0x929B
#define GL_SOFTLIGHT_KHR                  0x929C
#define GL_DIFFERENCE_KHR                 0x929E
#define GL_EXCLUSION_KHR                  0x92A0
#define GL_HSL_HUE_KHR                    0x92AD
#define GL_HSL_SATURATION_KHR             0x92AE
#define GL_HSL_COLOR_KHR                  0x92AF
#define GL_HSL_LUMINOSITY_KHR             0x92B0
#define glBlendBarrierKHR NvRemap_glBlendBarrierKHR
#endif /* GL_KHR_blend_equation_advanced */

#ifndef GL_KHR_debug
#define GL_KHR_debug 1
typedef unsigned int GLenum;
typedef int GLsizei;
typedef unsigned int GLuint;
typedef unsigned char GLboolean;
typedef char GLchar;
typedef void (GL_APIENTRY  *GLDEBUGPROCKHR)(GLenum source,GLenum type,GLuint id,GLenum severity,GLsizei length,const GLchar *message,const void *userParam);
#define GL_DEBUG_OUTPUT_SYNCHRONOUS_KHR   0x8242
#define GL_DEBUG_NEXT_LOGGED_MESSAGE_LENGTH_KHR 0x8243
#define GL_DEBUG_CALLBACK_FUNCTION_KHR    0x8244
#define GL_DEBUG_CALLBACK_USER_PARAM_KHR  0x8245
#define GL_DEBUG_SOURCE_API_KHR           0x8246
#define GL_DEBUG_SOURCE_WINDOW_SYSTEM_KHR 0x8247
#define GL_DEBUG_SOURCE_SHADER_COMPILER_KHR 0x8248
#define GL_DEBUG_SOURCE_THIRD_PARTY_KHR   0x8249
#define GL_DEBUG_SOURCE_APPLICATION_KHR   0x824A
#define GL_DEBUG_SOURCE_OTHER_KHR         0x824B
#define GL_DEBUG_TYPE_ERROR_KHR           0x824C
#define GL_DEBUG_TYPE_DEPRECATED_BEHAVIOR_KHR 0x824D
#define GL_DEBUG_TYPE_UNDEFINED_BEHAVIOR_KHR 0x824E
#define GL_DEBUG_TYPE_PORTABILITY_KHR     0x824F
#define GL_DEBUG_TYPE_PERFORMANCE_KHR     0x8250
#define GL_DEBUG_TYPE_OTHER_KHR           0x8251
#define GL_DEBUG_TYPE_MARKER_KHR          0x8268
#define GL_DEBUG_TYPE_PUSH_GROUP_KHR      0x8269
#define GL_DEBUG_TYPE_POP_GROUP_KHR       0x826A
#define GL_DEBUG_SEVERITY_NOTIFICATION_KHR 0x826B
#define GL_MAX_DEBUG_GROUP_STACK_DEPTH_KHR 0x826C
#define GL_DEBUG_GROUP_STACK_DEPTH_KHR    0x826D
#define GL_BUFFER_KHR                     0x82E0
#define GL_SHADER_KHR                     0x82E1
#define GL_PROGRAM_KHR                    0x82E2
#define GL_VERTEX_ARRAY_KHR               0x8074
#define GL_QUERY_KHR                      0x82E3
#define GL_SAMPLER_KHR                    0x82E6
#define GL_MAX_LABEL_LENGTH_KHR           0x82E8
#define GL_MAX_DEBUG_MESSAGE_LENGTH_KHR   0x9143
#define GL_MAX_DEBUG_LOGGED_MESSAGES_KHR  0x9144
#define GL_DEBUG_LOGGED_MESSAGES_KHR      0x9145
#define GL_DEBUG_SEVERITY_HIGH_KHR        0x9146
#define GL_DEBUG_SEVERITY_MEDIUM_KHR      0x9147
#define GL_DEBUG_SEVERITY_LOW_KHR         0x9148
#define GL_DEBUG_OUTPUT_KHR               0x92E0
#define GL_CONTEXT_FLAG_DEBUG_BIT_KHR     0x00000002
#define GL_STACK_OVERFLOW_KHR             0x0503
#define GL_STACK_UNDERFLOW_KHR            0x0504
#define glDebugMessageControlKHR NvRemap_glDebugMessageControlKHR
#define glDebugMessageInsertKHR NvRemap_glDebugMessageInsertKHR
#define glDebugMessageCallbackKHR NvRemap_glDebugMessageCallbackKHR
#define glGetDebugMessageLogKHR NvRemap_glGetDebugMessageLogKHR
#define glPushDebugGroupKHR NvRemap_glPushDebugGroupKHR
#define glPopDebugGroupKHR NvRemap_glPopDebugGroupKHR
#define glObjectLabelKHR NvRemap_glObjectLabelKHR
#define glGetObjectLabelKHR NvRemap_glGetObjectLabelKHR
#define glObjectPtrLabelKHR NvRemap_glObjectPtrLabelKHR
#define glGetObjectPtrLabelKHR NvRemap_glGetObjectPtrLabelKHR
#define glGetPointervKHR NvRemap_glGetPointervKHR
#endif /* GL_KHR_debug */

#ifndef GL_KHR_texture_compression_astc_ldr
#define GL_KHR_texture_compression_astc_ldr 1
#define GL_COMPRESSED_RGBA_ASTC_4x4_KHR   0x93B0
#define GL_COMPRESSED_RGBA_ASTC_5x4_KHR   0x93B1
#define GL_COMPRESSED_RGBA_ASTC_5x5_KHR   0x93B2
#define GL_COMPRESSED_RGBA_ASTC_6x5_KHR   0x93B3
#define GL_COMPRESSED_RGBA_ASTC_6x6_KHR   0x93B4
#define GL_COMPRESSED_RGBA_ASTC_8x5_KHR   0x93B5
#define GL_COMPRESSED_RGBA_ASTC_8x6_KHR   0x93B6
#define GL_COMPRESSED_RGBA_ASTC_8x8_KHR   0x93B7
#define GL_COMPRESSED_RGBA_ASTC_10x5_KHR  0x93B8
#define GL_COMPRESSED_RGBA_ASTC_10x6_KHR  0x93B9
#define GL_COMPRESSED_RGBA_ASTC_10x8_KHR  0x93BA
#define GL_COMPRESSED_RGBA_ASTC_10x10_KHR 0x93BB
#define GL_COMPRESSED_RGBA_ASTC_12x10_KHR 0x93BC
#define GL_COMPRESSED_RGBA_ASTC_12x12_KHR 0x93BD
#define GL_COMPRESSED_SRGB8_ALPHA8_ASTC_4x4_KHR 0x93D0
#define GL_COMPRESSED_SRGB8_ALPHA8_ASTC_5x4_KHR 0x93D1
#define GL_COMPRESSED_SRGB8_ALPHA8_ASTC_5x5_KHR 0x93D2
#define GL_COMPRESSED_SRGB8_ALPHA8_ASTC_6x5_KHR 0x93D3
#define GL_COMPRESSED_SRGB8_ALPHA8_ASTC_6x6_KHR 0x93D4
#define GL_COMPRESSED_SRGB8_ALPHA8_ASTC_8x5_KHR 0x93D5
#define GL_COMPRESSED_SRGB8_ALPHA8_ASTC_8x6_KHR 0x93D6
#define GL_COMPRESSED_SRGB8_ALPHA8_ASTC_8x8_KHR 0x93D7
#define GL_COMPRESSED_SRGB8_ALPHA8_ASTC_10x5_KHR 0x93D8
#define GL_COMPRESSED_SRGB8_ALPHA8_ASTC_10x6_KHR 0x93D9
#define GL_COMPRESSED_SRGB8_ALPHA8_ASTC_10x8_KHR 0x93DA
#define GL_COMPRESSED_SRGB8_ALPHA8_ASTC_10x10_KHR 0x93DB
#define GL_COMPRESSED_SRGB8_ALPHA8_ASTC_12x10_KHR 0x93DC
#define GL_COMPRESSED_SRGB8_ALPHA8_ASTC_12x12_KHR 0x93DD
#endif /* GL_KHR_texture_compression_astc_ldr */

#ifndef GL_OES_sample_shading
#define GL_OES_sample_shading 1
#include <KHR/khrplatform.h>
typedef khronos_float_t GLfloat;
#define GL_SAMPLE_SHADING_OES             0x8C36
#define GL_MIN_SAMPLE_SHADING_VALUE_OES   0x8C37
#define glMinSampleShadingOES NvRemap_glMinSampleShadingOES
#endif /* GL_OES_sample_shading */

#ifndef GL_OES_sample_variables
#define GL_OES_sample_variables 1
#endif /* GL_OES_sample_variables */

#ifndef GL_OES_shader_image_atomic
#define GL_OES_shader_image_atomic 1
#endif /* GL_OES_shader_image_atomic */

#ifndef GL_OES_shader_multisample_interpolation
#define GL_OES_shader_multisample_interpolation 1
#define GL_MIN_FRAGMENT_INTERPOLATION_OFFSET_OES 0x8E5B
#define GL_MAX_FRAGMENT_INTERPOLATION_OFFSET_OES 0x8E5C
#define GL_FRAGMENT_INTERPOLATION_OFFSET_BITS_OES 0x8E5D
#endif /* GL_OES_shader_multisample_interpolation */

#ifndef GL_OES_texture_stencil8
#define GL_OES_texture_stencil8 1
#define GL_STENCIL_INDEX_OES              0x1901
#define GL_STENCIL_INDEX8_OES             0x8D48
#endif /* GL_OES_texture_stencil8 */

#ifndef GL_OES_texture_storage_multisample_2d_array
#define GL_OES_texture_storage_multisample_2d_array 1
#define GL_TEXTURE_2D_MULTISAMPLE_ARRAY_OES 0x9102
#define GL_TEXTURE_BINDING_2D_MULTISAMPLE_ARRAY_OES 0x9105
#define GL_SAMPLER_2D_MULTISAMPLE_ARRAY_OES 0x910B
#define GL_INT_SAMPLER_2D_MULTISAMPLE_ARRAY_OES 0x910C
#define GL_UNSIGNED_INT_SAMPLER_2D_MULTISAMPLE_ARRAY_OES 0x910D
#define glTexStorage3DMultisampleOES NvRemap_glTexStorage3DMultisampleOES
#endif /* GL_OES_texture_storage_multisample_2d_array */

#ifndef GL_EXT_copy_image
#define GL_EXT_copy_image 1
typedef int GLint;
#define glCopyImageSubDataEXT NvRemap_glCopyImageSubDataEXT
#endif /* GL_EXT_copy_image */

#ifndef GL_EXT_draw_buffers_indexed
#define GL_EXT_draw_buffers_indexed 1
#define GL_BLEND_EQUATION_RGB             0x8009
#define GL_BLEND_EQUATION_ALPHA           0x883D
#define GL_BLEND_SRC_RGB                  0x80C9
#define GL_BLEND_SRC_ALPHA                0x80CB
#define GL_BLEND_DST_RGB                  0x80C8
#define GL_BLEND_DST_ALPHA                0x80CA
#define GL_COLOR_WRITEMASK                0x0C23
#define GL_BLEND                          0x0BE2
#define GL_FUNC_ADD                       0x8006
#define GL_FUNC_SUBTRACT                  0x800A
#define GL_FUNC_REVERSE_SUBTRACT          0x800B
#define GL_MIN                            0x8007
#define GL_MAX                            0x8008
#define GL_ZERO                           0
#define GL_ONE                            1
#define GL_SRC_COLOR                      0x0300
#define GL_ONE_MINUS_SRC_COLOR            0x0301
#define GL_DST_COLOR                      0x0306
#define GL_ONE_MINUS_DST_COLOR            0x0307
#define GL_SRC_ALPHA                      0x0302
#define GL_ONE_MINUS_SRC_ALPHA            0x0303
#define GL_DST_ALPHA                      0x0304
#define GL_ONE_MINUS_DST_ALPHA            0x0305
#define GL_CONSTANT_COLOR                 0x8001
#define GL_ONE_MINUS_CONSTANT_COLOR       0x8002
#define GL_CONSTANT_ALPHA                 0x8003
#define GL_ONE_MINUS_CONSTANT_ALPHA       0x8004
#define GL_SRC_ALPHA_SATURATE             0x0308
#define glEnableiEXT NvRemap_glEnableiEXT
#define glDisableiEXT NvRemap_glDisableiEXT
#define glBlendEquationiEXT NvRemap_glBlendEquationiEXT
#define glBlendEquationSeparateiEXT NvRemap_glBlendEquationSeparateiEXT
#define glBlendFunciEXT NvRemap_glBlendFunciEXT
#define glBlendFuncSeparateiEXT NvRemap_glBlendFuncSeparateiEXT
#define glColorMaskiEXT NvRemap_glColorMaskiEXT
#define glIsEnablediEXT NvRemap_glIsEnablediEXT
#endif /* GL_EXT_draw_buffers_indexed */

#ifndef GL_EXT_geometry_shader
#define GL_EXT_geometry_shader 1
#define GL_GEOMETRY_SHADER_EXT            0x8DD9
#define GL_GEOMETRY_SHADER_BIT_EXT        0x00000004
#define GL_GEOMETRY_LINKED_VERTICES_OUT_EXT 0x8916
#define GL_GEOMETRY_LINKED_INPUT_TYPE_EXT 0x8917
#define GL_GEOMETRY_LINKED_OUTPUT_TYPE_EXT 0x8918
#define GL_GEOMETRY_SHADER_INVOCATIONS_EXT 0x887F
#define GL_LAYER_PROVOKING_VERTEX_EXT     0x825E
#define GL_LINES_ADJACENCY_EXT            0x000A
#define GL_LINE_STRIP_ADJACENCY_EXT       0x000B
#define GL_TRIANGLES_ADJACENCY_EXT        0x000C
#define GL_TRIANGLE_STRIP_ADJACENCY_EXT   0x000D
#define GL_MAX_GEOMETRY_UNIFORM_COMPONENTS_EXT 0x8DDF
#define GL_MAX_GEOMETRY_UNIFORM_BLOCKS_EXT 0x8A2C
#define GL_MAX_COMBINED_GEOMETRY_UNIFORM_COMPONENTS_EXT 0x8A32
#define GL_MAX_GEOMETRY_INPUT_COMPONENTS_EXT 0x9123
#define GL_MAX_GEOMETRY_OUTPUT_COMPONENTS_EXT 0x9124
#define GL_MAX_GEOMETRY_OUTPUT_VERTICES_EXT 0x8DE0
#define GL_MAX_GEOMETRY_TOTAL_OUTPUT_COMPONENTS_EXT 0x8DE1
#define GL_MAX_GEOMETRY_SHADER_INVOCATIONS_EXT 0x8E5A
#define GL_MAX_GEOMETRY_TEXTURE_IMAGE_UNITS_EXT 0x8C29
#define GL_MAX_GEOMETRY_ATOMIC_COUNTER_BUFFERS_EXT 0x92CF
#define GL_MAX_GEOMETRY_ATOMIC_COUNTERS_EXT 0x92D5
#define GL_MAX_GEOMETRY_IMAGE_UNIFORMS_EXT 0x90CD
#define GL_MAX_GEOMETRY_SHADER_STORAGE_BLOCKS_EXT 0x90D7
#define GL_FIRST_VERTEX_CONVENTION_EXT    0x8E4D
#define GL_LAST_VERTEX_CONVENTION_EXT     0x8E4E
#define GL_UNDEFINED_VERTEX_EXT           0x8260
#define GL_PRIMITIVES_GENERATED_EXT       0x8C87
#define GL_FRAMEBUFFER_DEFAULT_LAYERS_EXT 0x9312
#define GL_MAX_FRAMEBUFFER_LAYERS_EXT     0x9317
#define GL_FRAMEBUFFER_INCOMPLETE_LAYER_TARGETS_EXT 0x8DA8
#define GL_FRAMEBUFFER_ATTACHMENT_LAYERED_EXT 0x8DA7
#define GL_REFERENCED_BY_GEOMETRY_SHADER_EXT 0x9309
#define glFramebufferTextureEXT NvRemap_glFramebufferTextureEXT
#endif /* GL_EXT_geometry_shader */

#ifndef GL_EXT_geometry_shader4
#define GL_EXT_geometry_shader4 1
#define GL_GEOMETRY_VERTICES_OUT_EXT      0x8DDA
#define GL_GEOMETRY_INPUT_TYPE_EXT        0x8DDB
#define GL_GEOMETRY_OUTPUT_TYPE_EXT       0x8DDC
#define GL_MAX_GEOMETRY_VARYING_COMPONENTS_EXT 0x8DDD
#define GL_MAX_VERTEX_VARYING_COMPONENTS_EXT 0x8DDE
#define GL_MAX_VARYING_COMPONENTS_EXT     0x8B4B
#define GL_FRAMEBUFFER_INCOMPLETE_LAYER_COUNT_EXT 0x8DA9
#define GL_FRAMEBUFFER_ATTACHMENT_TEXTURE_LAYER_EXT 0x8CD4
#define GL_PROGRAM_POINT_SIZE_EXT         0x8642
#define glProgramParameteriEXT NvRemap_glProgramParameteriEXT
#endif /* GL_EXT_geometry_shader4 */

#ifndef GL_EXT_gpu_shader5
#define GL_EXT_gpu_shader5 1
#endif /* GL_EXT_gpu_shader5 */

#ifndef GL_EXT_primitive_bounding_box
#define GL_EXT_primitive_bounding_box 1
#define GL_PRIMITIVE_BOUNDING_BOX_EXT     0x92BE
#define glPrimitiveBoundingBoxEXT NvRemap_glPrimitiveBoundingBoxEXT
#endif /* GL_EXT_primitive_bounding_box */

#ifndef GL_EXT_shader_io_blocks
#define GL_EXT_shader_io_blocks 1
#endif /* GL_EXT_shader_io_blocks */

#ifndef GL_EXT_tessellation_shader
#define GL_EXT_tessellation_shader 1
#define GL_PATCHES_EXT                    0x000E
#define GL_PATCH_VERTICES_EXT             0x8E72
#define GL_TESS_CONTROL_OUTPUT_VERTICES_EXT 0x8E75
#define GL_TESS_GEN_MODE_EXT              0x8E76
#define GL_TESS_GEN_SPACING_EXT           0x8E77
#define GL_TESS_GEN_VERTEX_ORDER_EXT      0x8E78
#define GL_TESS_GEN_POINT_MODE_EXT        0x8E79
#define GL_TRIANGLES                      0x0004
#define GL_ISOLINES_EXT                   0x8E7A
#define GL_QUADS_EXT                      0x0007
#define GL_EQUAL                          0x0202
#define GL_FRACTIONAL_ODD_EXT             0x8E7B
#define GL_FRACTIONAL_EVEN_EXT            0x8E7C
#define GL_CCW                            0x0901
#define GL_CW                             0x0900
#define GL_MAX_PATCH_VERTICES_EXT         0x8E7D
#define GL_MAX_TESS_GEN_LEVEL_EXT         0x8E7E
#define GL_MAX_TESS_CONTROL_UNIFORM_COMPONENTS_EXT 0x8E7F
#define GL_MAX_TESS_EVALUATION_UNIFORM_COMPONENTS_EXT 0x8E80
#define GL_MAX_TESS_CONTROL_TEXTURE_IMAGE_UNITS_EXT 0x8E81
#define GL_MAX_TESS_EVALUATION_TEXTURE_IMAGE_UNITS_EXT 0x8E82
#define GL_MAX_TESS_CONTROL_OUTPUT_COMPONENTS_EXT 0x8E83
#define GL_MAX_TESS_PATCH_COMPONENTS_EXT  0x8E84
#define GL_MAX_TESS_CONTROL_TOTAL_OUTPUT_COMPONENTS_EXT 0x8E85
#define GL_MAX_TESS_EVALUATION_OUTPUT_COMPONENTS_EXT 0x8E86
#define GL_MAX_TESS_CONTROL_UNIFORM_BLOCKS_EXT 0x8E89
#define GL_MAX_TESS_EVALUATION_UNIFORM_BLOCKS_EXT 0x8E8A
#define GL_MAX_TESS_CONTROL_INPUT_COMPONENTS_EXT 0x886C
#define GL_MAX_TESS_EVALUATION_INPUT_COMPONENTS_EXT 0x886D
#define GL_MAX_COMBINED_TESS_CONTROL_UNIFORM_COMPONENTS_EXT 0x8E1E
#define GL_MAX_COMBINED_TESS_EVALUATION_UNIFORM_COMPONENTS_EXT 0x8E1F
#define GL_MAX_TESS_CONTROL_ATOMIC_COUNTER_BUFFERS_EXT 0x92CD
#define GL_MAX_TESS_EVALUATION_ATOMIC_COUNTER_BUFFERS_EXT 0x92CE
#define GL_MAX_TESS_CONTROL_ATOMIC_COUNTERS_EXT 0x92D3
#define GL_MAX_TESS_EVALUATION_ATOMIC_COUNTERS_EXT 0x92D4
#define GL_MAX_TESS_CONTROL_IMAGE_UNIFORMS_EXT 0x90CB
#define GL_MAX_TESS_EVALUATION_IMAGE_UNIFORMS_EXT 0x90CC
#define GL_MAX_TESS_CONTROL_SHADER_STORAGE_BLOCKS_EXT 0x90D8
#define GL_MAX_TESS_EVALUATION_SHADER_STORAGE_BLOCKS_EXT 0x90D9
#define GL_PRIMITIVE_RESTART_FOR_PATCHES_SUPPORTED 0x8221
#define GL_IS_PER_PATCH_EXT               0x92E7
#define GL_REFERENCED_BY_TESS_CONTROL_SHADER_EXT 0x9307
#define GL_REFERENCED_BY_TESS_EVALUATION_SHADER_EXT 0x9308
#define GL_TESS_CONTROL_SHADER_EXT        0x8E88
#define GL_TESS_EVALUATION_SHADER_EXT     0x8E87
#define GL_TESS_CONTROL_SHADER_BIT_EXT    0x00000008
#define GL_TESS_EVALUATION_SHADER_BIT_EXT 0x00000010
#define glPatchParameteriEXT NvRemap_glPatchParameteriEXT
#endif /* GL_EXT_tessellation_shader */

#ifndef GL_EXT_texture_border_clamp
#define GL_EXT_texture_border_clamp 1
#define GL_TEXTURE_BORDER_COLOR_EXT       0x1004
#define GL_CLAMP_TO_BORDER_EXT            0x812D
#define glTexParameterIivEXT NvRemap_glTexParameterIivEXT
#define glTexParameterIuivEXT NvRemap_glTexParameterIuivEXT
#define glGetTexParameterIivEXT NvRemap_glGetTexParameterIivEXT
#define glGetTexParameterIuivEXT NvRemap_glGetTexParameterIuivEXT
#define glSamplerParameterIivEXT NvRemap_glSamplerParameterIivEXT
#define glSamplerParameterIuivEXT NvRemap_glSamplerParameterIuivEXT
#define glGetSamplerParameterIivEXT NvRemap_glGetSamplerParameterIivEXT
#define glGetSamplerParameterIuivEXT NvRemap_glGetSamplerParameterIuivEXT
#endif /* GL_EXT_texture_border_clamp */

#ifndef GL_EXT_texture_buffer
#define GL_EXT_texture_buffer 1
typedef khronos_intptr_t GLintptr;
typedef khronos_ssize_t GLsizeiptr;
#define GL_TEXTURE_BUFFER_EXT             0x8C2A
#define GL_TEXTURE_BUFFER_BINDING_EXT     0x8C2A
#define GL_MAX_TEXTURE_BUFFER_SIZE_EXT    0x8C2B
#define GL_TEXTURE_BINDING_BUFFER_EXT     0x8C2C
#define GL_TEXTURE_BUFFER_DATA_STORE_BINDING_EXT 0x8C2D
#define GL_TEXTURE_BUFFER_OFFSET_ALIGNMENT_EXT 0x919F
#define GL_SAMPLER_BUFFER_EXT             0x8DC2
#define GL_INT_SAMPLER_BUFFER_EXT         0x8DD0
#define GL_UNSIGNED_INT_SAMPLER_BUFFER_EXT 0x8DD8
#define GL_IMAGE_BUFFER_EXT               0x9051
#define GL_INT_IMAGE_BUFFER_EXT           0x905C
#define GL_UNSIGNED_INT_IMAGE_BUFFER_EXT  0x9067
#define GL_TEXTURE_BUFFER_OFFSET_EXT      0x919D
#define GL_TEXTURE_BUFFER_SIZE_EXT        0x919E
#define glTexBufferEXT NvRemap_glTexBufferEXT
#define glTexBufferRangeEXT NvRemap_glTexBufferRangeEXT
#endif /* GL_EXT_texture_buffer */

#ifndef GL_EXT_texture_buffer_object
#define GL_EXT_texture_buffer_object 1
#define GL_TEXTURE_BUFFER_FORMAT_EXT      0x8C2E
#endif /* GL_EXT_texture_buffer_object */

#ifndef GL_EXT_texture_cube_map_array
#define GL_EXT_texture_cube_map_array 1
#define GL_TEXTURE_CUBE_MAP_ARRAY_EXT     0x9009
#define GL_TEXTURE_BINDING_CUBE_MAP_ARRAY_EXT 0x900A
#define GL_SAMPLER_CUBE_MAP_ARRAY_EXT     0x900C
#define GL_SAMPLER_CUBE_MAP_ARRAY_SHADOW_EXT 0x900D
#define GL_INT_SAMPLER_CUBE_MAP_ARRAY_EXT 0x900E
#define GL_UNSIGNED_INT_SAMPLER_CUBE_MAP_ARRAY_EXT 0x900F
#define GL_IMAGE_CUBE_MAP_ARRAY_EXT       0x9054
#define GL_INT_IMAGE_CUBE_MAP_ARRAY_EXT   0x905F
#define GL_UNSIGNED_INT_IMAGE_CUBE_MAP_ARRAY_EXT 0x906A
#endif /* GL_EXT_texture_cube_map_array */

#ifdef __cplusplus
}
#endif

#endif
