//----------------------------------------------------------------------------------
// File:        NvVkUtil/NvMeshExtVK.cpp
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

#include "NvVkUtil/NvMeshExtVK.h"
#include "NvModel/NvModelExt.h"
#include "NvModel/NvSkeleton.h"
#include "../../src/NvModel/NvModelExtObj.h"
#include "../../src/NvModel/NvModelSubMeshObj.h"
#include "NvVkUtil/NvVkContext.h"

namespace Nv
{
	NvMeshExtVK::NvMeshExtVK() :
		m_pSrcMesh(NULL),
		m_materialID(-1),
		m_indexCount(0),
		m_vertexCount(0),
		m_vertexSize(0),
		m_positionSize(0),
		m_positionOffset(0),
		m_normalSize(0),
		m_normalOffset(-1),
		m_texCoordSize(0),
		m_texCoordOffset(-1),
		m_tangentSize(0),
		m_tangentOffset(-1),
		m_colorSize(0),
		m_colorOffset(-1),
		m_boneSize(0),
		m_boneIndexOffset(-1),
		m_weightSize(0),
		m_weightOffset(-1),
		m_parentNode(-1)
	{
	}

	NvMeshExtVK::NvMeshExtVK(const NvMeshExtVK& other) :
		m_pSrcMesh(other.m_pSrcMesh),
		m_materialID(other.m_materialID),
		m_indexCount(other.m_indexCount),
		m_vertexCount(other.m_vertexCount),
		m_vertexSize(other.m_vertexSize),
		m_positionSize(other.m_positionSize),
		m_positionOffset(other.m_positionOffset),
		m_normalSize(other.m_normalSize),
		m_normalOffset(other.m_normalOffset),
		m_texCoordSize(other.m_texCoordSize),
		m_texCoordOffset(other.m_texCoordOffset),
		m_tangentSize(other.m_tangentSize),
		m_tangentOffset(other.m_tangentOffset),
		m_colorSize(other.m_colorSize),
		m_colorOffset(other.m_colorOffset),
		m_boneSize(other.m_boneSize),
		m_boneIndexOffset(other.m_boneIndexOffset),
		m_weightSize(other.m_weightSize),
		m_weightOffset(other.m_weightOffset),
		m_parentNode(other.m_parentNode),
		m_offsetMatrix(other.m_offsetMatrix)
	{
	}

	NvMeshExtVK::~NvMeshExtVK()
	{
		Clear();
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

	bool NvMeshExtVK::InitFromSubmesh(NvVkContext& vk,
		NvModelExt* pModel, uint32_t subMeshID)
	{
		if (NULL == pModel)
		{
			return false;
		}

		m_pSrcMesh = pModel->GetSubMesh(subMeshID);
		if (NULL == m_pSrcMesh)
		{
			return false;
		}

		m_materialID = m_pSrcMesh->m_materialId;

		m_vertexSize = m_pSrcMesh->m_vertSize; // In Floats

		// We will always have positions in our vertex, and we will put 
		// them first in the vertex layout
		m_positionOffset = 0;

		// Account for normals, if there are any
		m_normalOffset = m_pSrcMesh->m_normalOffset * sizeof(float);

		// Account for texture coordinates, if there are any
		m_texCoordOffset = m_pSrcMesh->m_texCoordOffset * sizeof(float);

		// Account for tangents, if there are any
		m_tangentOffset = m_pSrcMesh->m_tangentOffset * sizeof(float);

		// Account for colors, if there are any
		m_colorOffset = m_pSrcMesh->m_colorOffset * sizeof(float);

		// Account for bone indices, if there are any
		m_boneIndexOffset = m_pSrcMesh->m_boneIndexOffset * sizeof(uint32_t);

		// Account for bone weights, if there are any
		m_weightOffset = m_pSrcMesh->m_boneWeightOffset * sizeof(float);

		uint32_t vertBytes = sizeof(float) * m_vertexSize;

		// Allocate a large enough vertex buffer to hold all vertices in the mesh
		m_vertexCount = m_pSrcMesh->m_vertexCount;

		m_parentNode = m_pSrcMesh->m_parentBone;

		// Create a vertex buffer and fill it with data
		uint32_t vboBytes = vertBytes * m_vertexCount;


		// Allocate a large enough index buffer to hold the indices for all primitives in the mesh
		m_indexCount = m_pSrcMesh->m_indexCount;

		// Create the vertex buffer
		VkResult result = vk.createAndFillBuffer(vboBytes,
            VK_BUFFER_USAGE_VERTEX_BUFFER_BIT, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT,
			mVBO, m_pSrcMesh->m_vertices);
		CHECK_VK_RESULT();

		// Create the index buffer
		result = vk.createAndFillBuffer(sizeof(uint32_t) * m_indexCount,
            VK_BUFFER_USAGE_INDEX_BUFFER_BIT, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT,
			mIBO, m_pSrcMesh->m_indices);
		CHECK_VK_RESULT();

		// Create static state info for the mPipeline.
		mBindingCount = 1; // no instancing initially
		mVertexBindings[0].binding = 0;
		mVertexBindings[0].stride = vertBytes;
		mVertexBindings[0].inputRate = VK_VERTEX_INPUT_RATE_VERTEX;
		mVertexBindings[1].binding = 1;
		mVertexBindings[1].stride = 0; // set if instance data is enabled
		mVertexBindings[1].inputRate = VK_VERTEX_INPUT_RATE_INSTANCE;

		return true;
	}

	bool NvMeshExtVK::UpdateBoneTransforms(Nv::NvSkeleton* pSrcSkel)
	{
		if ((NULL == m_pSrcMesh) || (NULL == pSrcSkel))
		{
			return false;
		}

		// Update the mesh offset from its parent node
		nv::matrix4f* pSrcTransforms = pSrcSkel->GetTransforms();
		if ((m_parentNode >= 0) && (m_parentNode < pSrcSkel->GetNumNodes()))
		{
			m_offsetMatrix = pSrcTransforms[m_parentNode];
		}

		return true;
	}

	bool NvMeshExtVK::InitVertexState() {
		mAttribCount = 0;

		mAttributes[0].location = 0;
		mAttributes[mAttribCount].binding = 0;
		mAttributes[mAttribCount].format = getComponentFormat(3);
		mAttributes[mAttribCount].offset = 0;
		mAttribCount++;

		if (m_pSrcMesh->HasNormals()) {
			mAttributes[mAttribCount].location = 1;
			mAttributes[mAttribCount].binding = 0;
			mAttributes[mAttribCount].format = getComponentFormat(3);
			mAttributes[mAttribCount].offset = m_normalOffset;
			mAttribCount++;
		}

		if (m_pSrcMesh->HasTexCoords()) {
			mAttributes[mAttribCount].location = 2;
			mAttributes[mAttribCount].binding = 0;
			mAttributes[mAttribCount].format = getComponentFormat(2);
			mAttributes[mAttribCount].offset = m_texCoordOffset;
			mAttribCount++;
		}

		if (m_pSrcMesh->HasTangents()) {
			mAttributes[mAttribCount].location = 3;
			mAttributes[mAttribCount].binding = 0;
			mAttributes[mAttribCount].format = getComponentFormat(3);
			mAttributes[mAttribCount].offset = m_tangentOffset;
			mAttribCount++;
		}

		if (m_pSrcMesh->HasColors()) {
			mAttributes[mAttribCount].location = 4;
			mAttributes[mAttribCount].binding = 0;
			mAttributes[mAttribCount].format = getComponentFormat(4);
			mAttributes[mAttribCount].offset = m_colorOffset;
			mAttribCount++;
		}

		if (m_pSrcMesh->HasBoneWeights()) {
			mAttributes[mAttribCount].location = 5;
			mAttributes[mAttribCount].binding = 0;
			mAttributes[mAttribCount].format = VK_FORMAT_R32G32B32A32_UINT;
			mAttributes[mAttribCount].offset = m_boneIndexOffset;
			mAttribCount++;
			mAttributes[mAttribCount].location = 6;
			mAttributes[mAttribCount].binding = 0;
			mAttributes[mAttribCount].format = getComponentFormat(4);
			mAttributes[mAttribCount].offset = m_weightOffset;
			mAttribCount++;
		}

		mVIStateInfo = { VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO };
		mVIStateInfo.vertexBindingDescriptionCount = mBindingCount;
		mVIStateInfo.pVertexBindingDescriptions = mVertexBindings;
		mVIStateInfo.vertexAttributeDescriptionCount = mAttribCount;
		mVIStateInfo.pVertexAttributeDescriptions = mAttributes;

		mIAStateInfo = { VK_STRUCTURE_TYPE_PIPELINE_INPUT_ASSEMBLY_STATE_CREATE_INFO };
		mIAStateInfo.topology = VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST;
		mIAStateInfo.primitiveRestartEnable = VK_FALSE;

		return true;
	}

	bool NvMeshExtVK::EnableInstanceData(uint32_t instanceVertSize) {
		mBindingCount = 2;
		mVertexBindings[1].stride = instanceVertSize;

		mVIStateInfo.vertexBindingDescriptionCount = mBindingCount;

		return true;
	}

	bool NvMeshExtVK::AddInstanceData(uint32_t location, VkFormat format, uint32_t offset) {
		mAttributes[mAttribCount].location = location;
		mAttributes[mAttribCount].binding = 1;
		mAttributes[mAttribCount].format = format;
		mAttributes[mAttribCount].offset = offset;
		mAttribCount++;

		mVIStateInfo.vertexAttributeDescriptionCount = mAttribCount;

		return true;
	}


	void NvMeshExtVK::Draw(VkCommandBuffer& cmd, uint32_t instanceCount, uint32_t firstInst)
	{
		VkResult result = VK_SUCCESS;

		// Bind the vertex and index buffers
		VkDeviceSize offsets[] = { 0 };
		vkCmdBindVertexBuffers(cmd, 0, 1, &mVBO(), offsets);
		vkCmdBindIndexBuffer(cmd, mIBO(), 0, VK_INDEX_TYPE_UINT32);

		// Draw the triangle
		vkCmdDrawIndexed(cmd, m_indexCount, instanceCount, 0, 0, firstInst);
	}

	void NvMeshExtVK::Clear()
	{
	}
}
