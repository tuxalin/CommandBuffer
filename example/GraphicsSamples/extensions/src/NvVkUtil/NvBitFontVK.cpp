//----------------------------------------------------------------------------------
// File:        NvVkUtil/NvBitFontVK.cpp
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
#include "../../src/NvUI/NvBitFontInternal.h"
#include "NvUIVK.h"
#include "NvVkUtil/NvVkContext.h"
#include "NV/NvMath.h"

#define USE_SOURCE_SHADERS 0

#if USE_SOURCE_SHADERS
//========================================================================
// static vars
//========================================================================

static char s_fontShader[] =
"#GLSL_VS\n"
"#version 440 core\n"
"#extension GL_KHR_vulkan_glsl : require\n"
"#extension GL_NV_gpu_shader5 : require\n"
"\n"
"// this is set from higher level.  think of it as the upper model matrix\n"
"layout(set = 0, binding = 0) uniform Block {\n"
"    mat4 pixelToClipMat;\n"
"    vec2 texelScale;\n"
"    vec4 outlineColor;\n"
"};\n"
"layout(location = 0) in vec2 pos_attr;\n"
"layout(location = 1) in vec2 tex_attr;\n"
"layout(location = 2) in vec4 col_attr;\n"
"out vec4 col_var;\n"
"out vec2 tex_var;\n"
"void main() {\n"
"    // account for translation and rotation of the primitive into [-1,1] spatial default.\n"
"    gl_Position = pixelToClipMat * vec4(pos_attr.x, pos_attr.y, 0, 1);\n"
"    col_var = col_attr;"
"    tex_var = tex_attr;\n"
"}\n"
"#GLSL_FS\n"
"#version 440 core\n"
"#extension GL_KHR_vulkan_glsl : require\n"
"#extension GL_NV_gpu_shader5 : require\n"
"\n"
"layout(set = 0, binding = 0) uniform Block {\n"
"    mat4 pixelToClipMat;\n"
"    vec2 texelScale;\n"
"    vec4 outlineColor;\n"
"};\n"
"layout(set = 1, binding = 1) uniform sampler2D fontTex;\n"
"in vec4 col_var;\n"
"in vec2 tex_var;\n"
"layout(location = 0) out vec4 fragColor;\n"
"void main() {\n"
"    float alpha = texture(fontTex, tex_var).r;\n"
"    fragColor = col_var * vec4(1.0, 1.0, 1.0, alpha);\n"
"}\n";

// this seems the most efficient way to outline an existing glyph.
// one key thing is it samples 'x' and not '+', helping sharp corners.
static char s_fontOutlineShader[] =
"#GLSL_VS\n"
"#version 440 core\n"
"#extension GL_KHR_vulkan_glsl : require\n"
"#extension GL_NV_gpu_shader5 : require\n"
"\n"
"// this is set from higher level.  think of it as the upper model matrix\n"
"layout(binding = 0) uniform Block {\n"
"    mat4 pixelToClipMat;\n"
"    vec2 texelScale;\n"
"    vec4 outlineColor;\n"
"};\n"
"layout(location = 0) in vec2 pos_attr;\n"
"layout(location = 1) in vec2 tex_attr;\n"
"layout(location = 2) in vec4 col_attr;\n"
"out vec4 col_var;\n"
"out vec2 tex_var;\n"
"void main() {\n"
"    // account for translation and rotation of the primitive into [-1,1] spatial default.\n"
"    gl_Position = pixelToClipMat * vec4(pos_attr.x, pos_attr.y, 0, 1);\n"
"    col_var = col_attr;"
"    tex_var = tex_attr;\n"
"}\n"
"#GLSL_FS\n"
"#version 440 core\n"
"#extension GL_KHR_vulkan_glsl : require\n"
"#extension GL_NV_gpu_shader5 : require\n"
"\n"
"layout(set = 0, binding = 0) uniform Block {\n"
"    mat4 pixelToClipMat;\n"
"    vec2 texelScale;\n"
"    vec4 outlineColor;\n"
"};\n"
"layout(set = 1, binding = 1) uniform sampler2D fontTex;\n"
"in vec4 col_var;\n"
"in vec2 tex_var;\n"
"layout(location = 0) out vec4 fragColor;\n"
"void main() {\n"
"    float alpha = texture(fontTex, tex_var).r;\n"
"    vec4 baseCol = col_var;\n"
"    baseCol.a = alpha;\n"
"    float left = tex_var.x - texelScale.x;\n"
"    float right = tex_var.x + texelScale.x;\n"
"    float top = tex_var.y - texelScale.y;\n"
"    float bottom = tex_var.y + texelScale.y;\n"
"    float a1 = texture(fontTex, vec2(left, top)).r;\n"
"    float a2 = texture(fontTex, vec2(right, top)).r;\n"
"    float a3 = texture(fontTex, vec2(left, bottom)).r;\n"
"    float a4 = texture(fontTex, vec2(right, bottom)).r;\n"
"    vec4 lineCol = outlineColor;\n"
"    lineCol.a = clamp(a1+a2+a3+a4+alpha, 0.0, 1.0);\n"
"    fragColor = mix(lineCol, baseCol, alpha);\n"
"}\n";
#else
#include "shaders/font.cpp"
#include "shaders/fontOutline.cpp"
#endif

const static int32_t MAX_CHARS = 1024;

//========================================================================
// datatypes & defines
//========================================================================

typedef struct
{
	nv::matrix4f pixelToClipMat;
	nv::vec2f texelScale;
	nv::vec4f outlineColor;
} NvBitFontUBO;

typedef struct
{
	float x, y;     // position
	float s, t;     // texture coordinate
	uint32_t color; 
} NvBitFontVertex;

static NvPackedColor s_charColorTable[6] =
{
    NV_PACKED_COLOR(0xFF, 0xFF, 0xFF, 0xFF), //white
    NV_PACKED_COLOR(0x99, 0x99, 0x99, 0xFF), //medium-gray
    NV_PACKED_COLOR(0x00, 0x00, 0x00, 0xFF), //black
    NV_PACKED_COLOR(0xFF, 0x33, 0x33, 0xFF), //brightened red
    NV_PACKED_COLOR(0x11, 0xFF, 0x11, 0xFF), //brighter green
    NV_PACKED_COLOR(0x33, 0x33, 0xFF, 0xFF) //brightened blue
};

class NvBitFontRenderVK: public NvBitFontRender {
public:
    static NvBitFontRender* Create(NvImage* image);

    virtual ~NvBitFontRenderVK() {
		// delete font texture
//		NvUIVKctx().mAlloc->free(mTex);
 //        glDeleteTextures(1, &(m_tex));
    }

	VkDescriptorSet m_texDescriptorSet;
	NvVkTexture mTex;
};

class NvBFTextRenderVK : public NvBFTextRender {
public:
    static NvBFTextRender* Create();
	static int32_t StaticInit();
	static void InitPipeline(uint32_t shaderCount,
		VkPipelineShaderStageCreateInfo* shaderStages,
		VkPipeline* pipeline);

    NvBFTextRenderVK();
    virtual ~NvBFTextRenderVK();
    virtual void RenderPrep();
    virtual void Render(const float* matrix, const NvPackedColor& color, NvBitFont* font, bool outline, int count);
    virtual void UpdateText(int count, const BFVert* data, bool midrender);
    virtual void RenderDone();

	NvVkBuffer m_vbo;
	NvBitFontVertex* m_vboMapping;

	static NvSimpleUBO<NvBitFontUBO, 1024> ms_ubo;

	static NvVkBuffer ms_ibo;

	static VkVertexInputAttributeDescription ms_attributes[3];
	static VkVertexInputBindingDescription ms_vertexBindings;

	static VkPipelineVertexInputStateCreateInfo ms_viStateInfo;
	static VkPipelineInputAssemblyStateCreateInfo ms_iaStateInfo;

	static VkDescriptorPool ms_descriptorPool;
	static VkDescriptorSetLayout ms_descriptorSetLayouts[2];
	static VkDescriptorSet ms_uboDescriptorSet;

	static VkPipelineLayout ms_pipelineLayout;
	static VkPipeline ms_pipelineText;
	static VkPipeline ms_pipelineOutline;
	static VkSampler ms_sampler;
};

NvSimpleUBO<NvBitFontUBO, 1024> NvBFTextRenderVK::ms_ubo;
NvVkBuffer NvBFTextRenderVK::ms_ibo;
VkVertexInputAttributeDescription NvBFTextRenderVK::ms_attributes[3];
VkVertexInputBindingDescription NvBFTextRenderVK::ms_vertexBindings;
VkPipelineVertexInputStateCreateInfo NvBFTextRenderVK::ms_viStateInfo;
VkPipelineInputAssemblyStateCreateInfo NvBFTextRenderVK::ms_iaStateInfo;
VkDescriptorPool NvBFTextRenderVK::ms_descriptorPool;
VkDescriptorSetLayout NvBFTextRenderVK::ms_descriptorSetLayouts[2];
VkDescriptorSet NvBFTextRenderVK::ms_uboDescriptorSet;
VkPipelineLayout NvBFTextRenderVK::ms_pipelineLayout;
VkPipeline NvBFTextRenderVK::ms_pipelineText;
VkPipeline NvBFTextRenderVK::ms_pipelineOutline;
VkSampler NvBFTextRenderVK::ms_sampler;

NvBFTextRender* NvBFTextRenderVK::Create() {
    return new NvBFTextRenderVK;
}

int32_t NvBFTextRenderVK::StaticInit() {
	NvVkContext& vk = *NvUIVKctx().mVk;
	VkResult result;

	// Create an index buffer and fill it with data
	int32_t n = sizeof(int16_t) * IND_PER_QUAD * MAX_CHARS;
	int16_t* indexList = (int16_t*)malloc(n);

	// re-init the index buffer.
	for (int32_t c = 0; c<MAX_CHARS; c++) // triangle list indices... three per triangle, six per quad.
	{
		indexList[c * 6 + 0] = (int16_t)(c * 4 + 0);
		indexList[c * 6 + 1] = (int16_t)(c * 4 + 2);
		indexList[c * 6 + 2] = (int16_t)(c * 4 + 1);
		indexList[c * 6 + 3] = (int16_t)(c * 4 + 0);
		indexList[c * 6 + 4] = (int16_t)(c * 4 + 3);
		indexList[c * 6 + 5] = (int16_t)(c * 4 + 2);
	}

	if (VK_SUCCESS != vk.createAndFillBuffer(n, VK_BUFFER_USAGE_INDEX_BUFFER_BIT, 
		VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT, ms_ibo, indexList))
		return -1;

	delete[] indexList;

	ms_ubo.Initialize(vk);

	// Create static state info for the mPipeline.
	ms_vertexBindings.binding = 0;
	ms_vertexBindings.stride = sizeof(NvBitFontVertex);
	ms_vertexBindings.inputRate = VK_VERTEX_INPUT_RATE_VERTEX;

	ms_attributes[0].location = 0;
	ms_attributes[0].binding = 0;
	ms_attributes[0].format = VK_FORMAT_R32G32_SFLOAT;
	ms_attributes[0].offset = offsetof(NvBitFontVertex, x);
	ms_attributes[1].location = 1;
	ms_attributes[1].binding = 0;
	ms_attributes[1].format = VK_FORMAT_R32G32_SFLOAT;
	ms_attributes[1].offset = offsetof(NvBitFontVertex, s);
	ms_attributes[2].location = 2;
	ms_attributes[2].binding = 0;
	ms_attributes[2].format = VK_FORMAT_R8G8B8A8_UNORM;
	ms_attributes[2].offset = offsetof(NvBitFontVertex, color);

	// Create descriptor layout to match the shader resources
	{
		VkDescriptorSetLayoutBinding binding;
		binding.binding = 0;
		binding.descriptorCount = 1;
		binding.stageFlags = VK_SHADER_STAGE_VERTEX_BIT | VK_SHADER_STAGE_FRAGMENT_BIT;
		binding.descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER_DYNAMIC;
		binding.pImmutableSamplers = NULL;

		VkDescriptorSetLayoutCreateInfo descriptorSetEntry = { VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO };
		descriptorSetEntry.bindingCount = 1;
		descriptorSetEntry.pBindings = &binding;

		result = vkCreateDescriptorSetLayout(vk.device(), &descriptorSetEntry, 0, ms_descriptorSetLayouts);
		CHECK_VK_RESULT();
	}

	// Create descriptor layout to match the shader resources
	{
		VkDescriptorSetLayoutBinding binding;
		binding.binding = 1;
		binding.descriptorCount = 1;
		binding.stageFlags = VK_SHADER_STAGE_FRAGMENT_BIT;
		binding.descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
		binding.pImmutableSamplers = NULL;

		VkDescriptorSetLayoutCreateInfo descriptorSetEntry = { VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO };
		descriptorSetEntry.bindingCount = 1;
		descriptorSetEntry.pBindings = &binding;

		result = vkCreateDescriptorSetLayout(vk.device(), &descriptorSetEntry, 0, ms_descriptorSetLayouts+1);
		CHECK_VK_RESULT();
	}

	{
		VkPipelineLayoutCreateInfo pipelineLayoutCreateInfo = { VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO };
		pipelineLayoutCreateInfo.setLayoutCount = 2;
		pipelineLayoutCreateInfo.pSetLayouts = ms_descriptorSetLayouts;
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
	descriptorRegionInfo.poolSizeCount = sizeof descriptorPoolSizes / sizeof descriptorPoolSizes[0];
	descriptorRegionInfo.pPoolSizes = descriptorPoolSizes;
	
	result = vkCreateDescriptorPool(vk.device(), &descriptorRegionInfo, NULL, &ms_descriptorPool);
	CHECK_VK_RESULT();

	ms_viStateInfo = { VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO };
	ms_viStateInfo.vertexBindingDescriptionCount = 1;
	ms_viStateInfo.pVertexBindingDescriptions = &ms_vertexBindings;
	ms_viStateInfo.vertexAttributeDescriptionCount = 3;
	ms_viStateInfo.pVertexAttributeDescriptions = ms_attributes;

	ms_iaStateInfo = { VK_STRUCTURE_TYPE_PIPELINE_INPUT_ASSEMBLY_STATE_CREATE_INFO };
	ms_iaStateInfo.topology = VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST;
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
	samplerCreateInfo.maxLod = 12.0;
	samplerCreateInfo.borderColor = VK_BORDER_COLOR_FLOAT_OPAQUE_WHITE;

	result = vkCreateSampler(vk.device(), &samplerCreateInfo, 0, &ms_sampler);

	VkPipelineShaderStageCreateInfo stages[2];
	uint32_t stageCount = 0;
#if USE_SOURCE_SHADERS
	stageCount = vk.createShadersFromSourceString(s_fontShader, stages, 2);
#else
	stageCount = vk.createShadersFromBinaryBlob((uint32_t*)fontData, fontLength, stages, 2);
#endif

	InitPipeline(stageCount, stages, &ms_pipelineText);

#if USE_SOURCE_SHADERS
	stageCount = vk.createShadersFromSourceString(s_fontOutlineShader, stages, 2);
#else
	stageCount = vk.createShadersFromBinaryBlob((uint32_t*)fontOutlineData, fontOutlineLength, stages, 2);
#endif

	InitPipeline(stageCount, stages, &ms_pipelineOutline);

	VkDescriptorSetAllocateInfo descriptorSetAllocateInfo = { VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO };
	descriptorSetAllocateInfo.descriptorPool = ms_descriptorPool;
	descriptorSetAllocateInfo.descriptorSetCount = 1;
	descriptorSetAllocateInfo.pSetLayouts = ms_descriptorSetLayouts;
	result = vkAllocateDescriptorSets(vk.device(), &descriptorSetAllocateInfo, &ms_uboDescriptorSet);
	CHECK_VK_RESULT();

	VkDescriptorBufferInfo uboDesc;
	ms_ubo.GetDesc(uboDesc);

	VkWriteDescriptorSet writeDescriptorSet;
	writeDescriptorSet = { VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET };
	writeDescriptorSet.dstSet = ms_uboDescriptorSet;
	writeDescriptorSet.dstBinding = 0;
	writeDescriptorSet.dstArrayElement = 0;
	writeDescriptorSet.descriptorCount = 1;
	writeDescriptorSet.descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER_DYNAMIC;
	writeDescriptorSet.pBufferInfo = &uboDesc;

	vkUpdateDescriptorSets(vk.device(), 1, &writeDescriptorSet, 0, 0);

	return 0;
}

void NvBFTextRenderVK::InitPipeline(uint32_t shaderCount,
	VkPipelineShaderStageCreateInfo* shaderStages,
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

	// Shaders
	VkGraphicsPipelineCreateInfo pipelineInfo = { VK_STRUCTURE_TYPE_GRAPHICS_PIPELINE_CREATE_INFO };
	// Assuming that all sub-meshes in an ModelExt have the same layout...
	pipelineInfo.pVertexInputState = &ms_viStateInfo;
	pipelineInfo.pInputAssemblyState = &ms_iaStateInfo;
	pipelineInfo.pViewportState = &vpStateInfo;
	pipelineInfo.pRasterizationState = &rsStateInfo;
	pipelineInfo.pColorBlendState = &colorInfoBlend;
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


static void NvBitFontRenderShutdown()
{
}

NvBitFontRender* NvBitFontRenderVK::Create(NvImage *image)
{
	NvVkContext& vk = *NvUIVKctx().mVk;
	VkResult result;
	NvBitFontRenderVK* render = new NvBitFontRenderVK;

	if (!image || (image->getFormat() != NVIMAGE_ALPHA) ||
		(image->getType() != NVIMAGE_UNSIGNED_BYTE)) {
		render->mTex = {};
		delete render;
		return NULL;
	}

	if (!vk.uploadTexture(image, render->mTex))
		render->mTex = {};

	VkDescriptorSetAllocateInfo descriptorSetAllocateInfo = { VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO };
	descriptorSetAllocateInfo.descriptorPool = NvBFTextRenderVK::ms_descriptorPool;
	descriptorSetAllocateInfo.descriptorSetCount = 1;
	descriptorSetAllocateInfo.pSetLayouts = NvBFTextRenderVK::ms_descriptorSetLayouts + 1;
	result = vkAllocateDescriptorSets(vk.device(), &descriptorSetAllocateInfo, &render->m_texDescriptorSet);
	CHECK_VK_RESULT();

	VkDescriptorImageInfo texDesc = {};
	texDesc.imageLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
	texDesc.imageView = render->mTex.view;
	texDesc.sampler = NvBFTextRenderVK::ms_sampler;

	VkWriteDescriptorSet writeDescriptorSet;
	writeDescriptorSet = { VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET };
	writeDescriptorSet.dstSet = render->m_texDescriptorSet;
	writeDescriptorSet.dstBinding = 1;
	writeDescriptorSet.dstArrayElement = 0;
	writeDescriptorSet.descriptorCount = 1;
	writeDescriptorSet.descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
	writeDescriptorSet.pImageInfo = &texDesc;

	vkUpdateDescriptorSets(vk.device(), 1, &writeDescriptorSet, 0, 0);

	return render;
}



//========================================================================
NvBFTextRenderVK::NvBFTextRenderVK() //: m_vbo(0)
{
	NvVkContext& vk = *NvUIVKctx().mVk;
	VkResult result;

	int32_t bufferSize = MAX_CHARS * VERT_PER_QUAD * sizeof(NvBitFontVertex);
	if (VK_SUCCESS != vk.createAndFillBuffer(bufferSize, VK_BUFFER_USAGE_VERTEX_BUFFER_BIT, 
		VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT, 
		m_vbo)) {
		m_vboMapping = NULL;
		return;
	}

	result = vkMapMemory(vk.device(), m_vbo.mem, 0, bufferSize, 0, (void**)&m_vboMapping);
	if (result != VK_SUCCESS) {
		m_vboMapping = NULL;
	}
}

//========================================================================
NvBFTextRenderVK::~NvBFTextRenderVK()
{
}


//========================================================================
//========================================================================
//========================================================================
void NvBFTextRenderVK::RenderPrep()
{
}


void NvBFTextRenderVK::Render(const float* matrix, const NvPackedColor& color, NvBitFont* font, bool outline, int count)
{
	NvVkContext& vk = *NvUIVKctx().mVk;
	VkCommandBuffer& cmd = NvUIVKctx().mCmd;

	// Set transform
	// pixelToClipMatrix
	memcpy(&(ms_ubo->pixelToClipMat), matrix, 16 * sizeof(float));
	ms_ubo->pixelToClipMat(1, 1) *= -1.0f;
	ms_ubo->pixelToClipMat(1, 3) *= -1.0f;
	ms_ubo->texelScale.x = 1.0f / font->m_afont->m_charCommon.m_pageWidth;
	ms_ubo->texelScale.y = 1.0f / font->m_afont->m_charCommon.m_pageHeight;
	ms_ubo->outlineColor = nv::vec4f(NV_PC_RED_FLOAT(color), NV_PC_GREEN_FLOAT(color),
		NV_PC_BLUE_FLOAT(color), NV_PC_ALPHA_FLOAT(color));

	ms_ubo.Update();
	uint32_t uboOffset = ms_ubo.getDynamicOffset();

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

	if (outline)
		vkCmdBindPipeline(cmd, VK_PIPELINE_BIND_POINT_GRAPHICS, ms_pipelineOutline);
	else
		vkCmdBindPipeline(cmd, VK_PIPELINE_BIND_POINT_GRAPHICS, ms_pipelineText);

	vkCmdBindDescriptorSets(cmd, VK_PIPELINE_BIND_POINT_GRAPHICS, ms_pipelineLayout, 0, 1, &ms_uboDescriptorSet, 1, &uboOffset);
	vkCmdBindDescriptorSets(cmd, VK_PIPELINE_BIND_POINT_GRAPHICS, ms_pipelineLayout, 1, 1, 
		&(((NvBitFontRenderVK*)font->m_render)->m_texDescriptorSet), 0, NULL);

	// Bind the vertex and index buffers
	VkDeviceSize offsets[] = { 0 };
	vkCmdBindVertexBuffers(cmd, 0, 1, &m_vbo(), offsets);
	vkCmdBindIndexBuffer(cmd, ms_ibo(), 0, VK_INDEX_TYPE_UINT16);

	// Draw the triangle
	vkCmdDrawIndexed(cmd, IND_PER_QUAD * count, 1, 0, 0, 0);
}

void NvBFTextRenderVK::UpdateText(int count, const BFVert* data, bool midrender)
{
	if (!m_vboMapping)
		return;

	memcpy(m_vboMapping, data, count * VERT_PER_QUAD * sizeof(NvBitFontVertex));
	VkMappedMemoryRange range = { VK_STRUCTURE_TYPE_MAPPED_MEMORY_RANGE };
	range.memory = m_vbo.mem;
	range.offset = 0;
	range.size = count * VERT_PER_QUAD * sizeof(NvBitFontVertex);
	vkFlushMappedMemoryRanges(NvUIVKctx().mVk->device(), 1, &range);
}

void NvBFTextRenderVK::RenderDone()
{
	/* */
}

void NvBitfontUseVK(NvVkContext* context) {
	NvBitFontRenderFactory::GlobalInit = &NvBFTextRenderVK::StaticInit;
    NvBitFontRenderFactory::FontRenderCreate = &NvBitFontRenderVK::Create;
    NvBitFontRenderFactory::TextRenderCreate = &NvBFTextRenderVK::Create;
    NvBitFontRenderFactory::GlobalShutdown = &NvBitFontRenderShutdown;
}
