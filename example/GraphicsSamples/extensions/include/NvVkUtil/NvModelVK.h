//----------------------------------------------------------------------------------
// File:        NvVkUtil/NvModelVK.h
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

#ifndef NVMODELVK_H_
#define NVMODELVK_H_

#include <NvSimpleTypes.h>
#include "NvVkUtil/NvVkContext.h"

#include "NV/NvMath.h"

/// \file
/// VK geometric model handing and rendering

class NvModel;

/// Drawable geometric model using VK vertex buffers.  Supports loading from OBJ file data.
/// Contains positions as well as optional normals, UVs, and tangent vectors
class NvModelVK
{
public:
	/// Create a renderable model from OBJ data
	/// \param[in] vk Vulkan device/queue to render to
	/// \param[in] data pointer to OBJ file data
	/// \param[in] scale the target radius to which we want the model scaled, or <0 if no scaling should be done
	/// \param[in] computeNormals indicate whether per-vertex normals should be estimated and added
	/// \param[in] computeTangents indicate whether per-vertex tangent vectors should be estimated and added
	/// \return a pointer to the new model
	static NvModelVK* CreateFromObj(NvVkContext& vk, uint8_t* data, float scale, bool computeNormals = false, bool computeTangents = false);

	/// Create a renderable model from a preprocessed "NVM" file, which is much faster and more efficient to load than OBJ
	/// \param[in] vk Vulkan device/queue to render to
	/// \param[in] data pointer to NVM file data
	/// \return a pointer to the new model
	static NvModelVK* CreateFromPreprocessed(NvVkContext& vk, uint8_t* data);

	~NvModelVK();

	/// Draw to an open command buffer
	/// \param[in] the command buffer to draw into
	void Draw(VkCommandBuffer& cmd);

	/// Get the index count (for advanced rendering)
	/// \return the index count
	uint32_t getIndexCount() { return mIndexCount; }

	/// Get the low-level geometry data.
	/// Returns the underlying geometry model data instance
	/// \return a pointer to the #NvModel instance that holds the client-memory data
	NvModel *getModel();

	/// Updates the center point of the model
	void computeCenter();

	nv::vec3f m_center; ///< The computed center of the bounding box of the app

	nv::vec3f GetMinExt() const
	{
		return m_minExtent;
	}

	nv::vec3f GetMaxExt() const
	{
		return m_maxExtent;
	}



	/// Get the Vertex input state for creating a rendering pipeline object for this model
	/// \return the VIStateInfo for this model
	VkPipelineVertexInputStateCreateInfo& getVIInfo() { return mVIStateInfo; }

	/// Get the Input Assembly state for creating a rendering pipeline object for this model
	/// \return the IAStateInfo for this model
	VkPipelineInputAssemblyStateCreateInfo& getIAInfo() { return mIAStateInfo; }

	/// useful when doing custom rendering

	VkIndexType getIndexType() const
	{
		return VK_INDEX_TYPE_UINT32;
	}

	const NvVkBuffer& VBO() const
	{
		return mVBO;
	}

	const NvVkBuffer& IBO() const
	{
		return mIBO;
	}

	/// \privatesection

private:
	NvModelVK();
	void initBuffers(NvVkContext& vk);

	NvModel *model;
	nv::vec3f m_minExtent, m_maxExtent, m_radius;
	uint32_t mIndexCount;

	NvVkBuffer mVBO;
	NvVkBuffer mIBO;

	VkVertexInputBindingDescription mVertexBindings;
	VkVertexInputAttributeDescription mAttributes[4];
	VkPipelineVertexInputStateCreateInfo mVIStateInfo;
	VkPipelineInputAssemblyStateCreateInfo mIAStateInfo;
};


#endif /* NVMODELVK_H_ */
