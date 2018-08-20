//----------------------------------------------------------------------------------
// File:        NvVkUtil/NvUIShadersVK.cpp
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
#include "NvUIShadersVK.h"
#include "NvUIVK.h"
#include "NvVkUtil/NvVkContext.h"
#include "NV/NvLogs.h"

#define USE_SOURCE_SHADERS 0

#if USE_SOURCE_SHADERS
// cannot be const, as the processing changes it
static char s_graphicShader[] =
"#GLSL_VS\n"
"#version 440 core\n"
"\n"
"// this is set from higher level.  think of it as the upper model matrix\n"
"layout(binding = 0) uniform Block {\n"
"    mat4 pixelToClipMat;\n"
"    vec4 color;\n"
"    vec2 thickness;\n"
"    vec2 texBorder;\n"
"    float alpha;\n"
"};\n"
"layout(location = 0) in vec2 position;\n"
"layout(location = 1) in vec2 tex;\n"
"out IO { vec2 tex_coord; };\n"
"void main()\n"
"{\n"
"    gl_Position = pixelToClipMat * vec4(position, 0, 1);\n"
"    tex_coord = tex;\n"
"}\n"
"#GLSL_FS\n"
"#version 440 core\n"
"\n"
"// this is set from higher level.  think of it as the upper model matrix\n"
"layout(binding = 0) uniform Block {\n"
"    mat4 pixelToClipMat;\n"
"    vec4 color;\n"
"    vec2 thickness;\n"
"    vec2 texBorder;\n"
"    float alpha;\n"
"};\n"
"in IO { vec2 tex_coord; };\n"
"layout(binding = 1) uniform sampler2D sampler;\n"
"layout(location = 0) out vec4 fragColor;\n"
"void main()\n"
"{\n"
"    fragColor = texture2D(sampler, tex_coord) * vec4(color.r,color.g,color.b,alpha);\n"
"}\n";
#else
#include "shaders/graphic.cpp"
#endif

//======================================================================
//======================================================================
void NvGraphicShaderVK::Load()
{
	NvVkContext& vk = *NvUIVKctx().mVk;
	uint32_t stageCount = 0;
#if USE_SOURCE_SHADERS
	stageCount = vk.createShadersFromSourceString(s_graphicShader, mStages, MAX_STAGES);
#else
	stageCount = vk.createShadersFromBinaryBlob((uint32_t*)graphicData, graphicLength, mStages, MAX_STAGES);
#endif
}

//======================================================================
// ----- NvUIGraphicFrame -----
//======================================================================

#if USE_SOURCE_SHADERS
// cannot be const, as the processing changes it
static char s_frameShader[] =
"#GLSL_VS\n"
"#version 440 core\n"
"\n"
"// this is set from higher level.  think of it as the upper model matrix\n"
"layout(binding = 0) uniform Block {\n"
"    mat4 pixelToClipMat;\n"
"    vec4 color;\n"
"    vec2 thickness;\n"
"    vec2 texBorder;\n"
"    float alpha;\n"
"};\n"
"layout(location = 0) in vec2 position;\n"
"layout(location = 1) in vec2 tex;\n"
"layout(location = 2) in vec2 border;\n"
"out IO { vec2 tex_coord; };\n"
"void main()\n"
"{\n"
"    vec2 invBorder = vec2(1,1) - border;\n"
"    vec2 shiftedPosition = (position-thickness*invBorder*position);\n"
"    // we need to convert from -1,1 coords into 0,1 coords before xform.\n"
"    shiftedPosition *= 0.5;\n"
"    shiftedPosition += 0.5;\n"
"    // then we multiply like uigraphic normally would\n"
"    gl_Position = pixelToClipMat * vec4(shiftedPosition, 0, 1);\n"
"    tex_coord = tex + invBorder * -position * texBorder;\n"
"}\n"
"#GLSL_FS\n"
"#version 440 core\n"
"\n"
"// this is set from higher level.  think of it as the upper model matrix\n"
"layout(binding = 0) uniform Block {\n"
"    mat4 pixelToClipMat;\n"
"    vec4 color;\n"
"    vec2 thickness;\n"
"    vec2 texBorder;\n"
"    float alpha;\n"
"};\n"
"in IO { vec2 tex_coord; };\n"
"layout(binding = 1) uniform sampler2D sampler;\n"
"layout(location = 0) out vec4 fragColor;\n"
"void main()\n"
"{\n"
"    gl_FragColor = texture2D(sampler, tex_coord) * vec4(color.r,color.g,color.b,alpha);\n"
"}\n";
#else
#include "shaders/graphicFrame.cpp"
#endif


//======================================================================
//======================================================================
void NvGraphicFrameShaderVK::Load()
{
	NvVkContext& vk = *NvUIVKctx().mVk;
	uint32_t stageCount = 0;
#if USE_SOURCE_SHADERS
	mStageCount = vk.createShadersFromSourceString(s_frameShader, mStages, MAX_STAGES);
#else
	mStageCount = vk.createShadersFromBinaryBlob((uint32_t*)graphicFrameData, graphicFrameLength, mStages, MAX_STAGES);
#endif
}

