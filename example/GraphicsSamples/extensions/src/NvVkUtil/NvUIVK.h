//----------------------------------------------------------------------------------
// File:        NvVkUtil/NvUIVK.h
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
#ifndef NV_UI_VK_H
#define NV_UI_VK_H

#include <NvUI/NvUI.h>
#include "NvImage/NvImage.h"
#include "NV/NvLogs.h"

#include "NvVkUtil/NvVkUtil.h"
#include "NvVkUtil/NvSimpleUBO.h"
#include "NvUIShadersVK.h"

class NvVkContext;
class NvUITextureRenderVK;

class NvUIGraphicRenderBaseVK {
public:
	NvUIGraphicRenderBaseVK();
	~NvUIGraphicRenderBaseVK();

	static bool StaticInit();
	static bool StaticCleanup();
	static void InitPipeline(uint32_t shaderCount,
		VkPipelineShaderStageCreateInfo* shaderStages,
		VkPipelineColorBlendStateCreateInfo* pBlendState,
		VkPipeline* pipeline);

	static NvSimpleUBO<NvGraphicUBO, 1024> ms_ubo;

	static NvVkBuffer ms_vbo;
	static NvVkBuffer ms_ibo;
	static uint32_t ms_indexCount;
	static VkPipelineLayout ms_pipelineLayout;

	static VkVertexInputAttributeDescription ms_attributes[3];
	static VkVertexInputBindingDescription ms_vertexBindings;

	static VkPipelineVertexInputStateCreateInfo ms_viStateInfo;
	static VkPipelineInputAssemblyStateCreateInfo ms_iaStateInfo;

	static VkDescriptorPool ms_descriptorPool;
	static VkDescriptorSetLayout ms_descriptorSetLayout;

	VkDescriptorSet m_descriptorSet;
	uint32_t m_uboOffset;

	static VkSampler ms_sampler;
};

class NvUIGraphicRenderVK : public NvUIGraphicRender, public NvUIGraphicRenderBaseVK {
public:
    static NvUIGraphicRender* Create(NvUIGraphic* graphic);

    NvUIGraphicRenderVK(NvUIGraphic* graphic);
    virtual ~NvUIGraphicRenderVK();
    virtual void Draw(float alpha, const NvPackedColor& color, const float pixelToClipMatrix[4][4]);

    NvUITextureRenderVK* GetTex() { return (NvUITextureRenderVK*)(m_graphic->GetTex()->GetRender()); }
	static bool StaticInit();
	static bool StaticCleanup();

    NvUIGraphic* m_graphic;

	static int32_t ms_initCount;

	static NvGraphicShaderVK ms_shader;
	static VkPipeline ms_pipelineAlpha;
	static VkPipeline ms_pipelineOpaque;
};


class NvUIGraphicFrameRenderVK : public NvUIGraphicFrameRender, public NvUIGraphicRenderBaseVK {
public:
    static NvUIGraphicFrameRender* Create(NvUIGraphicFrame* graphic);

    NvUIGraphicFrameRenderVK(NvUIGraphicFrame* graphic);
    virtual ~NvUIGraphicFrameRenderVK();
    virtual void Draw(float alpha, const NvPackedColor& color, const float pixelToClipMatrix[4][4],
        const nv::vec2<float>& thickness, const nv::vec2<float>& texBorder, bool drawCenter);

    static bool StaticInit();
	static bool StaticCleanup();

    NvUIGraphicFrame* m_graphic;

    static int32_t ms_staticCount;

	static NvGraphicFrameShaderVK ms_shader;
	static VkPipeline ms_pipelineAlpha;
	static VkPipeline ms_pipelineOpaque;
};

class NvUIDrawCtx {
public:
	NvVkContext* mVk;
	VkCommandBuffer mCmd;
};

extern NvUIDrawCtx& NvUIVKctx();


#endif
