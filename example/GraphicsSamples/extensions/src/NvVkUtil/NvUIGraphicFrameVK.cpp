//----------------------------------------------------------------------------------
// File:        NvVkUtil/NvUIGraphicFrameVK.cpp
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
/*
 * library for implementing base classes for NV UI framework
 * used for building Widgets and Applications on common code.
 */

#include "NvUI/NvUI.h"

#include "NvAssetLoader/NvAssetLoader.h"

#include "NvUIVK.h"
#include "NvUITextureRenderVK.h"

#include <math.h>




int32_t NvUIGraphicFrameRenderVK::ms_staticCount = 0;

NvGraphicFrameShaderVK NvUIGraphicFrameRenderVK::ms_shader;
VkPipeline NvUIGraphicFrameRenderVK::ms_pipelineAlpha;
VkPipeline NvUIGraphicFrameRenderVK::ms_pipelineOpaque;

//======================================================================
//======================================================================
NvUIGraphicFrameRender* NvUIGraphicFrameRenderVK::Create(NvUIGraphicFrame* graphic) {
    return new NvUIGraphicFrameRenderVK(graphic);
}

NvUIGraphicFrameRenderVK::NvUIGraphicFrameRenderVK(NvUIGraphicFrame* graphic)
{
	NvVkContext& vk = *NvUIVKctx().mVk;
	m_graphic = graphic;
    StaticInit();

	VkResult result;
	VkDescriptorSetAllocateInfo descriptorSetAllocateInfo = { VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO };
	descriptorSetAllocateInfo.descriptorPool = ms_descriptorPool;
	descriptorSetAllocateInfo.descriptorSetCount = 1;
	descriptorSetAllocateInfo.pSetLayouts = &ms_descriptorSetLayout;
	result = vkAllocateDescriptorSets(vk.device(), &descriptorSetAllocateInfo, &m_descriptorSet);
	CHECK_VK_RESULT();

	VkDescriptorBufferInfo uboDesc;
	ms_ubo.GetDesc(uboDesc);

	VkDescriptorImageInfo texDesc = {};
	texDesc.imageLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
	texDesc.imageView = ((NvUITextureRenderVK*)(m_graphic->GetTex()->GetRender()))->mTex.view;
	texDesc.sampler = ms_sampler;

	VkWriteDescriptorSet writeDescriptorSets[2];
	writeDescriptorSets[0] = { VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET };
	writeDescriptorSets[0].dstSet = m_descriptorSet;
	writeDescriptorSets[0].dstBinding = 0;
	writeDescriptorSets[0].dstArrayElement = 0;
	writeDescriptorSets[0].descriptorCount = 1;
	writeDescriptorSets[0].descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER_DYNAMIC;
	writeDescriptorSets[0].pBufferInfo = &uboDesc;
	writeDescriptorSets[1] = { VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET };
	writeDescriptorSets[1].dstSet = m_descriptorSet;
	writeDescriptorSets[1].dstBinding = 1;
	writeDescriptorSets[1].dstArrayElement = 0;
	writeDescriptorSets[1].descriptorCount = 1;
	writeDescriptorSets[1].descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
	writeDescriptorSets[1].pImageInfo = &texDesc;

	vkUpdateDescriptorSets(vk.device(), 2, writeDescriptorSets, 0, 0);
}

NvUIGraphicFrameRenderVK::~NvUIGraphicFrameRenderVK()
{
    StaticCleanup();
}

bool NvUIGraphicFrameRenderVK::StaticCleanup()
{
    if (--ms_staticCount == 0)
    {
    }

    return true;
}

//======================================================================
//======================================================================
bool NvUIGraphicFrameRenderVK::StaticInit()
{

	//NvUIGraphicRenderVK::StaticInit();

	if (!ms_staticCount)
    {
       ms_shader.Load();

	   VkPipelineColorBlendAttachmentState colorStateBlend = { VK_STRUCTURE_TYPE_PIPELINE_COLOR_BLEND_STATE_CREATE_INFO };
	   colorStateBlend.colorWriteMask = VK_COLOR_COMPONENT_R_BIT | VK_COLOR_COMPONENT_G_BIT | VK_COLOR_COMPONENT_B_BIT | VK_COLOR_COMPONENT_A_BIT;
	   colorStateBlend.blendEnable = VK_TRUE;
	   colorStateBlend.alphaBlendOp = VK_BLEND_OP_ADD;
	   colorStateBlend.colorBlendOp = VK_BLEND_OP_ADD;
	   colorStateBlend.srcAlphaBlendFactor = VK_BLEND_FACTOR_SRC_ALPHA;
	   colorStateBlend.dstAlphaBlendFactor = VK_BLEND_FACTOR_ONE_MINUS_SRC_ALPHA;
	   colorStateBlend.srcColorBlendFactor = VK_BLEND_FACTOR_SRC_ALPHA;
	   colorStateBlend.dstColorBlendFactor = VK_BLEND_FACTOR_ONE_MINUS_SRC_ALPHA;

	   VkPipelineColorBlendStateCreateInfo colorInfoBlend = { VK_STRUCTURE_TYPE_PIPELINE_COLOR_BLEND_STATE_CREATE_INFO };
	   colorInfoBlend.logicOpEnable = VK_FALSE;
	   colorInfoBlend.attachmentCount = 1;
	   colorInfoBlend.pAttachments = &colorStateBlend;
	   colorInfoBlend.blendConstants[0] = 1.0f;
	   colorInfoBlend.blendConstants[1] = 1.0f;
	   colorInfoBlend.blendConstants[2] = 1.0f;
	   colorInfoBlend.blendConstants[3] = 1.0f;

	   InitPipeline(ms_shader.mStageCount,
		   ms_shader.mStages, &colorInfoBlend,
		   &ms_pipelineAlpha);

	   colorStateBlend.blendEnable = VK_FALSE;
	   InitPipeline(ms_shader.mStageCount,
		   ms_shader.mStages, &colorInfoBlend,
		   &ms_pipelineOpaque);
	}

	ms_staticCount++;

	return true;
}

//======================================================================
//======================================================================
void NvUIGraphicFrameRenderVK::Draw(float alpha, const NvPackedColor& color, const float pixelToClipMatrix[4][4],
                            const nv::vec2<float>& thickness, const nv::vec2<float>& texBorder, bool drawCenter)
{
	NvVkContext& vk = *NvUIVKctx().mVk;
	VkCommandBuffer& cmd = NvUIVKctx().mCmd;

	// Set transform
	// pixelToClipMatrix
	memcpy(&(ms_ubo->pixelToClipMat), pixelToClipMatrix, 16 * sizeof(float));
	ms_ubo->pixelToClipMat(1, 1) *= -1.0f;
	ms_ubo->pixelToClipMat(1, 3) *= -1.0f;
	ms_ubo->alpha = alpha;
	ms_ubo->color = nv::vec4f(NV_PC_RED_FLOAT(color),
		NV_PC_GREEN_FLOAT(color), NV_PC_BLUE_FLOAT(color), 1.0f);

	NvUITexture* tex = m_graphic->GetTex();

	ms_ubo->thickness = thickness;
	ms_ubo->texBorder.x = texBorder.x / tex->GetWidth();
	ms_ubo->texBorder.y = texBorder.y / tex->GetHeight();
	ms_ubo.Update();
	m_uboOffset = ms_ubo.getDynamicOffset();

	VkViewport vp;
	VkRect2D sc;
	vp.x = 0;
	vp.y = 0;
	vp.height = (float)(vk.mainRenderTarget()->height());
	vp.width = (float)(vk.mainRenderTarget()->width());
	vp.minDepth = 0.0f;
	vp.maxDepth = 1.0f;

	sc.offset.x = 0;
	sc.offset.y = 0;
	sc.extent.width = vp.width;
	sc.extent.height = vp.height;

	vkCmdSetViewport(cmd, 0, 1, &vp);
	vkCmdSetScissor(cmd, 0, 1, &sc);

	if (m_graphic->GetTex()->GetHasAlpha() || (alpha < 1.0f)) {
		vkCmdBindPipeline(cmd, VK_PIPELINE_BIND_POINT_GRAPHICS, ms_pipelineAlpha);
	}
	else {
		vkCmdBindPipeline(cmd, VK_PIPELINE_BIND_POINT_GRAPHICS, ms_pipelineOpaque);
	}

	vkCmdBindDescriptorSets(cmd, VK_PIPELINE_BIND_POINT_GRAPHICS, ms_pipelineLayout, 0, 1, &m_descriptorSet, 1, &m_uboOffset);

	// Bind the vertex and index buffers
	VkDeviceSize offsets[] = { 0 };
	vkCmdBindVertexBuffers(cmd, 0, 1, &ms_vbo(), offsets);
	vkCmdBindIndexBuffer(cmd, ms_ibo(), 0, VK_INDEX_TYPE_UINT16);

	// Draw the triangle
	vkCmdDrawIndexed(cmd, 30 + 6, 1, 0, 0, 0);
}
