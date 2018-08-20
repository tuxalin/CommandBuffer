//----------------------------------------------------------------------------------
// File:        NvVkUtil/NvModelExtVK.h
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
#ifndef NVGLMODELEXT_VK_H_
#define NVGLMODELEXT_VK_H_

#include "NV/NvMath.h"
#include "NvMeshExtVK.h"
#include "NvMaterialVK.h"
#include "NvModel/NvModelExt.h"
#include <vector>

class NvVkContext;

namespace Nv
{
	class NvModelExt;

	/// \file
	/// VK-specific multi-submesh geometric model handling and rendering
	class NvModelExtVK
	{
	public:
		~NvModelExtVK();

		/// Initialize internal model with passed in ptr
		/// \param[in] vk the VK device/queue to use
		/// \param[in] pSourceModel pointer to an NvModelExt to use for mesh data.  
		/// WARNING!!! This pointer is cached in the object, and must not be freed after this call returns WARNING!!!
		/// \return a pointer to the VK-specific object or NULL on failure
		static NvModelExtVK* Create(NvVkContext& vk, NvModelExt* pSourceModel);

		/// Free the rendering resources held by this model
		/// \param[in] vk the VK device/queue to use
		void Release(NvVkContext& vk);

		/// Get the low-level geometry data.
		/// Returns the underlying geometry model data instance
		/// \return a pointer to the #NvModelExt instance that holds the client-memory data
		NvModelExt* GetModel() { return m_pSourceModel; }

		// Bounding box accessor methods
		/// Returns a vector containing the minimum coordinates of an axis-aligned bounding box for the model
		/// \return Point defining the minimum corner of the model's axis-aligned bounding box
		nv::vec3f GetMinExt() const { return m_pSourceModel->GetMinExt(); }

		/// Returns a vector containing the maximum coordinates of an axis-aligned bounding box for the model
		/// \return Point defining the maximum corner of the model's axis-aligned bounding box
		nv::vec3f GetMaxExt() const { return m_pSourceModel->GetMaxExt(); }

		/// Returns a vector containing the coordinates of the center of the axis-aligned bounding box for the model
		/// \return Point defining the center of the model's axis-aligned bounding box
		nv::vec3f GetCenter() const { return m_pSourceModel->GetCenter(); }

		// Sub-mesh access
		/// Returns the number of meshes that comprise the model
		/// \return Number of meshes contained in the model
		uint32_t GetMeshCount() { return m_meshes.size(); }

		/// Retrieves a mesh from the model
		/// \param[in] meshIndex Index of the mesh within the model to retrieve
		/// \return A pointer to the mesh at the requested index, if there is one.
		NvMeshExtVK* GetMesh(uint32_t meshIndex)
		{
			if (meshIndex >= m_meshes.size())
				return NULL;
			return m_meshes[meshIndex];
		}

		// Material Access
		/// Returns the number of materials used by the model
		/// \return Number of materials defined in the model
		uint32_t GetMaterialCount() { return m_materials.size(); }

		/// Retrieves a material from the model
		/// \param[in] materialIndex Index of the material within the model to retrieve
		/// \return A pointer to the material at the requested index, if there is one.
		NvMaterialVK* GetMaterial(uint32_t materialIndex)
		{
			if (materialIndex >= m_materials.size())
				return NULL;
			return &(m_materials[materialIndex]);
		}

		// Texture Access
		/// Returns the number of textures used by the model
		/// \return Number of textures defined in the model
		uint32_t GetTextureCount() { return m_textures.size(); }

		/// Retrieves a texture from the model
		/// \param[in] textureIndex Index of the texture within the model to retrieve
		/// \return The VK Texture object for the texture at the requested index, if there is one.
		NvVkTexture* GetTexture(uint32_t textureIndex)
		{
			if (textureIndex >= m_textures.size())
				return 0;
			return m_textures[textureIndex];
		}

		VkSampler& GetSampler() { return m_linearSampler; }

		bool InitVertexState();

		bool InstancingEnabled() { return m_instanced; }
		bool EnableInstanceData(uint32_t instanceVertSize);

		bool AddInstanceData(uint32_t location, VkFormat format, uint32_t offset);

		/// Copy the current transforms from bones in the skeleton
		/// to contained meshes in preparation for rendering
		/// \return True if the mesh's transforms could be updated from 
		///         the model's skeleton, false if they could not.
		bool UpdateBoneTransforms();

	private:
		/// \privatesection
		// NvModelExtVK may only be created through the factory method
		NvModelExtVK(NvModelExt* pSourceModel = NULL);

		/// Initialize or update the model geometry by processing the source model
		//// into VK renderable data
		/// \param[in] computeTangents if set to true, then tangent vectors will be computed
		/// to be in the S texture coordinate direction.  This may require vertices to be
		/// duplicated in order to allow multiple tangents at a point.  This can cause model
		/// size explosion, and should be done only if required.  If true, normals will also be
		/// computed, regardless of computeNormals value.
		/// \param[in] computeNormals if set to true, then normal vectors will be computed.
		void PrepareForRendering(NvVkContext& vk, NvModelExt* pModel);

		// Pointer to the original source model that contains the data which the
		// VK model was derived from
		NvModelExt* m_pSourceModel;

		// Array of all sub-meshes within the model
		std::vector<NvMeshExtVK*> m_meshes;

		// Array of all materials used by meshes within the model
		std::vector<NvMaterialVK> m_materials;

		// Current, simple implementation creates a single, linear filtering sampler
		// per model, shared by all textures in that model.
		// We will need a more advanced texture/sampler sharing approach in order to be viable
		VkSampler m_linearSampler;

		// Array of all textures used by meshes within the model
		std::vector<NvVkTexture*> m_textures;

		bool m_instanced;
	};
}
#endif
