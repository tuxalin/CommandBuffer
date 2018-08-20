//----------------------------------------------------------------------------------
// File:        NvVkUtil/NvModelVK.cpp
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
#include "NvVkUtil/NvModelVK.h"
#include "NvModel/NvModel.h"
#include "../NvModel/NvModelObj.h"

NvModelVK* NvModelVK::CreateFromObj(NvVkContext& vk, uint8_t* data, float scale, bool computeNormals, bool computeTangents) {
	NvModel* model = NvModelObj::CreateFromObj(data, scale, computeNormals, computeTangents);
	if (!model)
		return NULL;

	NvModelVK* modelVK = new NvModelVK;
	modelVK->model = model;
	modelVK->computeCenter();
	modelVK->initBuffers(vk);

	return modelVK;
}

NvModelVK* NvModelVK::CreateFromPreprocessed(NvVkContext& vk, uint8_t* data) {
	NvModel* model = NvModel::CreateFromPreprocessed(data);
	if (!model)
		return NULL;

	NvModelVK* modelVK = new NvModelVK;
	modelVK->model = model;
	modelVK->computeCenter();
	modelVK->initBuffers(vk);

	return modelVK;
}

NvModelVK::NvModelVK()
{
	model = NULL;
}

NvModelVK::~NvModelVK()
{
	delete model;
}

void NvModelVK::computeCenter()
{
	model->getBoundingBox(m_minExtent, m_maxExtent);

	m_radius = (m_maxExtent - m_minExtent) / 2.0f;
	m_center = m_minExtent + m_radius;
}

static VkFormat getComponentFormat(uint32_t count) {
	switch (count) {
	case 1:
		return VK_FORMAT_R32_SFLOAT;
	case 2:
		return VK_FORMAT_R32G32_SFLOAT;
	case 3:
		return VK_FORMAT_R32G32B32_SFLOAT;
	case 4:
		return VK_FORMAT_R32G32B32A32_SFLOAT;
	default:
		return VK_FORMAT_UNDEFINED;
	}
}

void NvModelVK::initBuffers(NvVkContext& vk)
{
	VkResult result;

	// We will always have positions in our vertex, and we will put 
	// them first in the vertex layout
	int32_t positionOffset = 0;

	// Account for normals, if there are any
	int32_t normalOffset = -1;
	if (model->getCompiledNormalOffset() > 0)
		normalOffset = model->getCompiledNormalOffset() * sizeof(float);

	// Account for texture coordinates, if there are any
	int32_t texCoordOffset = -1;
	if (model->getCompiledTexCoordOffset() > 0)
		texCoordOffset = model->getCompiledTexCoordOffset() * sizeof(float);

	// Account for tangents, if there are any
	int32_t tangentOffset = -1;
	if (model->getCompiledTangentOffset() > 0)
		tangentOffset = model->getCompiledTangentOffset() * sizeof(float);

	uint32_t vertBytes = sizeof(float) * model->getCompiledVertexSize();

	// Allocate a large enough vertex buffer to hold all vertices in the mesh
	uint32_t vertexCount = model->getCompiledVertexCount();

	// Create a vertex buffer and fill it with data
	uint32_t vboBytes = vertBytes * vertexCount;

	// Create the vertex buffer
	result = vk.createAndFillBuffer(vboBytes,
		VK_BUFFER_USAGE_VERTEX_BUFFER_BIT, 
		VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT,
		mVBO, model->getCompiledVertices());
	CHECK_VK_RESULT();

	// Create the index buffer
	NvModelPrimType::Enum prim;
	mIndexCount = model->getCompiledIndexCount(prim);
	result = vk.createAndFillBuffer(sizeof(uint32_t) * mIndexCount,
		VK_BUFFER_USAGE_INDEX_BUFFER_BIT,
		VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT,
		mIBO, model->getCompiledIndices(prim));
	CHECK_VK_RESULT();

	// Create static state info for the mPipeline.
	mVertexBindings.binding = 0;
	mVertexBindings.stride = vertBytes;
	mVertexBindings.inputRate = VK_VERTEX_INPUT_RATE_VERTEX;

	mAttributes[0].location = 0;
	mAttributes[0].binding = 0;
	mAttributes[0].format = getComponentFormat(model->getPositionSize());
	mAttributes[0].offset = 0;
	mAttributes[1].location = 1;
	mAttributes[1].binding = 0;
	mAttributes[1].format = getComponentFormat(model->getNormalSize());
	mAttributes[1].offset = normalOffset;
	mAttributes[2].location = 2;
	mAttributes[2].binding = 0;
	mAttributes[2].format = getComponentFormat(model->getTexCoordSize());
	mAttributes[2].offset = texCoordOffset;

	mVIStateInfo = { VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO };
	mVIStateInfo.vertexBindingDescriptionCount = 1;
	mVIStateInfo.pVertexBindingDescriptions = &mVertexBindings;
	mVIStateInfo.vertexAttributeDescriptionCount = 3;
	mVIStateInfo.pVertexAttributeDescriptions = mAttributes;

	mIAStateInfo = { VK_STRUCTURE_TYPE_PIPELINE_INPUT_ASSEMBLY_STATE_CREATE_INFO };
	mIAStateInfo.topology = VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST;
	mIAStateInfo.primitiveRestartEnable = VK_FALSE;
}


NvModel * NvModelVK::getModel()
{
	return model;
}

void NvModelVK::Draw(VkCommandBuffer& cmd) {
	VkResult result = VK_SUCCESS;

	// Bind the vertex and index buffers
	VkDeviceSize offsets[] = { 0 };
	vkCmdBindVertexBuffers(cmd, 0, 1, &mVBO(), offsets);
	vkCmdBindIndexBuffer(cmd, mIBO(), 0, getIndexType());

	// Draw the triangle
	vkCmdDrawIndexed(cmd, mIndexCount, 1, 0, 0, 0);
}

