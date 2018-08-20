//----------------------------------------------------------------------------------
// File:        NvVkUtil/NvUIShadersVK.h
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
#ifndef NV_UI_SHADERS_VK_H
#define NV_UI_SHADERS_VK_H

#include "NvUI/NvUI.h"
#include "NV/NvMath.h"
#include "NvVkUtil/NvVkUtil.h"

typedef struct
{
	float x, y;     // position
	float s, t;     // texture coordinate
	float bx, by;   // whether or not this vertex is a border vertex in x and y
} NvGraphicVertex;

typedef struct
{
    nv::matrix4f pixelToClipMat;
    nv::vec4f color;
    nv::vec2f thickness;
    nv::vec2f texBorder;
    float alpha;
} NvGraphicUBO;

//=============================================================================
//=============================================================================
/** A helper object for managing a shader program for NvUIGraphic and subclasses. */
class NvGraphicShaderVK
{
public:
    /** Helper for compiling the given shader strings and then retrieving indicies. */
    virtual void Load();

	enum { MAX_STAGES = 2 };

	VkPipelineShaderStageCreateInfo mStages[MAX_STAGES];
	uint32_t mStageCount;
};

//=============================================================================
//=============================================================================
/** This adds NvUIGraphicFrame features to the NvGraphicShader 'template'. */
class NvGraphicFrameShaderVK : public NvGraphicShaderVK
{
private:
    INHERIT_FROM(NvGraphicShaderVK);

public:
    /** Helper for compiling the given shader strings and then retrieving indicies.
        Overrides the version in NvGraphicShader, calls the inherited version first,
        then retrieves our additional indices. */
    virtual void Load();
};


#endif
