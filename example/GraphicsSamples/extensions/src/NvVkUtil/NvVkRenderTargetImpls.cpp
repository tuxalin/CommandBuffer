//----------------------------------------------------------------------------------
// File:        NvVkUtil/NvVkRenderTargetImpls.cpp
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
#include "NvVkRenderTargetImpls.h"

#define ARRAY_SIZE(a) ((sizeof a)/(sizeof a[0]))

NvVkRenderTarget::~NvVkRenderTarget() {
}

bool NvVkRenderTarget::initialize(VkImageLayout layout) {
	VkResult result = VK_ERROR_INITIALIZATION_FAILED;

	VkAttachmentDescription attachments[2] = {};
	attachments[0].format = _targetFormat;
	attachments[0].samples = VK_SAMPLE_COUNT_1_BIT;
	attachments[0].loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
	attachments[0].storeOp = VK_ATTACHMENT_STORE_OP_STORE;
	attachments[0].initialLayout = layout;
	attachments[0].finalLayout = layout;
	attachments[0].flags = 0;

	attachments[1].format = _depthStencilFormat;
	attachments[1].samples = VK_SAMPLE_COUNT_1_BIT;
	attachments[1].loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
	attachments[1].storeOp = VK_ATTACHMENT_STORE_OP_STORE;
	attachments[1].stencilLoadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
	attachments[1].stencilStoreOp = VK_ATTACHMENT_STORE_OP_STORE;
	attachments[1].initialLayout = VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL;
	attachments[1].finalLayout = VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL;
	attachments[1].flags = 0;

	VkSubpassDescription subPass;
	subPass.pipelineBindPoint = VK_PIPELINE_BIND_POINT_GRAPHICS;
	subPass.inputAttachmentCount = 0;
	subPass.pInputAttachments = NULL;

	VkAttachmentReference colorRefs[1] = { { 0, VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL } };
	subPass.colorAttachmentCount = ARRAY_SIZE(colorRefs);
	subPass.pColorAttachments = colorRefs;
	subPass.flags = 0;

	VkAttachmentReference depthRefs[1] = { { 1, VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL } };

	subPass.pDepthStencilAttachment = depthRefs;

	subPass.preserveAttachmentCount = 0;
	subPass.pPreserveAttachments = NULL;
	subPass.pResolveAttachments = NULL;

	VkRenderPassCreateInfo rpInfo = { VK_STRUCTURE_TYPE_RENDER_PASS_CREATE_INFO };
	rpInfo.pAttachments = attachments;
	rpInfo.attachmentCount = ARRAY_SIZE(attachments);
	rpInfo.pSubpasses = &subPass;
	rpInfo.subpassCount = 1;
	rpInfo.pDependencies = NULL;
	rpInfo.dependencyCount = 0;

	result = vkCreateRenderPass(vk().device(), &rpInfo, NULL, &_clearRenderPass);
	CHECK_VK_RESULT();

	attachments[0].loadOp = VK_ATTACHMENT_LOAD_OP_LOAD;
	attachments[1].loadOp = VK_ATTACHMENT_LOAD_OP_LOAD;
	attachments[1].stencilLoadOp = VK_ATTACHMENT_LOAD_OP_LOAD;

	result = vkCreateRenderPass(vk().device(), &rpInfo, NULL, &_copyRenderPass);
	CHECK_VK_RESULT();

	return true;
}


NvVkMultibufferedRenderTarget* NvVkMultibufferedRenderTarget::create(
	NvVkContext& vk, const VkFormat& targetFormat, const VkFormat& depthStencilFormat, bool useSwapChain,
	int32_t w, int32_t h) {
	NvVkMultibufferedRenderTarget* rt = new NvVkMultibufferedRenderTarget(vk);

	rt->_useSwapChain = useSwapChain;
	rt->_targetFormat = targetFormat;
	rt->_depthStencilFormat = depthStencilFormat;

	rt->_swapchain = VK_NULL_HANDLE;

	if (!rt->initialize(useSwapChain 
		? VK_IMAGE_LAYOUT_PRESENT_SRC_KHR : VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL)) {
		delete rt;
		return NULL;
	}


	// only initialize the actual buffers if we have a valid size 
	if (w && h) {
		// only an error to fail the resize if we have a valid surface
		if (!rt->resize(w, h) && rt->_surface) {
			delete rt;
			return NULL;
		}
	}

	return rt;
}

bool NvVkMultibufferedRenderTarget::resize(int32_t& w, int32_t& h) {
	VkResult result;
	VkPhysicalDevice physDev = _vk.physicalDevice();
	VkDevice device = _vk.device();

	uint32_t newWidth = _width;
	uint32_t newHeight = _height;

	VkExtent2D swapchainExtent;
	VkSurfaceCapabilitiesKHR surfCapabilities;

	uint32_t _frameCount;
	VkImage *frameImages = NULL;

	if (_useSwapChain) {
		if (!_surface)
			return false;

		// Check the surface capabilities and formats
		result = vkGetPhysicalDeviceSurfaceCapabilitiesKHR(physDev,
			_surface, &surfCapabilities);

		// width and height are either both -1, or both not -1.
		if (surfCapabilities.currentExtent.width == (uint32_t)-1) {
			// If the surface size is undefined, the size is set to
			// the size of the images requested.
			swapchainExtent.width = newWidth;
			swapchainExtent.height = newHeight;
		}
		else {
			// If the surface size is defined, the swap chain size must match
			swapchainExtent = surfCapabilities.currentExtent;
			newWidth = surfCapabilities.currentExtent.width;
			newHeight = surfCapabilities.currentExtent.height;
		}
	}
	else {
		if (w * h == 0)
			return false;

		newWidth = w;
		newHeight = h;
	}

	if (newWidth == _width && newHeight == _height &&
		_depthStencil.image != VK_NULL_HANDLE)
		return true;

	_width = newWidth;
	_height = newHeight;

	if (_frames.size()) {
		result = vkDeviceWaitIdle(device);
		CHECK_VK_RESULT();
		if (result != VK_SUCCESS)
			return false;

		for (uint32_t i = 0; i < _frames.size(); i++) {
			Frame& frame = _frames[i];

			if (frame._frameBuffer != VK_NULL_HANDLE) {
				vkDestroyFramebuffer(device, frame._frameBuffer, NULL);
				frame._frameBuffer = VK_NULL_HANDLE;

				vkDestroyImage(device, frame._target(), NULL);
				frame._target() = VK_NULL_HANDLE;

				vkDestroyImageView(device, frame._targetView, NULL);
				frame._targetView = VK_NULL_HANDLE;
			}
		}

		while (!_backBuffers.empty())
		{
			BackBuffer frame = _backBuffers.front();

			if (frame._acquireSemaphore != VK_NULL_HANDLE) {
				vkDestroySemaphore(_vk.device(), frame._acquireSemaphore, NULL);
				frame._acquireSemaphore = VK_NULL_HANDLE;
			}

			if (frame._renderSemaphore != VK_NULL_HANDLE) {
				vkDestroySemaphore(_vk.device(), frame._renderSemaphore, NULL);
				frame._renderSemaphore = VK_NULL_HANDLE;
			}

			if (frame._presentFence != VK_NULL_HANDLE) {
				vkDestroyFence(_vk.device(), frame._presentFence, NULL);
				frame._presentFence = VK_NULL_HANDLE;
			}

			_backBuffers.pop();
		}

		while (!_memory.empty())
		{
			VkDeviceMemory mem = _memory.front();

			vkFreeMemory(_vk.device(), mem, NULL);

			_memory.pop();
		}

		vkDestroyImage(vk().device(), _depthStencil(), NULL);
		_depthStencil() = VK_NULL_HANDLE;

		_frames.clear();
		result = vkDeviceWaitIdle(vk().device());
		CHECK_VK_RESULT();
		if (result != VK_SUCCESS)
			return false;
	}

	if (_useSwapChain) {
		uint32_t presentModeCount;
		result = vkGetPhysicalDeviceSurfacePresentModesKHR(physDev,
			_surface, &presentModeCount, NULL);
		CHECK_VK_RESULT();
		if (result != VK_SUCCESS)
			return false;

		VkPresentModeKHR *presentModes = new VkPresentModeKHR[presentModeCount];

		result = vkGetPhysicalDeviceSurfacePresentModesKHR(physDev,
			_surface, &presentModeCount, presentModes);
		CHECK_VK_RESULT();
		if (result != VK_SUCCESS)
			return false;

		VkPresentModeKHR swapchainPresentMode = VK_PRESENT_MODE_FIFO_KHR;
		for (uint32_t i = 0; i < presentModeCount; i++) {
			if (presentModes[i] == VK_PRESENT_MODE_MAILBOX_KHR)
				swapchainPresentMode = VK_PRESENT_MODE_MAILBOX_KHR;
		}

		delete[] presentModes;

		// Determine the number of VkImage's to use in the swap chain (we desire to
		// own only 1 image at a time, besides the images being displayed and
		// queued for display):
		uint32_t desiredNumberOfSwapchainImages =
			surfCapabilities.minImageCount + 1;
		if ((surfCapabilities.maxImageCount > 0) &&
			(desiredNumberOfSwapchainImages > surfCapabilities.maxImageCount)) {
			// Application must settle for fewer images than desired:
			desiredNumberOfSwapchainImages = surfCapabilities.maxImageCount;
		}

		VkSurfaceTransformFlagsKHR preTransform;
		if (surfCapabilities.supportedTransforms &
			VK_SURFACE_TRANSFORM_IDENTITY_BIT_KHR) {
			preTransform = VK_SURFACE_TRANSFORM_IDENTITY_BIT_KHR;
		}
		else {
			preTransform = surfCapabilities.currentTransform;
		}

		VkBool32 supported;
		// TODO pipe through NvVkContext::queueFamilyIndex()
		result = vkGetPhysicalDeviceSurfaceSupportKHR(_vk.physicalDevice(),
			_vk.queueFamilyIndex(), _surface, &supported);
		CHECK_VK_RESULT();
		if (result != VK_SUCCESS)
			return false;

		uint32_t surfFmtCount = 0;
		vkGetPhysicalDeviceSurfaceFormatsKHR(_vk.physicalDevice(), _surface, &surfFmtCount, NULL);

		VkSurfaceFormatKHR* fmts = new VkSurfaceFormatKHR[surfFmtCount];
		vkGetPhysicalDeviceSurfaceFormatsKHR(_vk.physicalDevice(), _surface, &surfFmtCount, fmts);

		VkSurfaceFormatKHR* match = fmts;
		for (uint32_t i = 0; i < surfFmtCount; i++) {
			if (_targetFormat == fmts[i].format) {
				match = fmts + i;
				break;
			}
		}

		_targetFormat = match->format;

		VkSwapchainCreateInfoKHR swapchain = { VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR };
		swapchain.pNext = NULL;
		swapchain.surface = _surface;
		swapchain.minImageCount = desiredNumberOfSwapchainImages;
		swapchain.imageFormat = match->format;
		swapchain.imageColorSpace = match->colorSpace;
		swapchain.imageExtent.width = swapchainExtent.width;
		swapchain.imageExtent.height = swapchainExtent.height;
		swapchain.imageUsage = vk().configuration().mainTargetUsageFlags;
		swapchain.preTransform = (VkSurfaceTransformFlagBitsKHR)preTransform;
		swapchain.compositeAlpha = VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR;
		swapchain.imageArrayLayers = 1;
		swapchain.imageSharingMode = VK_SHARING_MODE_EXCLUSIVE;
		swapchain.queueFamilyIndexCount = 0; // TODO pipe through NvVkContext::queueFamilyIndex()
		swapchain.pQueueFamilyIndices = NULL; // TODO pipe through NvVkContext::queueFamilyIndex()
		swapchain.presentMode = swapchainPresentMode;
		swapchain.clipped = true;
		swapchain.oldSwapchain = _swapchain;

		VkSwapchainKHR oldSwapchain = _swapchain;

		result = vkCreateSwapchainKHR(device, &swapchain, NULL, &_swapchain);
		CHECK_VK_RESULT();
		if (result != VK_SUCCESS)
			return false;
		/*
		// If we just re-created an existing swapchain, we should destroy the old
		// swapchain at this point.
		// Note: destroying the swapchain also cleans up all its associated
		// presentable images once the platform is done with them.
		if (oldSwapchain != VK_NULL_HANDLE) {
		vkDestroySwapchainKHR(device, oldSwapchain, NULL);
		}
		*/
		result = vkGetSwapchainImagesKHR(device, _swapchain, &_frameCount, NULL);
		CHECK_VK_RESULT();
		if (result != VK_SUCCESS)
			return false;

		frameImages = new VkImage[_frameCount];

		result = vkGetSwapchainImagesKHR(device, _swapchain, &_frameCount, frameImages);
		CHECK_VK_RESULT();
		if (result != VK_SUCCESS)
			return false;
	}
	else
	{
		_frameCount = 2;

		frameImages = new VkImage[_frameCount];

		for (uint32_t i = 0; i < _frameCount; i++) {
			// Create the render targets
			VkImageCreateInfo cbImageInfo = { VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO };
			cbImageInfo.imageType = VK_IMAGE_TYPE_2D;
			cbImageInfo.format = _targetFormat;
			cbImageInfo.extent.width = _width;
			cbImageInfo.extent.height = _height;
			cbImageInfo.extent.depth = 1;
			cbImageInfo.mipLevels = 1;
			cbImageInfo.arrayLayers = 1;
			cbImageInfo.samples = VK_SAMPLE_COUNT_1_BIT;
			cbImageInfo.tiling = VK_IMAGE_TILING_OPTIMAL;
			cbImageInfo.usage = vk().configuration().mainTargetUsageFlags;
			cbImageInfo.flags = 0;

			NvVkImage image;
			result = vk().createImage(cbImageInfo, image, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT);
			CHECK_VK_RESULT();

			frameImages[i] = image();
			_memory.push(image.mem);

			_vk.transitionImageLayout(frameImages[i],
				VK_IMAGE_ASPECT_COLOR_BIT,
				VK_IMAGE_LAYOUT_UNDEFINED,
				VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL);
		}
	}

	VkImageCreateInfo dsImageInfo = { VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO };
	dsImageInfo.imageType = VK_IMAGE_TYPE_2D;
	dsImageInfo.format = _depthStencilFormat;
	dsImageInfo.extent.width = _width;
	dsImageInfo.extent.height = _height;
	dsImageInfo.extent.depth = 1;
	dsImageInfo.mipLevels = 1;
	dsImageInfo.arrayLayers = 1;
	dsImageInfo.samples = VK_SAMPLE_COUNT_1_BIT;
	dsImageInfo.tiling = VK_IMAGE_TILING_OPTIMAL;
	dsImageInfo.usage = vk().configuration().mainDepthStencilUsageFlags;
	dsImageInfo.flags = 0;
	dsImageInfo.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;

	result = vk().createImage(dsImageInfo, _depthStencil, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT);
	CHECK_VK_RESULT();
	if (result != VK_SUCCESS)
		return false;

	_vk.transitionImageLayout(_depthStencil.image,
		VK_IMAGE_ASPECT_DEPTH_BIT | VK_IMAGE_ASPECT_STENCIL_BIT,
		VK_IMAGE_LAYOUT_UNDEFINED,
		VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL,
		VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_WRITE_BIT,
		VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_WRITE_BIT);

	VkImageViewCreateInfo dsImageViewInfo = { VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO };
	dsImageViewInfo.format = dsImageInfo.format;
	dsImageViewInfo.components.r = VK_COMPONENT_SWIZZLE_R;
	dsImageViewInfo.components.g = VK_COMPONENT_SWIZZLE_G;
	dsImageViewInfo.components.b = VK_COMPONENT_SWIZZLE_B;
	dsImageViewInfo.components.a = VK_COMPONENT_SWIZZLE_A;
	dsImageViewInfo.flags = 0;
	dsImageViewInfo.subresourceRange.levelCount = 1;
	dsImageViewInfo.subresourceRange.baseMipLevel = 0;
	dsImageViewInfo.subresourceRange.layerCount = 1;
	dsImageViewInfo.subresourceRange.baseArrayLayer = 0;
	dsImageViewInfo.subresourceRange.aspectMask = VK_IMAGE_ASPECT_STENCIL_BIT | VK_IMAGE_ASPECT_DEPTH_BIT;

	dsImageViewInfo.image = _depthStencil();
	result = vkCreateImageView(vk().device(), &dsImageViewInfo, NULL, &_dsView);
	CHECK_VK_RESULT();
	if (result != VK_SUCCESS)
		return false;

	_frames.resize(_frameCount, Frame());

	for (uint32_t i = 0; i < _frameCount; i++) {
		Frame& frame = _frames[i];
		VkImageViewCreateInfo viewInfo;
		viewInfo.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
		viewInfo.pNext = NULL;
		viewInfo.format = _targetFormat;
		viewInfo.components.r = VK_COMPONENT_SWIZZLE_R;
		viewInfo.components.g = VK_COMPONENT_SWIZZLE_G;
		viewInfo.components.b = VK_COMPONENT_SWIZZLE_B;
		viewInfo.components.a = VK_COMPONENT_SWIZZLE_A;

		viewInfo.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
		viewInfo.subresourceRange.baseMipLevel = 0;
		viewInfo.subresourceRange.levelCount = 1;
		viewInfo.subresourceRange.baseArrayLayer = 0;
		viewInfo.subresourceRange.layerCount = 1;
		viewInfo.viewType = VK_IMAGE_VIEW_TYPE_2D;
		viewInfo.flags = 0;

		frame._target.image = frameImages[i];

		_vk.transitionImageLayout(frame._target.image,
			VK_IMAGE_ASPECT_COLOR_BIT,
			VK_IMAGE_LAYOUT_UNDEFINED,
			VK_IMAGE_LAYOUT_PRESENT_SRC_KHR);

		viewInfo.image = frameImages[i];

		result = vkCreateImageView(device, &viewInfo, NULL, &frame._targetView);
		CHECK_VK_RESULT();
		if (result != VK_SUCCESS)
			return false;

		VkImageView bindInfos[2];
		bindInfos[0] = frame._targetView;
		bindInfos[1] = _dsView;

		VkFramebufferCreateInfo fbInfo = { VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO };
		fbInfo.attachmentCount = 2;
		fbInfo.pAttachments = bindInfos;
		fbInfo.width = _width;
		fbInfo.height = _height;
		fbInfo.layers = 1;
		fbInfo.renderPass = _clearRenderPass;

		result = vkCreateFramebuffer(vk().device(), &fbInfo, NULL, &frame._frameBuffer);
		CHECK_VK_RESULT();
		if (result != VK_SUCCESS)
			return false;
	}

	uint32_t backbufferCount = _useSwapChain ? (_frameCount + 1) : _frameCount;
	for (uint32_t i = 0; i < backbufferCount; i++)
	{
		BackBuffer frame;

		// Create the present semaphores
		{
			VkSemaphoreCreateInfo createInfo;
			createInfo.sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO;
			createInfo.pNext = nullptr;
			createInfo.flags = 0;
			VkResult result = vkCreateSemaphore(_vk.device(), &createInfo, nullptr, &frame._acquireSemaphore);
			result = vkCreateSemaphore(_vk.device(), &createInfo, nullptr, &frame._renderSemaphore);
		}

		frame._presentFence = VK_NULL_HANDLE;
		frame._index = i;

		_backBuffers.push(frame);
	}

	delete[] frameImages;

	result = vkDeviceWaitIdle(device);
	CHECK_VK_RESULT();
	if (result != VK_SUCCESS)
		return false;

	return true;
}

bool NvVkMultibufferedRenderTarget::beginFrame() {
	BackBuffer& frame = _backBuffers.front();
	VkResult result = VK_SUCCESS;

	if (_useSwapChain) {
		if (frame._presentFence == VK_NULL_HANDLE)
		{
			VkFenceCreateInfo fenceInfo = {};
			fenceInfo.sType = VK_STRUCTURE_TYPE_FENCE_CREATE_INFO;
			vkCreateFence(_vk.device(), &fenceInfo, nullptr, &frame._presentFence);
		}
		else
		{
			// wait until acquire and render semaphores are waited/unsignaled
			VkResult fenceStatus = vkGetFenceStatus(_vk.device(), frame._presentFence);
			if (VK_NOT_READY == fenceStatus) {
				result = vkWaitForFences(_vk.device(), 1, &frame._presentFence,
					true, UINT64_MAX);
			}

			// reset the fence
			if (VK_SUCCESS == vkGetFenceStatus(_vk.device(), frame._presentFence)) {
				result = vkResetFences(_vk.device(), 1, &frame._presentFence);
			}
		}

		// Get the index of the next available swapchain image:
		VkResult err = vkAcquireNextImageKHR(_vk.device(), _swapchain, UINT64_MAX,
			frame._acquireSemaphore,
			(VkFence)0,
			&frame._index);

		_currentBuffer = frame;
		_backBuffers.pop();

		if (err == VK_ERROR_OUT_OF_DATE_KHR) {
			int32_t w = 0;
			int32_t h = 0;
			return resize(w, h);
		}
		else if (err == VK_SUBOPTIMAL_KHR) {
		}
		else {
			return err == VK_SUCCESS;
		}
	}
	else
	{
		// index has already been set at initialization
		_currentBuffer = frame;
		_backBuffers.pop();
	}

	return true;
}

bool NvVkMultibufferedRenderTarget::endFrame() {
	BackBuffer& frame = _currentBuffer;

	if (_swapchain) {
		VkPresentInfoKHR present;
		present.sType = VK_STRUCTURE_TYPE_PRESENT_INFO_KHR;
		present.pNext = nullptr;
		present.swapchainCount = 1;
		present.pSwapchains = &_swapchain; //&mInfo.swap_chain;
		present.pImageIndices = &frame._index; //&mInfo.current_buffer;
		present.pWaitSemaphores = &frame._renderSemaphore;
		present.waitSemaphoreCount = 1;
		present.pResults = nullptr;
		VkResult err = vkQueuePresentKHR(_vk.queue(), &present);

		if (VK_SUCCESS != vkQueueSubmit(_vk.queue(), 0, nullptr, frame._presentFence))
			return false;
	}

	_backBuffers.push(frame);
	
	return true;
}

void NvVkMultibufferedRenderTarget::setSurface(VkSurfaceKHR surface) {
	_surface = surface;
	int32_t w = _width;
	int32_t h = _height;
	resize(w, h);
}
