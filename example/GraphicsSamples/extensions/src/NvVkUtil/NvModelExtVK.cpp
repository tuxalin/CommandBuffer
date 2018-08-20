//----------------------------------------------------------------------------------
// File:        NvVkUtil/NvModelExtVK.cpp
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

#include "NvVkUtil/NvModelExtVK.h"
#include "NvModel/NvModelExt.h"
#include "../../src/NvModel/NvModelExtObj.h"

namespace Nv
{

	NvModelExtVK* NvModelExtVK::Create(NvVkContext& vk, NvModelExt* pSourceModel)
	{
		if (NULL == pSourceModel)
		{
			return NULL;
		}

		NvModelExtVK* model = new NvModelExtVK(pSourceModel);
		model->m_pSourceModel = pSourceModel;
		model->PrepareForRendering(vk, pSourceModel);
		return model;
	}

	void NvModelExtVK::Release(NvVkContext& vk)
	{
	}

	NvModelExtVK::NvModelExtVK(NvModelExt* pSourceModel) :
		m_pSourceModel(pSourceModel),
		m_instanced(false)
	{
		NV_ASSERT(NULL != pSourceModel);
	}

	NvModelExtVK::~NvModelExtVK()
	{
		if (m_pSourceModel)
		{
			delete m_pSourceModel;
		}
	};

	bool NvModelExtVK::UpdateBoneTransforms()
	{
		std::vector<NvMeshExtVK*>::iterator meshIt = m_meshes.begin();
		std::vector<NvMeshExtVK*>::iterator meshEnd = m_meshes.end();
		bool result = true;

		for (; meshIt != meshEnd; ++meshIt)
		{
			result &= (*meshIt)->UpdateBoneTransforms(GetModel()->GetSkeleton());
		}
		return result;
	}

	void NvModelExtVK::PrepareForRendering(NvVkContext& vk,
		NvModelExt* pModel)
	{
		VkResult result;

		VkSamplerCreateInfo samplerCreateInfo = { VK_STRUCTURE_TYPE_SAMPLER_CREATE_INFO };
		samplerCreateInfo.magFilter = VK_FILTER_LINEAR;
		samplerCreateInfo.minFilter = VK_FILTER_LINEAR;
		samplerCreateInfo.mipmapMode = VK_SAMPLER_MIPMAP_MODE_LINEAR;
		samplerCreateInfo.addressModeU = VK_SAMPLER_ADDRESS_MODE_REPEAT;
		samplerCreateInfo.addressModeV = VK_SAMPLER_ADDRESS_MODE_REPEAT;
		samplerCreateInfo.addressModeW = VK_SAMPLER_ADDRESS_MODE_REPEAT;
		samplerCreateInfo.mipLodBias = 0.0;
		samplerCreateInfo.maxAnisotropy = 1;
		samplerCreateInfo.compareOp = VK_COMPARE_OP_NEVER;
		samplerCreateInfo.minLod = 0.0;
		samplerCreateInfo.maxLod = 16.0;
		samplerCreateInfo.borderColor = VK_BORDER_COLOR_FLOAT_OPAQUE_WHITE;

		result = vkCreateSampler(vk.device(), &samplerCreateInfo, 0, &m_linearSampler);

		// Get GL usable versions of all the textures used by the model
		uint32_t textureCount = m_pSourceModel->GetTextureCount();
		m_textures.resize(textureCount);
		for (uint32_t textureIndex = 0; textureIndex < textureCount; ++textureIndex)
		{
			NvVkTexture* t = new NvVkTexture;
			if (vk.uploadTextureFromFile(m_pSourceModel->GetTextureName(textureIndex).c_str(), *t)) {
				m_textures[textureIndex] = t;
			}
		}

		// Get VK usable versions of all the materials in the model
		uint32_t materialCount = pModel->GetMaterialCount();
		m_materials.resize(materialCount);
		if (materialCount > 0)
		{
			for (uint32_t materialIndex = 0; materialIndex < materialCount; ++materialIndex)
			{
				m_materials[materialIndex].InitFromMaterial(m_pSourceModel, materialIndex);
			}
		}

		// Get VK renderable versions of all meshes in the model
		uint32_t meshCount = pModel->GetMeshCount();
		m_meshes.resize(meshCount);
		if (meshCount > 0)
		{
			for (uint32_t meshIndex = 0; meshIndex < meshCount; ++meshIndex)
			{
				m_meshes[meshIndex] = new NvMeshExtVK;
				m_meshes[meshIndex]->InitFromSubmesh(vk, pModel, meshIndex);
			}
		}

		InitVertexState();
	}

	bool NvModelExtVK::EnableInstanceData(uint32_t instanceVertSize) {
		uint32_t meshCount = GetMeshCount();
		for (uint32_t meshIndex = 0; meshIndex < meshCount; ++meshIndex)
		{
			if (!GetMesh(meshIndex)->EnableInstanceData(instanceVertSize))
				return false;
		}

		m_instanced = true;
		return true;
	}

	bool NvModelExtVK::AddInstanceData(uint32_t location, VkFormat format, uint32_t offset) {
		uint32_t meshCount = GetMeshCount();
		for (uint32_t meshIndex = 0; meshIndex < meshCount; ++meshIndex)
		{
			if (!GetMesh(meshIndex)->AddInstanceData(location, format, offset))
				return false;
		}

		return true;
	}

	bool NvModelExtVK::InitVertexState() {
		uint32_t meshCount = GetMeshCount();
		for (uint32_t meshIndex = 0; meshIndex < meshCount; ++meshIndex)
		{
			if (!GetMesh(meshIndex)->InitVertexState())
				return false;
		}

		return true;
	}
}
