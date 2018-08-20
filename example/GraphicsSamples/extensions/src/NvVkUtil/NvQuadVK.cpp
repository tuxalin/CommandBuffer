//----------------------------------------------------------------------------------
// File:        NvVkUtil/NvQuadVK.cpp
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

#include "NV/NvLogs.h"
#include "NvVkUtil/NvQuadVK.h"

// vertex positions in NDC tex-coords
static const float s_fullScreenQuadData[] =
{
	-1.0f, 1.0f, 0.0f, 0.0f,
	1.0f, 1.0f, 1.0f, 0.0f,
	-1.0f, -1.0f, 0.0f, 1.0f,
	1.0f, -1.0f, 1.0f, 1.0f
};

NvQuadVK* NvQuadVK::Create(NvVkContext& vk) {
	VkResult result;

	NvQuadVK* quad = new NvQuadVK;

	// Create the vertex buffer
	result = vk.createAndFillBuffer(4 * 4 * sizeof(float),
		VK_BUFFER_USAGE_VERTEX_BUFFER_BIT,
		VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT,
		quad->mVBO, s_fullScreenQuadData);
	CHECK_VK_RESULT();

	// Create static state info for the mPipeline.
	quad->mVertexBindings.binding = 0;
	quad->mVertexBindings.stride = 4 * sizeof(float);
	quad->mVertexBindings.inputRate = VK_VERTEX_INPUT_RATE_VERTEX;

	quad->mAttributes[0].location = 0;
	quad->mAttributes[0].binding = 0;
	quad->mAttributes[0].format = VK_FORMAT_R32G32_SFLOAT;
	quad->mAttributes[0].offset = 0;
	quad->mAttributes[1].location = 1;
	quad->mAttributes[1].binding = 0;
	quad->mAttributes[1].format = VK_FORMAT_R32G32_SFLOAT;
	quad->mAttributes[1].offset = 2 * sizeof(float);
	
	quad->mVIStateInfo = { VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO };
	quad->mVIStateInfo.vertexBindingDescriptionCount = 1;
	quad->mVIStateInfo.pVertexBindingDescriptions = &quad->mVertexBindings;
	quad->mVIStateInfo.vertexAttributeDescriptionCount = 2;
	quad->mVIStateInfo.pVertexAttributeDescriptions = quad->mAttributes;

	quad->mIAStateInfo = { VK_STRUCTURE_TYPE_PIPELINE_INPUT_ASSEMBLY_STATE_CREATE_INFO };
	quad->mIAStateInfo.topology = VK_PRIMITIVE_TOPOLOGY_TRIANGLE_STRIP;
	quad->mIAStateInfo.primitiveRestartEnable = VK_FALSE;

	return quad;
}

void NvQuadVK::Draw(VkCommandBuffer& cmd) {
	VkResult result = VK_SUCCESS;

	// Bind the vertex and index buffers
	VkDeviceSize offsets[] = { 0 };
	vkCmdBindVertexBuffers(cmd, 0, 1, &mVBO(), offsets);

	// Draw the triangle
	vkCmdDraw(cmd, 4, 1, 0, 0);
}

NvQuadVK::NvQuadVK() {


}
