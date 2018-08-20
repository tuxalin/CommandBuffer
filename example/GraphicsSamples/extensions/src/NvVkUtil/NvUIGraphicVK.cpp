//----------------------------------------------------------------------------------
// File:        NvVkUtil/NvUIGraphicVK.cpp
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
#include "NvVkUtil/NvVkContext.h"
#include "NvUITextureRenderVK.h"

#include <math.h>

//======================================================================
//======================================================================
NvUIGraphicRender* NvUIGraphicRenderVK::Create(NvUIGraphic* graphic) {
	return new NvUIGraphicRenderVK(graphic);
}

//======================================================================
//======================================================================
NvSimpleUBO<NvGraphicUBO, 1024> NvUIGraphicRenderBaseVK::ms_ubo;

NvVkBuffer NvUIGraphicRenderBaseVK::ms_vbo;
NvVkBuffer NvUIGraphicRenderBaseVK::ms_ibo;
VkPipelineLayout NvUIGraphicRenderBaseVK::ms_pipelineLayout;
VkDescriptorSetLayout NvUIGraphicRenderBaseVK::ms_descriptorSetLayout;

VkDescriptorPool NvUIGraphicRenderBaseVK::ms_descriptorPool;

VkSampler NvUIGraphicRenderBaseVK::ms_sampler;

VkVertexInputAttributeDescription NvUIGraphicRenderBaseVK::ms_attributes[3];
VkVertexInputBindingDescription NvUIGraphicRenderBaseVK::ms_vertexBindings;

VkPipelineVertexInputStateCreateInfo NvUIGraphicRenderBaseVK::ms_viStateInfo;
VkPipelineInputAssemblyStateCreateInfo NvUIGraphicRenderBaseVK::ms_iaStateInfo;

NvGraphicShaderVK NvUIGraphicRenderVK::ms_shader;
VkPipeline NvUIGraphicRenderVK::ms_pipelineAlpha;
VkPipeline NvUIGraphicRenderVK::ms_pipelineOpaque;

int32_t NvUIGraphicRenderVK::ms_initCount;


//======================================================================
// ----- NvUIGraphicRenderBaseVK -----
//======================================================================
NvUIGraphicRenderBaseVK::NvUIGraphicRenderBaseVK()
{
}

NvUIGraphicRenderBaseVK::~NvUIGraphicRenderBaseVK()
{
//    NvUIVKctx().mAlloc->free(m_ubo);
}

//======================================================================
//======================================================================
bool NvUIGraphicRenderBaseVK::StaticInit()
{
	NvVkContext& vk = *NvUIVKctx().mVk;
	VkResult result;

	NvGraphicVertex vert[4 * 4 + 4];
	NvGraphicVertex tempFrame[4] =
	{
		{ -1, 1, 0, 1, 0, 0 },
		{ 1, 1, 1, 1, 0, 0 },
		{ -1, -1, 0, 0, 0, 0 },
		{ 1, -1, 1, 0, 0, 0 }
	};
	NvGraphicVertex tempGraphic[4] =
	{
		{ 0, 1, 0, 1, 0, 0 },
		{ 0, 0, 0, 0, 0, 0 },
		{ 1, 0, 1, 0, 0, 0 },
		{ 1, 1, 1, 1, 0, 0 }
	};

	for (int32_t y = 0; y < 4; y++)
	{
		for (int32_t x = 0; x < 4; x++)
		{
			int32_t src = ((y >> 1) * 2) + (x >> 1);  //0,0,1,1,0,0,1,1,2,2,3,3,2,2,3,3
			int32_t dst = y * 4 + x;                    //0,1,2,3,4,5,6,7...
			memcpy(&vert[dst], &tempFrame[src], sizeof(NvGraphicVertex));
			if (y == 0 || y == 3)
				vert[dst].by = 1;
			if (x == 0 || x == 3)
				vert[dst].bx = 1;
		}
	}
	for (int32_t x = 0; x < 4; x++)
	{
		int32_t dst = 16 + x;
		memcpy(&vert[dst], &tempGraphic[x], sizeof(NvGraphicVertex));
	}

	uint16_t indices[30 + 6 + 4] = {
		0, 4, 1, 5, 2, 6, 3, 7, 7, // frame first row
		4, 4, 8, 5, 9, 9, // frame left panel
		6, 6, 10, 7, 11, 11, // frame right panel
		8, 8, 12, 9, 13, 10, 14, 11, 15, 15, // frame bottom
		5, 5, 9, 6, 10, // frame center piece
		16, 17, 19, 18 // graphic
	};

	if (!vk.createAndFillBuffer(20 * sizeof(NvGraphicVertex), VK_BUFFER_USAGE_VERTEX_BUFFER_BIT,
		VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT, ms_vbo, vert)) {

	}

	if (!vk.createAndFillBuffer((30 + 6 + 4) * sizeof(uint16_t), VK_BUFFER_USAGE_INDEX_BUFFER_BIT,
		VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT, ms_ibo, indices)) {

	}

	ms_ubo.Initialize(vk);

	// Create static state info for the mPipeline.
	ms_vertexBindings.binding = 0;
	ms_vertexBindings.stride = sizeof(NvGraphicVertex);
	ms_vertexBindings.inputRate = VK_VERTEX_INPUT_RATE_VERTEX;

	ms_attributes[0].location = 0;
	ms_attributes[0].binding = 0;
	ms_attributes[0].format = VK_FORMAT_R32G32_SFLOAT;
	ms_attributes[0].offset = offsetof(NvGraphicVertex, x);
	ms_attributes[1].location = 1;
	ms_attributes[1].binding = 0;
	ms_attributes[1].format = VK_FORMAT_R32G32_SFLOAT;
	ms_attributes[1].offset = offsetof(NvGraphicVertex, s);
	ms_attributes[2].location = 2;
	ms_attributes[2].binding = 0;
	ms_attributes[2].format = VK_FORMAT_R32G32_SFLOAT;
	ms_attributes[2].offset = offsetof(NvGraphicVertex, bx);

	// Create descriptor layout to match the shader resources
	VkDescriptorSetLayoutBinding bindings[2];
	bindings[0].binding = 0;
	bindings[0].descriptorCount = 1;
	bindings[0].stageFlags = VK_SHADER_STAGE_VERTEX_BIT | VK_SHADER_STAGE_FRAGMENT_BIT;
	bindings[0].descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER_DYNAMIC;
	bindings[0].pImmutableSamplers = NULL;
	bindings[1].binding = 1;
	bindings[1].descriptorCount = 1;
	bindings[1].stageFlags = VK_SHADER_STAGE_FRAGMENT_BIT;
    bindings[1].descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
	bindings[1].pImmutableSamplers = NULL;

	{
		VkDescriptorSetLayoutCreateInfo descriptorSetEntry = { VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO };
		descriptorSetEntry.bindingCount = 2;
		descriptorSetEntry.pBindings = bindings;

		result = vkCreateDescriptorSetLayout(vk.device(), &descriptorSetEntry, 0, &ms_descriptorSetLayout);
		CHECK_VK_RESULT();
	}

	{
		VkPipelineLayoutCreateInfo pipelineLayoutCreateInfo = { VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO };
		pipelineLayoutCreateInfo.setLayoutCount = 1;
		pipelineLayoutCreateInfo.pSetLayouts = &ms_descriptorSetLayout;
		result = vkCreatePipelineLayout(vk.device(), &pipelineLayoutCreateInfo, 0, &ms_pipelineLayout);
		CHECK_VK_RESULT();
	}

	// Create descriptor region and set
	VkDescriptorPoolSize descriptorPoolSizes[2];

	descriptorPoolSizes[0].type = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER_DYNAMIC;
	descriptorPoolSizes[0].descriptorCount = 1024;
	descriptorPoolSizes[1].type = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
	descriptorPoolSizes[1].descriptorCount = 1024;

	VkDescriptorPoolCreateInfo descriptorRegionInfo = { VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO };
	descriptorRegionInfo.maxSets = 1024;
	descriptorRegionInfo.poolSizeCount = sizeof descriptorPoolSizes / sizeof descriptorPoolSizes [0];
	descriptorRegionInfo.pPoolSizes = descriptorPoolSizes;
	result = vkCreateDescriptorPool(vk.device(), &descriptorRegionInfo, NULL, &ms_descriptorPool);
	CHECK_VK_RESULT();

	ms_viStateInfo = { VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO };
	ms_viStateInfo.vertexBindingDescriptionCount = 1;
	ms_viStateInfo.pVertexBindingDescriptions = &ms_vertexBindings;
	ms_viStateInfo.vertexAttributeDescriptionCount = 3;
	ms_viStateInfo.pVertexAttributeDescriptions = ms_attributes;

	ms_iaStateInfo = { VK_STRUCTURE_TYPE_PIPELINE_INPUT_ASSEMBLY_STATE_CREATE_INFO };
	ms_iaStateInfo.topology = VK_PRIMITIVE_TOPOLOGY_TRIANGLE_STRIP;
	ms_iaStateInfo.primitiveRestartEnable = VK_FALSE;


	VkSamplerCreateInfo samplerCreateInfo = { VK_STRUCTURE_TYPE_SAMPLER_CREATE_INFO };
	samplerCreateInfo.magFilter = VK_FILTER_LINEAR;
	samplerCreateInfo.minFilter = VK_FILTER_LINEAR;
	samplerCreateInfo.mipmapMode = VK_SAMPLER_MIPMAP_MODE_LINEAR;
	samplerCreateInfo.addressModeU = VK_SAMPLER_ADDRESS_MODE_CLAMP_TO_EDGE;
	samplerCreateInfo.addressModeV = VK_SAMPLER_ADDRESS_MODE_CLAMP_TO_EDGE;
	samplerCreateInfo.addressModeW = VK_SAMPLER_ADDRESS_MODE_CLAMP_TO_EDGE;
	samplerCreateInfo.mipLodBias = 0.0;
	samplerCreateInfo.maxAnisotropy = 1;
	samplerCreateInfo.compareOp = VK_COMPARE_OP_NEVER;
	samplerCreateInfo.minLod = 0.0;
	samplerCreateInfo.maxLod = 16.0;
	samplerCreateInfo.borderColor = VK_BORDER_COLOR_FLOAT_OPAQUE_WHITE;

	result = vkCreateSampler(vk.device(), &samplerCreateInfo, 0, &ms_sampler);

	return true;
}

void NvUIGraphicRenderBaseVK::InitPipeline(uint32_t shaderCount,
	VkPipelineShaderStageCreateInfo* shaderStages,
	VkPipelineColorBlendStateCreateInfo* pBlendState,
	VkPipeline* pipeline)
{
	NvVkContext& vk = *NvUIVKctx().mVk;
	VkResult result;
	// Create static state info for the mPipeline.

	// set dynamically
	VkPipelineViewportStateCreateInfo vpStateInfo = { VK_STRUCTURE_TYPE_PIPELINE_VIEWPORT_STATE_CREATE_INFO };
	vpStateInfo.pNext = 0;
	vpStateInfo.viewportCount = 1;
	vpStateInfo.scissorCount = 1;

	VkPipelineRasterizationStateCreateInfo rsStateInfo = { VK_STRUCTURE_TYPE_PIPELINE_RASTERIZATION_STATE_CREATE_INFO };
	rsStateInfo.depthClampEnable = VK_FALSE;
	rsStateInfo.rasterizerDiscardEnable = VK_FALSE;
	rsStateInfo.polygonMode = VK_POLYGON_MODE_FILL;
	rsStateInfo.cullMode = VK_CULL_MODE_NONE;
	rsStateInfo.frontFace = VK_FRONT_FACE_COUNTER_CLOCKWISE;
	rsStateInfo.lineWidth = 1.0f;
	VkPipelineDepthStencilStateCreateInfo noDepth;
	noDepth = { VK_STRUCTURE_TYPE_PIPELINE_DEPTH_STENCIL_STATE_CREATE_INFO };
	noDepth.depthTestEnable = VK_FALSE;
	noDepth.depthWriteEnable = VK_FALSE;
	noDepth.depthCompareOp = VK_COMPARE_OP_ALWAYS;
	noDepth.depthBoundsTestEnable = VK_FALSE;
	noDepth.stencilTestEnable = VK_FALSE;
	noDepth.minDepthBounds = 0.0f;
	noDepth.maxDepthBounds = 1.0f;

	VkPipelineMultisampleStateCreateInfo msStateInfo = { VK_STRUCTURE_TYPE_PIPELINE_MULTISAMPLE_STATE_CREATE_INFO };
	msStateInfo.rasterizationSamples = VK_SAMPLE_COUNT_1_BIT;
	msStateInfo.alphaToCoverageEnable = VK_FALSE;
	msStateInfo.sampleShadingEnable = VK_FALSE;
	msStateInfo.minSampleShading = 1.0f;
	uint32_t smplMask = 0x1;
	msStateInfo.pSampleMask = &smplMask;

	VkPipelineTessellationStateCreateInfo tessStateInfo = { VK_STRUCTURE_TYPE_PIPELINE_TESSELLATION_STATE_CREATE_INFO };
	tessStateInfo.patchControlPoints = 0;

	VkPipelineDynamicStateCreateInfo dynStateInfo;
	VkDynamicState dynStates[] = { VK_DYNAMIC_STATE_VIEWPORT, VK_DYNAMIC_STATE_SCISSOR };
	memset(&dynStateInfo, 0, sizeof(dynStateInfo));
	dynStateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_DYNAMIC_STATE_CREATE_INFO;
	dynStateInfo.dynamicStateCount = 2;
	dynStateInfo.pDynamicStates = dynStates;

	// Shaders
	VkGraphicsPipelineCreateInfo pipelineInfo = { VK_STRUCTURE_TYPE_GRAPHICS_PIPELINE_CREATE_INFO };
	// Assuming that all sub-meshes in an ModelExt have the same layout...
	pipelineInfo.pVertexInputState = &ms_viStateInfo;
	pipelineInfo.pInputAssemblyState = &ms_iaStateInfo;
	pipelineInfo.pViewportState = &vpStateInfo;
	pipelineInfo.pRasterizationState = &rsStateInfo;
	pipelineInfo.pColorBlendState = pBlendState;
	pipelineInfo.pDepthStencilState = &noDepth;
	pipelineInfo.pMultisampleState = &msStateInfo;
	pipelineInfo.pTessellationState = &tessStateInfo;
	pipelineInfo.pDynamicState = &dynStateInfo;

	pipelineInfo.stageCount = shaderCount;
	pipelineInfo.pStages = shaderStages;

	pipelineInfo.renderPass = vk.mainRenderTarget()->copyRenderPass();
	pipelineInfo.subpass = 0;

	pipelineInfo.layout = ms_pipelineLayout;

	result = vkCreateGraphicsPipelines(vk.device(), VK_NULL_HANDLE, 1, &pipelineInfo, NULL,
		pipeline);
	CHECK_VK_RESULT();
}



//======================================================================
//======================================================================
bool NvUIGraphicRenderBaseVK::StaticCleanup()
{
	// TODO TBD

	return true;
}



//======================================================================
// ----- NvUIGraphicRenderVK -----
//======================================================================
NvUIGraphicRenderVK::NvUIGraphicRenderVK(NvUIGraphic* graphic)
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

NvUIGraphicRenderVK::~NvUIGraphicRenderVK()
{
    StaticCleanup();
}

//======================================================================
//======================================================================
bool NvUIGraphicRenderVK::StaticInit()
{
	if (ms_initCount++ == 0)
	{
		NvUIGraphicRenderBaseVK::StaticInit();
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

		InitPipeline(NvGraphicShaderVK::MAX_STAGES,
			ms_shader.mStages, &colorInfoBlend,
			&ms_pipelineAlpha);

		colorStateBlend.blendEnable = VK_FALSE;
		InitPipeline(NvGraphicShaderVK::MAX_STAGES,
			ms_shader.mStages, &colorInfoBlend,
			&ms_pipelineOpaque);
	}

	return true;
}

//======================================================================
//======================================================================
bool NvUIGraphicRenderVK::StaticCleanup()
{
	if (--ms_initCount == 0)
	{
		NvUIGraphicRenderBaseVK::StaticCleanup();
	}

	return true;
}


//======================================================================
//======================================================================
void NvUIGraphicRenderVK::Draw(float alpha, const NvPackedColor& color, const float pixelToClipMatrix[4][4])
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
	vkCmdBindIndexBuffer(cmd, ms_ibo(), 36 * sizeof(uint16_t), VK_INDEX_TYPE_UINT16);

	// Draw the triangle
	vkCmdDrawIndexed(cmd, 4, 1, 0, 0, 0);
}
