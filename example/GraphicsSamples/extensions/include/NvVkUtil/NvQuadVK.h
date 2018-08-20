//----------------------------------------------------------------------------------
// File:        NvVkUtil/NvQuadVK.h
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

#ifndef NVQUADVK_H_
#define NVQUADVK_H_

#include <NvSimpleTypes.h>
#include "NvVkUtil/NvVkContext.h"

#include "NV/NvMath.h"

/// \file
/// VK quad rendering

/// A pre-made Quad rendering class
class NvQuadVK
{
public:
	/// Create a new quad rendering object
	/// \param[in] the VK queue/device to use
	/// \return the quad object
	static NvQuadVK* Create(NvVkContext& vk);

	~NvQuadVK() { }

	/// Draw the quad
	/// \param[in] the destination command buffer
	void Draw(VkCommandBuffer& cmd);

	/// Get the Vertex input state for creating a rendering pipeline object for this quad
	/// \return the VIStateInfo for this quad
	VkPipelineVertexInputStateCreateInfo& getVIInfo() { return mVIStateInfo; }

	/// Get the Input Assembly state for creating a rendering pipeline object for this quad
	/// \return the IAStateInfo for this qaud
	VkPipelineInputAssemblyStateCreateInfo& getIAInfo() { return mIAStateInfo; }

	/// \privatesection

private:
	NvQuadVK();

	NvVkBuffer mVBO;

	VkVertexInputBindingDescription mVertexBindings;
	VkVertexInputAttributeDescription mAttributes[2];
	VkPipelineVertexInputStateCreateInfo mVIStateInfo;
	VkPipelineInputAssemblyStateCreateInfo mIAStateInfo;
};


#endif /* NVMODELVK_H_ */
