//----------------------------------------------------------------------------------
// File:        NvVkUtil/NvGLFWContextVK.cpp
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
#ifndef ANDROID
#include "NvGLFWContextVK.h"
#include "NvVkRenderTargetImpls.h"
#include <string.h>

NvVkRenderTarget* NvGLFWContextVK::mainRenderTarget() {
	return mSwapchainRenderTarget;
}

bool NvGLFWContextVK::initRenderTarget() {
	mSwapchainRenderTarget = NvVkMultibufferedRenderTarget::create(*this, m_cbFormat, m_dsFormat, !mUseFBOPair, mFBOWidth, mFBOHeight);

	if (mSwapchainRenderTarget) {
		if (!mUseFBOPair) {
			VkSurfaceKHR surface;
			extern VkResult glfwCreateWindowSurface(VkInstance instance, GLFWwindow* window, const VkAllocationCallbacks* allocator, VkSurfaceKHR* surface);
			glfwCreateWindowSurface(_instance, mWindow, NULL, &surface);

			mSwapchainRenderTarget->setSurface(surface);
		}
		return true;
	}
	else {
		return false;
	}
}

bool NvGLFWContextVK::readFramebufferRGBX32(uint8_t *dest, int32_t& w, int32_t& h) {
	NvVkRenderTarget& rt = *mainRenderTarget();

	w = rt.width();
	h = rt.height();

	if (!dest)
		return true;

	VkFormat format = rt.targetFormat();

	if (format == VK_FORMAT_R8G8B8A8_UNORM) {
		uint32_t size = 4 * w * h;

		VkBufferImageCopy region;
		region.bufferOffset = 0;
		region.bufferRowLength = 0;
		region.bufferImageHeight = 0;
		region.imageSubresource.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
		region.imageSubresource.baseArrayLayer = 0;
		region.imageSubresource.layerCount = 1;
		region.imageSubresource.mipLevel = 0;
		region.imageOffset = { 0, 0, 0 };
		region.imageExtent = { (uint32_t)w, (uint32_t)h, 1 };

		NvVkBuffer dstBuffer;
		VkResult result = createAndFillBuffer(size, VK_BUFFER_USAGE_FLAG_BITS_MAX_ENUM, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT, dstBuffer);
		
		VkCommandBuffer cmd = beginTempCmdBuffer();

		vkCmdCopyImageToBuffer(cmd,
			rt.image(),
			VK_IMAGE_LAYOUT_GENERAL,
			dstBuffer.buffer,
			1, &region);

		result = doneWithTempCmdBufferSubmit(cmd);

		vkDeviceWaitIdle(device());

		uint8_t* ptr = NULL;
		result = vkMapMemory(device(), dstBuffer.mem, 0, size, 0, (void**)&ptr);

		uint32_t rowSize = w * 4;
		ptr += rowSize * (h - 1);

		for (int32_t i = 0; i < h; i++) {
			memcpy(dest, ptr, rowSize);
			dest += rowSize;
			ptr -= rowSize;
		}

		return true;
	}

	return false;
}

#endif
