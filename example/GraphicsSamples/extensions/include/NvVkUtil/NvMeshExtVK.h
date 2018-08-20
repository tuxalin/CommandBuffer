//----------------------------------------------------------------------------------
// File:        NvVkUtil/NvMeshExtVK.h
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
#ifndef NVGLMESHEXT_VK_H_
#define NVGLMESHEXT_VK_H_
#pragma once

#include "NV/NvMath.h"
#include "NvVkUtil/NvVkContext.h"
#include <vector>


namespace Nv
{
	class NvModelExt;
	class NvSkeleton;
	class SubMesh;

	/// \file
	/// VK geometric sub-mesh data handling and rendering
	class NvMeshExtVK
	{
	public:
		NvMeshExtVK();
		NvMeshExtVK(const NvMeshExtVK& other);

		~NvMeshExtVK();

		const static int32_t c_MaxBonesPerVertex = 4;
		const static int32_t c_MaxBonesPerMesh = 32;

		/// Get the index count (for advanced rendering)
		/// \return the index count
		uint32_t getIndexCount() { return m_indexCount; }

		/// Returns the ID of the material used by this mesh.  The apps can map this to
		/// actual material info with #NvModelExtVK::GetMaterial
		/// \return index of the current mesh's material in the overall model's material list
		uint32_t GetMaterialID() { return m_materialID; }

		/// Get the Vertex input state for creating a rendering pipeline object for this model
		/// \return the VIStateInfo for this model
		VkPipelineVertexInputStateCreateInfo& getVIInfo() { return mVIStateInfo; }

		/// Get the Input Assembly state for creating a rendering pipeline object for this model
		/// \return the IAStateInfo for this model
		VkPipelineInputAssemblyStateCreateInfo& getIAInfo() { return mIAStateInfo; }

		/// \privatesection
		// Initialize mesh data from the given sub-mesh in the model
		bool InitFromSubmesh(NvVkContext& vk, NvModelExt* pModel,
			uint32_t subMeshID);

		bool UpdateBoneTransforms(Nv::NvSkeleton* pSrcSkel);
		
		/// Returns the model-relative transform of the mesh
		const nv::matrix4f& GetMeshOffset() const { return m_offsetMatrix; }

		/// Builds commands into the given command buffer to render
		/// this mesh
		/// \param[in] cmd CommandBuffer object to append the mesh's draw commands to.
		/// \param[in] instanceCount Number of instances to render. (if >= 1, uses DrawElementsInstanced,
		///                but if 0, uses DrawElements()
		/// \param[in] firstInst starting instance offset to use
		void Draw(VkCommandBuffer& cmd, uint32_t instanceCount = 1, uint32_t firstInst = 0);

		bool EnableInstanceData(uint32_t instanceVertSize);

		// Binding = 1
		bool AddInstanceData(uint32_t location, VkFormat format, uint32_t offset);

		bool InitVertexState();

		enum { MAX_ATTRIB_COUNT = 16 };

	private:
		/// \privatesection
		NvMeshExtVK &operator =(const NvMeshExtVK &) { return *this; }

		void Clear();

		SubMesh* m_pSrcMesh;

		// Index of the material used by this mesh
		uint32_t m_materialID;

		// GL IDs of the buffer objects
		uint32_t m_indexCount;

		uint32_t m_vertexCount;
		int32_t m_vertexSize;       // in floats

		int32_t m_positionSize;     // in floats
		int32_t m_positionOffset;   // in bytes

		int32_t m_normalSize;       // in floats
		int32_t m_normalOffset;     // in bytes

		int32_t m_texCoordSize;     // in floats
		int32_t m_texCoordOffset;   // in bytes

		int32_t m_tangentSize;      // in floats
		int32_t m_tangentOffset;    // in bytes

		int32_t m_colorSize;        // in floats
		int32_t m_colorOffset;      // in bytes

		int32_t m_boneSize;         // in floats
		int32_t m_boneIndexOffset;  // in bytes

		int32_t m_weightSize;       // in floats
		int32_t m_weightOffset;     // in bytes

		int32_t m_parentNode;
		nv::matrix4f m_offsetMatrix;

		NvVkBuffer mVBO;
		NvVkBuffer mIBO;

		uint32_t mBindingCount;
		uint32_t mAttribCount;
		VkVertexInputBindingDescription mVertexBindings[4];
		VkVertexInputAttributeDescription mAttributes[MAX_ATTRIB_COUNT];
		VkPipelineVertexInputStateCreateInfo mVIStateInfo;
		VkPipelineInputAssemblyStateCreateInfo mIAStateInfo;
	};
}
#endif
