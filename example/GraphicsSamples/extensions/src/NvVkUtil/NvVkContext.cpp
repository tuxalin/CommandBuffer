//----------------------------------------------------------------------------------
// File:        NvVkUtil/NvVkContext.cpp
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
#include "NvVkContext.h"
#include "NvAssetLoader/NvAssetLoader.h"
#include "NvImage/NvImage.h"
#include "NvVkUtil/NvGPUTimerVK.h"

#include "NV/NvLogs.h"
#include <NvAssert.h>

#include <vector>
#include <string>
#include <set>
#include <algorithm>

using std::vector;
using std::string;
using std::set;

#define ARRAY_SIZE(a) ( sizeof(a) / sizeof( (a)[0] ))

bool NvVkContext::reshape(int32_t& w, int32_t& h) {
	VkResult result;

	mainRenderTarget()->resize(w, h);

	result = vkDeviceWaitIdle(device());
	CHECK_VK_RESULT();
	if (result != VK_SUCCESS)
		return false;

	return true;
}

bool NvVkContext::nextFrame() {
	int current = mCurrFrameIndex % MAX_BUFFERED_FRAMES;

	VkCommandBuffer cmd = mFrames[current].mCmdBuffers;

	if (mFrames[current].mFence == VK_NULL_HANDLE) {
		VkFenceCreateInfo info = { VK_STRUCTURE_TYPE_FENCE_CREATE_INFO };
		VkResult result = vkCreateFence(_device, &info, NULL, &mFrames[current].mFence);
		if (result != VK_SUCCESS)
			return false;
	}

	VkSemaphore* wait = NULL;
	VkSemaphore* signal = NULL;

	VkResult result = vkEndCommandBuffer(cmd);
	CHECK_VK_RESULT();
	if (result != VK_SUCCESS)
		return false;

	mFrames[current].mOpen = false;

	signal = getSignalSync();
	wait = getWaitSync();

	VkSubmitInfo submitInfo = { VK_STRUCTURE_TYPE_SUBMIT_INFO };
	submitInfo.commandBufferCount = 1;
	submitInfo.pCommandBuffers = &cmd;

	submitInfo.waitSemaphoreCount = wait ? 1 : 0;
	submitInfo.pWaitSemaphores = wait;
	submitInfo.signalSemaphoreCount = signal ? 1 : 0;
	submitInfo.pSignalSemaphores = signal;

	VkPipelineStageFlags waitStageFlags[2] = { VK_PIPELINE_STAGE_ALL_GRAPHICS_BIT, VK_PIPELINE_STAGE_ALL_GRAPHICS_BIT };
	
	if (submitInfo.waitSemaphoreCount + submitInfo.signalSemaphoreCount)
		submitInfo.pWaitDstStageMask = waitStageFlags;

	result = vkQueueSubmit(queue(), 1, &submitInfo, VK_NULL_HANDLE);
	CHECK_VK_RESULT();
	if (result != VK_SUCCESS)
		return false;

	// Fence was rest in getMainCmdBuffer
	result = vkQueueSubmit(queue(), 0, NULL, mFrames[current].mFence);
	CHECK_VK_RESULT();
	if (result != VK_SUCCESS)
		return false;

	destroyPastFrameCmdBuffers();
	mCurrFrameIndex++;
	return true;
}

// Retrieves a reference to a managed, one-per-frame primary command buffer
// Do not submit directly to a queue - use submitMainCommandBuffer
VkCommandBuffer NvVkContext::getMainCommandBuffer(CmdBufferIndex index)
{
	int current = mCurrFrameIndex % MAX_BUFFERED_FRAMES;

	VkCommandBuffer cmd = mFrames[current].mCmdBuffers;
	if (index != CMD_UI_FRAMEWORK && !mFrames[current].mOpen) {
		VkResult result;
		if (mFrames[current].mFence != VK_NULL_HANDLE) {
			result = vkWaitForFences(device(), 1, &(mFrames[current].mFence), VK_FALSE, 0);

			if (result != VK_SUCCESS) {
				if (result == VK_TIMEOUT) {
//					LOGE("TIMEOUT!!!!");
					result = vkWaitForFences(device(), 1, &(mFrames[current].mFence), VK_FALSE, 0xffffffffffffffff);
					CHECK_VK_RESULT();
				}
			}

			result = vkResetFences(device(), 1, &(mFrames[current].mFence));
			CHECK_VK_RESULT();
		}

		// Record the commands (resets the buffer)
		VkCommandBufferBeginInfo beginInfo = { VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO };
		beginInfo.flags = VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT | VK_COMMAND_BUFFER_USAGE_SIMULTANEOUS_USE_BIT;
		result = vkBeginCommandBuffer(cmd, &beginInfo);
		CHECK_VK_RESULT();
		if (result != VK_SUCCESS)
			return 0;

		mFrames[current].mOpen = true;


		m_frameTimer->start(cmd);
	}

	return cmd;
}

// Submits this frame's main command buffer and does any end-of-frame sync
bool NvVkContext::submitMainCommandBuffer(CmdBufferIndex index)
{
	int current = mCurrFrameIndex % MAX_BUFFERED_FRAMES;

	VkCommandBuffer cmd = mFrames[current].mCmdBuffers;
	
	if (index == CMD_ONSCREEN)
		m_frameTimer->stop(cmd);
	
	return true;
}

VkCommandBuffer NvVkContext::createCmdBuffer(VkCommandPool pool, bool primary)
{
	VkResult result;

	// Create the command buffer.
	VkCommandBufferAllocateInfo cmdInfo = { VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO };
	cmdInfo.commandPool = pool;
	if (!cmdInfo.commandPool)
		return 0;

	cmdInfo.level = primary ? VK_COMMAND_BUFFER_LEVEL_PRIMARY 
		: VK_COMMAND_BUFFER_LEVEL_SECONDARY;
	cmdInfo.commandBufferCount = 1;
	VkCommandBuffer cmd;
	result = vkAllocateCommandBuffers(_device, &cmdInfo, &cmd);
	if (result != VK_SUCCESS)
		return 0;

	return cmd;
}

size_t NvVkStagingBuffer::append(size_t sz, const void* data)
{
	if (mUsed + sz > mAllocated){
		size_t oldsize = mAllocated;

		if (mAllocated){
			deinit();
		}

		mAllocated = std::max(sz, std::min(oldsize * 2, size_t(1024 * 1024 * 32)));

		VkResult result;
		// Create staging buffer
		VkBufferCreateInfo bufferStageInfo = { VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO };
		bufferStageInfo.size = mAllocated;
		bufferStageInfo.usage = VK_BUFFER_USAGE_TRANSFER_SRC_BIT;
		bufferStageInfo.flags = 0;

		result = vkCreateBuffer(mVk->device(), &bufferStageInfo, NULL, &mBuffer());
		if (result != VK_SUCCESS)
			return -1;

		result = mVk->allocMemAndBindBuffer(mBuffer, (VkFlags)VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT);
		if (result != VK_SUCCESS)
			return -1;

		result = vkMapMemory(mVk->device(), mBuffer.mem, 0, mAllocated, 0, (void**)&mMapping);
		if (result != VK_SUCCESS)
			return -1;

		mUsed = 0;
	}

	size_t offset = mUsed;
	memcpy(&mMapping[offset], data, sz);
	mUsed += sz;

	return offset;
}

void NvVkStagingBuffer::deinit()
{
	if (mAllocated){
		vkUnmapMemory(mVk->device(), mBuffer.mem);
		vkDestroyBuffer(mVk->device(), mBuffer(), NULL);
		vkFreeMemory(mVk->device(), mBuffer.mem, NULL);
		mMapping = NULL;
		mAllocated = 0;
	}
}

void checkVkResult(const char* file, int32_t line, VkResult result)
{

#define STR(a) case a: errorString = #a; break;

	if (result != VK_SUCCESS)
	{
		const char* errorString = 0;
		switch (result)
		{

			//STR(VK_SUCCESS);
			STR(VK_NOT_READY);
			STR(VK_TIMEOUT);
			STR(VK_EVENT_SET);
			STR(VK_EVENT_RESET);
			STR(VK_ERROR_INITIALIZATION_FAILED);
			STR(VK_ERROR_OUT_OF_HOST_MEMORY);
			STR(VK_ERROR_OUT_OF_DEVICE_MEMORY);
			STR(VK_ERROR_DEVICE_LOST);
			STR(VK_ERROR_MEMORY_MAP_FAILED);
			STR(VK_ERROR_INCOMPATIBLE_DRIVER);
		default: errorString = "unknown error"; break;
		}

#undef STR
		LOGE("VK error: %s, line %d: %s\n", file, line, errorString);
		NV_ASSERT(result == VK_SUCCESS);
		result = VK_SUCCESS; // nice place to hang a breakpoint in compiler... :)
	}
}

VkResult NvVkContext::allocMemAndBindImage(NvVkImage& image, VkFlags memProps)
{
	VkResult result;
	VkPhysicalDeviceMemoryProperties& memoryProperties = physicalDeviceMemoryProperties();

	VkMemoryRequirements  memReqs;
	vkGetImageMemoryRequirements(device(), image(), &memReqs);

	if (!memReqs.size){
		return VK_SUCCESS;
	}

	// Find an available memory type that satisfies the requested properties.
	uint32_t memoryTypeIndex;
	for (memoryTypeIndex = 0; memoryTypeIndex < memoryProperties.memoryTypeCount; ++memoryTypeIndex) {
		if (((memoryProperties.memoryTypes[memoryTypeIndex].propertyFlags & memProps) == memProps) &&
			((memReqs.memoryTypeBits >> memoryTypeIndex) & 1))  {
			break;
		}
	}
	if (memoryTypeIndex >= memoryProperties.memoryTypeCount) {
		return VK_ERROR_FORMAT_NOT_SUPPORTED;
	}

	VkMemoryAllocateInfo memInfo = {
		VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO,             // sType
		0,                                               // pNext
		memReqs.size,                                    // allocationSize
		memoryTypeIndex,                                 // memoryTypeIndex
	};

	result = vkAllocateMemory(device(), &memInfo, NULL, &image.mem);
	if (result != VK_SUCCESS) {
		return result;
	}

	result = vkBindImageMemory(device(), image(), image.mem, 0);
	if (result != VK_SUCCESS) {
		return result;
	}

	return VK_SUCCESS;
}

VkResult NvVkContext::createImage(VkImageCreateInfo& info, NvVkImage& image, VkFlags memProps) {
	VkResult result;

	result = vkCreateImage(device(), &info, NULL, &image());
	if (result != VK_SUCCESS)
		return result;

	result = allocMemAndBindImage(image, memProps);
	if (result != VK_SUCCESS)
		return result;

	return result;
}

void imageBarrierCreate(
	VkCommandBuffer& inCmd,
	VkImageLayout inOldLayout,
	VkImageLayout inNewLayout,
	VkImage inImage,
	VkImageSubresourceRange &inSubRange,
	VkAccessFlags inOutputMask = VK_ACCESS_TRANSFER_WRITE_BIT,
	VkAccessFlags inInputMask = VK_ACCESS_TRANSFER_READ_BIT,
	VkAccessFlagBits inSrcAccessMask = VK_ACCESS_COLOR_ATTACHMENT_READ_BIT,
	VkAccessFlagBits inDstAccessMask = VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT,
	VkPipelineStageFlags inSrcStages = VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT,
	VkPipelineStageFlags inDestStages = VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT
	) {
	VkImageMemoryBarrier image_memory_barrier;
	memset(&image_memory_barrier, 0, sizeof(image_memory_barrier));

	image_memory_barrier.sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER;
	image_memory_barrier.pNext = NULL;
	image_memory_barrier.dstAccessMask = inDstAccessMask;
	image_memory_barrier.srcAccessMask = inSrcAccessMask;
	image_memory_barrier.oldLayout = inOldLayout;
	image_memory_barrier.newLayout = inNewLayout;
	image_memory_barrier.image = inImage;
	image_memory_barrier.subresourceRange = inSubRange;
	image_memory_barrier.srcQueueFamilyIndex = 0; // TODO pipe through NvVkContext::queueFamilyIndex()
	image_memory_barrier.dstQueueFamilyIndex = 0; // TODO pipe through NvVkContext::queueFamilyIndex()

	if (inNewLayout == VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL){
		image_memory_barrier.srcAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT;
	}

	if (inNewLayout == VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL){
		image_memory_barrier.dstAccessMask = VK_ACCESS_SHADER_WRITE_BIT | VK_ACCESS_TRANSFER_WRITE_BIT;
	}

	VkPipelineStageFlags srcStages = VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT;
	VkPipelineStageFlags destStages = VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT;

	vkCmdPipelineBarrier(inCmd, srcStages, destStages, false,
		0, NULL, 0, NULL,
		1, &image_memory_barrier);
}

void imageSubresourceRange(
	VkImageSubresourceRange& outRange,
	VkImageAspectFlags inAspect,
	uint32_t inArraySize,
	uint32_t inBaseArraySlice,
	uint32_t inMipLevels,
	uint32_t inBaseMipLevel){
	memset(&outRange, 0, sizeof(VkImageSubresourceRange));
	outRange.aspectMask = inAspect;
	outRange.layerCount = inArraySize;
	outRange.baseArrayLayer = inBaseArraySlice;
	outRange.levelCount = inMipLevels;
	outRange.baseMipLevel = inBaseMipLevel;
}

void imageSetLayoutBarrier(VkCommandBuffer& cmd, VkQueue& queue,
	VkImage& image,
	VkImageAspectFlags aspect,
	VkImageLayout oldLayout,
	VkImageLayout newLayout,
	VkAccessFlagBits inSrcAccessmask = VK_ACCESS_COLOR_ATTACHMENT_READ_BIT,
	VkAccessFlagBits inDstAccessmask = VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT,
	uint32_t inArraySize = 1,
	uint32_t inBaseArraySlice = 0
	) {

	VkImageSubresourceRange subRange;

	imageSubresourceRange(subRange, aspect, inArraySize, inBaseArraySlice, 1, 0);
	imageBarrierCreate(cmd, oldLayout, newLayout, image, subRange, VK_ACCESS_TRANSFER_WRITE_BIT, VK_ACCESS_TRANSFER_READ_BIT, inSrcAccessmask, inDstAccessmask);

}

VkResult NvVkContext::transitionImageLayout(VkImage& image, VkImageAspectFlags aspect,
	VkImageLayout oldLayout, VkImageLayout newLayout, VkAccessFlagBits inSrcAccessmask,	VkAccessFlagBits inDstAccessmask) {
	VkCommandBuffer cmd = beginTempCmdBuffer();

	imageSetLayoutBarrier(cmd, _queue, image, aspect, oldLayout, newLayout, (VkAccessFlagBits)0, inDstAccessmask);
	imageSetLayoutBarrier(cmd, _queue, image, aspect, newLayout, newLayout, inSrcAccessmask, inDstAccessmask);

	return doneWithTempCmdBufferSubmit(cmd);
}


VkFormat NvVkContext::pickOptimalFormat(uint32_t count, const VkFormat *formats, VkFlags properties)
{
	VkFormat format = VK_FORMAT_UNDEFINED;
	VkFormatProperties formatProps = {};
	for (size_t i = 0; i < count; ++i) {
		vkGetPhysicalDeviceFormatProperties(physicalDevice(), formats[i], &formatProps);
		if (formatProps.optimalTilingFeatures & properties) {
			format = formats[i];
			break;
		}
	}
	return format;
}

bool NvVkContext::isExtensionSupported(const char * ext)
{
	return mCombinedExtensionNames.find(ext) != mCombinedExtensionNames.end();
}

bool NvVkContext::isLayerSupported(const char * layer)
{
	return mCombinedLayerNames.find(layer) != mCombinedLayerNames.end();
}

VkResult NvVkContext::allocMemAndBindBuffer(NvVkBuffer& buffer, VkFlags memProps)
{
	VkResult result;
	VkPhysicalDeviceMemoryProperties& memoryProperties = physicalDeviceMemoryProperties();

	VkMemoryRequirements  memReqs;
	vkGetBufferMemoryRequirements(device(), buffer(), &memReqs);

	if (!memReqs.size){
		return VK_SUCCESS;
	}

	// Find an available memory type that satisfies the requested properties.
	uint32_t memoryTypeIndex;
	for (memoryTypeIndex = 0; memoryTypeIndex < memoryProperties.memoryTypeCount; ++memoryTypeIndex) {
		if (((memoryProperties.memoryTypes[memoryTypeIndex].propertyFlags & memProps) == memProps) &&
			((memReqs.memoryTypeBits >> memoryTypeIndex) & 1))  {
			break;
		}
	}
	if (memoryTypeIndex >= memoryProperties.memoryTypeCount) {
		return VK_ERROR_FORMAT_NOT_SUPPORTED;
	}

	VkMemoryAllocateInfo memInfo = {
		VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO,             // sType
		0,                                               // pNext
		memReqs.size,                                    // allocationSize
		memoryTypeIndex,                                 // memoryTypeIndex
	};

	result = vkAllocateMemory(device(), &memInfo, NULL, &buffer.mem);
	if (result != VK_SUCCESS) {
		return result;
	}

	result = vkBindBufferMemory(device(), buffer(), buffer.mem, 0);
	if (result != VK_SUCCESS) {
		return result;
	}

	return VK_SUCCESS;
}

VkResult NvVkContext::fillBuffer(NvVkStagingBuffer* staging, NvVkBuffer& buffer, size_t offset, size_t size, const void* data)
{
	if (!staging) {
		staging = &stagingBuffer();
	}

	if (staging->needSync(size)){
		purgeTempCmdBuffers();
	}

	size_t offsetsrc = staging->append(size, data);

	VkCommandBuffer cmd = beginTempCmdBuffer();

	VkBufferCopy copy;
	copy.size = size;
	copy.dstOffset = offset;
	copy.srcOffset = offsetsrc;
	vkCmdCopyBuffer(cmd, staging->getBuffer(), buffer(), 1, &copy);

	return doneWithTempCmdBufferSubmit(cmd);
}



VkResult NvVkContext::createAndFillBuffer(size_t size, VkFlags usage, VkFlags memProps, NvVkBuffer& buffer, const void* data, NvVkStagingBuffer* staging)
{
	VkResult result;
	VkBufferCreateInfo bufferInfo = { VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO };
	bufferInfo.size = size;
	bufferInfo.usage = usage | VK_BUFFER_USAGE_TRANSFER_DST_BIT;
	bufferInfo.flags = 0;

	result = vkCreateBuffer(device(), &bufferInfo, NULL, &buffer());
	if (result != VK_SUCCESS)
		return result;

	result = allocMemAndBindBuffer(buffer, memProps);
	if (result != VK_SUCCESS)
		return result;

	if (data){
		result = fillBuffer(staging, buffer, 0, size, data);
	}

	return VK_SUCCESS;
}


// TODO  cleanup of resources

VkShaderModule NvVkContext::createShader(const char* shaderSource, VkShaderStageFlagBits inStage)
{
	VkResult result;
	VkShaderModuleCreateInfo shaderModuleInfo = { VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO };
	shaderModuleInfo.codeSize = strlen(shaderSource);
	shaderModuleInfo.pCode = (uint32_t*)shaderSource;

#if 1
	// Workaround for LunarG shader preamble
	const unsigned int* pCode = (const unsigned int*)shaderModuleInfo.pCode;
	if (pCode[0] == 0x07230203 && pCode[0] == 0)
	{
		shaderModuleInfo.codeSize -= sizeof(unsigned int) * 3;
		shaderModuleInfo.pCode = &pCode[3];
	}
#endif

	VkShaderModule shaderModule;
	result = vkCreateShaderModule(device(), &shaderModuleInfo, NULL, &shaderModule);

	return shaderModule;
}

static const char VS_TAG[] = "GLSL_VS";
static const uint32_t VS_INDEX = 0;
static const char FS_TAG[] = "GLSL_FS";
static const uint32_t FS_INDEX = 1;
static const char GS_TAG[] = "GLSL_GS";
static const uint32_t GS_INDEX = 2;
static const char TCS_TAG[] = "GLSL_TCS";
static const uint32_t TCS_INDEX = 3;
static const char TES_TAG[] = "GLSL_TES";
static const uint32_t TES_INDEX = 4;
static const char CS_TAG[] = "GLSL_CS";
static const uint32_t CS_INDEX = 5;

static const VkShaderStageFlagBits stageFlags[] = {
	VK_SHADER_STAGE_VERTEX_BIT,
	VK_SHADER_STAGE_FRAGMENT_BIT,
	VK_SHADER_STAGE_GEOMETRY_BIT,
	VK_SHADER_STAGE_TESSELLATION_CONTROL_BIT,
	VK_SHADER_STAGE_TESSELLATION_EVALUATION_BIT,
	VK_SHADER_STAGE_COMPUTE_BIT
};

uint32_t NvVkContext::createShadersFromSourceString(const std::string& sourceText, VkPipelineShaderStageCreateInfo* shaders, uint32_t maxShaders) {
	const int32_t MAX_STAGES = 6;

	const char* infiles[MAX_STAGES];
	for (uint32_t i = 0; i < MAX_STAGES; i++)
		infiles[i] = NULL;

	// walk the file, looking for tags.  when we find a tag, change the hash to a null char
	// and grab the pointer to the start of the next line for the desired shader

	// we leak this copy for now...
	uint32_t length = sourceText.length() + 1;
	char* ptr = new char[length];
	strncpy(ptr, sourceText.c_str(), length);
	while (ptr[0]) {
		while (ptr[0] && ptr[0] != '#') {
			ptr++;
		}

		if (ptr[0] == '#') {
			char* hash = ptr;
			char* tag = ptr + 1;
			const char** shaderPtrAddr = NULL;
			// found hash.  But is it a shader tag?
			if (!strncmp(tag, VS_TAG, strlen(VS_TAG))) {
				shaderPtrAddr = infiles + VS_INDEX;
			}
			else if (!strncmp(tag, FS_TAG, strlen(FS_TAG))) {
				shaderPtrAddr = infiles + FS_INDEX;
			}
			else if (!strncmp(tag, GS_TAG, strlen(GS_TAG))) {
				shaderPtrAddr = infiles + GS_INDEX;
			}
			else if (!strncmp(tag, TCS_TAG, strlen(TCS_TAG))) {
				shaderPtrAddr = infiles + TCS_INDEX;
			}
			else if (!strncmp(tag, TES_TAG, strlen(TES_TAG))) {
				shaderPtrAddr = infiles + TES_INDEX;
			}
			else if (!strncmp(tag, CS_TAG, strlen(CS_TAG))) {
				shaderPtrAddr = infiles + CS_INDEX;
			}

			// advance the ptr to the end of the line or the end of the string, whichever is first
			while (ptr[0] && ptr[0] != '\n' && ptr[0] != '\r')
				ptr++;

			if (shaderPtrAddr)
				*shaderPtrAddr = ptr;

			// if it is a valid tag, then null out the leading # to close the previous shader
			if (shaderPtrAddr)
				hash[0] = '\0';
		}
	}

	uint32_t numStages = 0;

	for (uint32_t i = 0; i < MAX_STAGES; i++) {
		if (infiles[i]) {
			if (numStages >= maxShaders)
				return 0;

			VkPipelineShaderStageCreateInfo& stage = shaders[numStages];

			stage.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
			stage.pNext = NULL;
			stage.stage = stageFlags[i];
			stage.pName = "main";
			stage.module = createShader(infiles[i], stageFlags[i]);
			stage.pSpecializationInfo = NULL;
			stage.flags = 0;

			numStages++;
		}
	}

	return numStages;
}

uint32_t NvVkContext::createShadersFromBinaryBlob(uint32_t* data, uint32_t leng, VkPipelineShaderStageCreateInfo* shaders, uint32_t maxShaders) {
	const int32_t MAX_STAGES = 6;

	struct StageTableEntry {
		uint32_t offset;
		uint32_t size;
		VkShaderStageFlagBits kind;
	};
	struct BinaryHeader {
		uint8_t tag[8];
		uint32_t count;
		StageTableEntry stages[];
	};

	BinaryHeader& header = *((BinaryHeader*)data);

	if (leng < 8 || memcmp(header.tag, "NVSPRV00", 8))
		return 0;

	if (maxShaders < header.count)
		return 0;

	for (uint32_t i = 0; i < header.count; i++) {
		StageTableEntry& entry = header.stages[i];
		VkPipelineShaderStageCreateInfo& stage = shaders[i];

		stage.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
		stage.pNext = NULL;
		stage.stage = entry.kind;
		stage.pName = "main";
		stage.pSpecializationInfo = NULL;
		stage.flags = 0;

		{
			VkResult result;
			VkShaderModuleCreateInfo shaderModuleInfo = { VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO };
			shaderModuleInfo.codeSize = entry.size;
			shaderModuleInfo.pCode = (uint32_t*)(((uint8_t*)data) + entry.offset);

#if 1
			// Workaround for LunarG shader preamble
			const unsigned int* pCode = (const unsigned int*)shaderModuleInfo.pCode;
			if (pCode[0] == 0x07230203 && pCode[0] == 0)
			{
				shaderModuleInfo.codeSize -= sizeof(unsigned int) * 3;
				shaderModuleInfo.pCode = &pCode[3];
			}
#endif
			result = vkCreateShaderModule(device(), &shaderModuleInfo, NULL, &stage.module);
		}
	}


	return header.count;
}

static VkFormat TranslateNvFormat(const NvImage &i);

bool NvVkContext::uploadTextureFromDDSFile(const char* filename, NvVkTexture& tex) {
	int32_t len;
	char* ddsData = NvAssetLoaderRead(filename, len);

	if (!ddsData)
		return 0;

	bool success = uploadTextureFromDDSData(ddsData, len, tex);

	NvAssetLoaderFree(ddsData);

	return success;
}

bool NvVkContext::uploadTextureFromFile(const char* filename, NvVkTexture& tex) {
    int32_t len;
    NvImage* image = new NvImage;
    char* ddsData = NvAssetLoaderRead(filename, len);

    if (!ddsData)
        return 0;

    const char* ext = filename + strlen(filename) - 1;

    while (ext > filename) {
        if (*ext == '.') {
            ext++;
            break;
        }
        ext--;
    }

    bool success = false;
    if (image->loadImageFromFileData((const uint8_t*)ddsData, len, ext)) {
        success = uploadTexture(image, tex);
    }

    NvAssetLoaderFree(ddsData);

    delete image;

    return success;
}

bool NvVkContext::uploadTextureFromDDSData(const char* ddsData, int32_t length, NvVkTexture& tex) {
	NvImage* image = new NvImage;

	bool success = false;
	if (image->loadImageFromFileData((const uint8_t*)ddsData, length, "dds")) {
		success = uploadTexture(image, tex);
	}

	delete image;

	return success;
}

bool NvVkContext::uploadTexture(const NvImage* image, NvVkTexture& tex) {
	VkResult result;

	if (!image)
		return false;

	VkFormat fmt = TranslateNvFormat(*image);

	if (fmt == VK_FORMAT_UNDEFINED)
		return false;

	int32_t w = image->getWidth();
	int32_t h = image->getHeight();

	VkImageCreateInfo info = { VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO };
	info.imageType = VK_IMAGE_TYPE_2D;
	info.format = fmt;
	info.extent.width = w;
	info.extent.height = h;
	info.extent.depth = 1;
	info.mipLevels = image->getMipLevels();
	info.flags = 0;
	info.samples = VK_SAMPLE_COUNT_1_BIT;
	info.tiling = VK_IMAGE_TILING_OPTIMAL;
	info.usage = VK_IMAGE_USAGE_TRANSFER_DST_BIT | VK_IMAGE_USAGE_SAMPLED_BIT;
	info.pQueueFamilyIndices = NULL;

	if (image->isCubeMap()) {
		info.arrayLayers = 6;
	}
	else {
		info.arrayLayers = 1;
	}

	result = createImage(info, tex(), VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT);
	if (result != VK_SUCCESS)
		return false;

	uint32_t regionCount = info.arrayLayers * info.mipLevels;

	// Create a buffer to hold the texture data before we copy it to the image.
	VkBufferCreateInfo texBufferInfo = { VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO };
	// pad block, as we may need to align the data to 4 bytes
	texBufferInfo.size = image->getDataBlockSize() + 4 * regionCount;
	texBufferInfo.usage = VK_BUFFER_USAGE_TRANSFER_SRC_BIT;
	texBufferInfo.flags = 0;
	NvVkBuffer texBuffer;
	result = vkCreateBuffer(device(), &texBufferInfo, 0, &texBuffer());
	if (result != VK_SUCCESS)
		return false;

	result = allocMemAndBindBuffer(texBuffer, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT);
	if (result != VK_SUCCESS)
		return false;

	void* destPtr = NULL;
	result = vkMapMemory(device(), texBuffer.mem, 0, texBufferInfo.size, 0, &destPtr);
	if (result != VK_SUCCESS)
		return false;

	VkBufferImageCopy* texCopyRegions 
		= new VkBufferImageCopy[regionCount];
	VkBufferImageCopy* rgn = texCopyRegions;

	memset(texCopyRegions, 0, sizeof(VkBufferImageCopy) * regionCount);

	uint32_t offset = 0;
	for (uint32_t layer = 0; layer < info.arrayLayers; layer++) {
		uint32_t width = image->getWidth();
		uint32_t height = image->getHeight();
		for (uint32_t mip = 0; mip < info.mipLevels; mip++) {
			uint32_t srcOffset = image->getLayerLevelOffset(mip, layer);
			uint32_t srcSize = image->getImageSize(mip);
			rgn->bufferOffset = offset;

			memcpy(destPtr, image->getDataBlock() + srcOffset, srcSize);
			uint32_t paddedSize = (srcSize + 3) & (~0x03);
			offset += paddedSize;

			destPtr = ((uint8_t*)destPtr) + paddedSize;

			rgn->imageSubresource.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
			rgn->imageSubresource.baseArrayLayer = layer;
			rgn->imageSubresource.mipLevel = mip;
			rgn->imageSubresource.layerCount = 1;
			rgn->imageOffset.x = 0;
			rgn->imageOffset.y = 0;
			rgn->imageOffset.z = 0;
			rgn->imageExtent.width = width;
			rgn->imageExtent.height = height;
			rgn->imageExtent.depth = 1;
			rgn++;
			height >>= 1;
			width >>= 1;
			height = height ? height : 1;
			width = width ? width : 1;
		}
	}

	vkUnmapMemory(device(), texBuffer.mem);

	VkCommandBuffer cmd = beginTempCmdBuffer();
	{
		// Transition the texture from undefined to transfer destination
		VkImageSubresourceRange subresourceRange = {};
		subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
		subresourceRange.baseMipLevel = 0;
		subresourceRange.levelCount = info.mipLevels;
		subresourceRange.baseArrayLayer = 0;
		subresourceRange.layerCount = info.arrayLayers;

		VkImageMemoryBarrier memBarrier = { VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER };
		memBarrier.srcAccessMask = 0;
		memBarrier.dstAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT;
		memBarrier.oldLayout = VK_IMAGE_LAYOUT_UNDEFINED;
		memBarrier.newLayout = VK_IMAGE_LAYOUT_GENERAL;
		memBarrier.image = tex.image();
		memBarrier.subresourceRange = subresourceRange;
		vkCmdPipelineBarrier(cmd, VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT, VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT, 0, 0, 0, 0, 0, 1, &memBarrier);
	}

	vkCmdCopyBufferToImage(cmd, texBuffer(), tex.image(), VK_IMAGE_LAYOUT_GENERAL, regionCount, texCopyRegions);

	delete[] texCopyRegions;

	// Transition the texture to the shader read state
	{
		VkImageSubresourceRange subresourceRange = {};
		subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
		subresourceRange.baseMipLevel = 0;
		subresourceRange.levelCount = info.mipLevels; 
		subresourceRange.baseArrayLayer = 0;
		subresourceRange.layerCount = info.arrayLayers; // TODO fetch from actual image

		VkImageMemoryBarrier memBarrier = { VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER };
		memBarrier.srcAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT;
		memBarrier.dstAccessMask = VK_ACCESS_SHADER_READ_BIT;
		memBarrier.oldLayout = VK_IMAGE_LAYOUT_GENERAL;
		memBarrier.newLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
		memBarrier.image = tex.image();
		memBarrier.subresourceRange = subresourceRange;
		vkCmdPipelineBarrier(cmd, VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT, VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT, 0, 0, 0, 0, 0, 1, &memBarrier);
	}

	VkFence copyFence;
	VkFenceCreateInfo fenceInfo;
	memset(&fenceInfo, 0, sizeof(fenceInfo));
	fenceInfo.sType = VK_STRUCTURE_TYPE_FENCE_CREATE_INFO;
	result = vkCreateFence(device(), &fenceInfo, NULL, &copyFence);

	doneWithTempCmdBufferSubmit(cmd, &copyFence);

	vkQueueWaitIdle(queue());
	vkWaitForFences(device(), 1, &copyFence, VK_TRUE, 100000000000);

	vkDestroyBuffer(device(), texBuffer(), NULL);
	vkFreeMemory(device(), texBuffer.mem, NULL);

	VkImageViewCreateInfo texViewCreateInfo = { VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO };
	texViewCreateInfo.image = tex.image();
	texViewCreateInfo.viewType = image->isCubeMap() ? VK_IMAGE_VIEW_TYPE_CUBE : VK_IMAGE_VIEW_TYPE_2D;
	texViewCreateInfo.format = fmt;
	texViewCreateInfo.components.r = VK_COMPONENT_SWIZZLE_R;
	texViewCreateInfo.components.g = VK_COMPONENT_SWIZZLE_G;
	texViewCreateInfo.components.b = VK_COMPONENT_SWIZZLE_B;
	texViewCreateInfo.components.a = VK_COMPONENT_SWIZZLE_A;
	texViewCreateInfo.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
	texViewCreateInfo.subresourceRange.baseMipLevel = 0;
	texViewCreateInfo.subresourceRange.levelCount = info.mipLevels;
	texViewCreateInfo.subresourceRange.baseArrayLayer = 0;
	texViewCreateInfo.subresourceRange.layerCount = info.arrayLayers;

	result = vkCreateImageView(device(), &texViewCreateInfo, 0, &tex.view);
	if (result != VK_SUCCESS) {
		return false;
	}

	return true;
}

//
//  Structure defines and constants from nvdds
//
//////////////////////////////////////////////////////////////////////

// surface description flags
const uint32_t DDSF_CAPS = 0x00000001l;
const uint32_t DDSF_HEIGHT = 0x00000002l;
const uint32_t DDSF_WIDTH = 0x00000004l;
const uint32_t DDSF_PITCH = 0x00000008l;
const uint32_t DDSF_PIXELFORMAT = 0x00001000l;
const uint32_t DDSF_MIPMAPCOUNT = 0x00020000l;
const uint32_t DDSF_LINEARSIZE = 0x00080000l;
const uint32_t DDSF_DEPTH = 0x00800000l;

// pixel format flags
const uint32_t DDSF_ALPHAPIXELS = 0x00000001l;
const uint32_t DDSF_FOURCC = 0x00000004l;
const uint32_t DDSF_RGB = 0x00000040l;
const uint32_t DDSF_RGBA = 0x00000041l;

// dwCaps1 flags
const uint32_t DDSF_COMPLEX = 0x00000008l;
const uint32_t DDSF_TEXTURE = 0x00001000l;
const uint32_t DDSF_MIPMAP = 0x00400000l;

// dwCaps2 flags
const uint32_t DDSF_CUBEMAP = 0x00000200l;
const uint32_t DDSF_CUBEMAP_POSITIVEX = 0x00000400l;
const uint32_t DDSF_CUBEMAP_NEGATIVEX = 0x00000800l;
const uint32_t DDSF_CUBEMAP_POSITIVEY = 0x00001000l;
const uint32_t DDSF_CUBEMAP_NEGATIVEY = 0x00002000l;
const uint32_t DDSF_CUBEMAP_POSITIVEZ = 0x00004000l;
const uint32_t DDSF_CUBEMAP_NEGATIVEZ = 0x00008000l;
const uint32_t DDSF_CUBEMAP_ALL_FACES = 0x0000FC00l;
const uint32_t DDSF_VOLUME = 0x00200000l;

// compressed texture types
const uint32_t FOURCC_UNKNOWN = 0;

#ifndef MAKEFOURCC
#define MAKEFOURCC(c0,c1,c2,c3) \
    ((uint32_t)(uint8_t)(c0)| \
    ((uint32_t)(uint8_t)(c1) << 8)| \
    ((uint32_t)(uint8_t)(c2) << 16)| \
    ((uint32_t)(uint8_t)(c3) << 24))
#endif

const uint32_t FOURCC_R8G8B8 = 20;
const uint32_t FOURCC_A8R8G8B8 = 21;
const uint32_t FOURCC_X8R8G8B8 = 22;
const uint32_t FOURCC_R5G6B5 = 23;
const uint32_t FOURCC_X1R5G5B5 = 24;
const uint32_t FOURCC_A1R5G5B5 = 25;
const uint32_t FOURCC_A4R4G4B4 = 26;
const uint32_t FOURCC_R3G3B2 = 27;
const uint32_t FOURCC_A8 = 28;
const uint32_t FOURCC_A8R3G3B2 = 29;
const uint32_t FOURCC_X4R4G4B4 = 30;
const uint32_t FOURCC_A2B10G10R10 = 31;
const uint32_t FOURCC_A8B8G8R8 = 32;
const uint32_t FOURCC_X8B8G8R8 = 33;
const uint32_t FOURCC_G16R16 = 34;
const uint32_t FOURCC_A2R10G10B10 = 35;
const uint32_t FOURCC_A16B16G16R16 = 36;

const uint32_t FOURCC_L8 = 50;
const uint32_t FOURCC_A8L8 = 51;
const uint32_t FOURCC_A4L4 = 52;
const uint32_t FOURCC_DXT1 = 0x31545844l; //(MAKEFOURCC('D','X','T','1'))
const uint32_t FOURCC_DXT2 = 0x32545844l; //(MAKEFOURCC('D','X','T','1'))
const uint32_t FOURCC_DXT3 = 0x33545844l; //(MAKEFOURCC('D','X','T','3'))
const uint32_t FOURCC_DXT4 = 0x34545844l; //(MAKEFOURCC('D','X','T','3'))
const uint32_t FOURCC_DXT5 = 0x35545844l; //(MAKEFOURCC('D','X','T','5'))
const uint32_t FOURCC_ATI1 = MAKEFOURCC('A', 'T', 'I', '1');
const uint32_t FOURCC_ATI2 = MAKEFOURCC('A', 'T', 'I', '2');
const uint32_t FOURCC_BC4U = MAKEFOURCC('B', 'C', '4', 'U');
const uint32_t FOURCC_BC4S = MAKEFOURCC('B', 'C', '4', 'S');
const uint32_t FOURCC_BC5S = MAKEFOURCC('B', 'C', '5', 'S');

const uint32_t FOURCC_D16_LOCKABLE = 70;
const uint32_t FOURCC_D32 = 71;
const uint32_t FOURCC_D24X8 = 77;
const uint32_t FOURCC_D16 = 80;

const uint32_t FOURCC_D32F_LOCKABLE = 82;

const uint32_t FOURCC_L16 = 81;

const uint32_t FOURCC_DX10 = MAKEFOURCC('D', 'X', '1', '0');

// signed normalized formats
const uint32_t FOURCC_Q16W16V16U16 = 110;

// Floating point surface formats

// s10e5 formats (16-bits per channel)
const uint32_t FOURCC_R16F = 111;
const uint32_t FOURCC_G16R16F = 112;
const uint32_t FOURCC_A16B16G16R16F = 113;

// IEEE s23e8 formats (32-bits per channel)
const uint32_t FOURCC_R32F = 114;
const uint32_t FOURCC_G32R32F = 115;
const uint32_t FOURCC_A32B32G32R32F = 116;

//DXGI enums
const uint32_t DDS10_FORMAT_UNKNOWN = 0;
const uint32_t DDS10_FORMAT_R32G32B32A32_TYPELESS = 1;
const uint32_t DDS10_FORMAT_R32G32B32A32_FLOAT = 2;
const uint32_t DDS10_FORMAT_R32G32B32A32_UINT = 3;
const uint32_t DDS10_FORMAT_R32G32B32A32_SINT = 4;
const uint32_t DDS10_FORMAT_R32G32B32_TYPELESS = 5;
const uint32_t DDS10_FORMAT_R32G32B32_FLOAT = 6;
const uint32_t DDS10_FORMAT_R32G32B32_UINT = 7;
const uint32_t DDS10_FORMAT_R32G32B32_SINT = 8;
const uint32_t DDS10_FORMAT_R16G16B16A16_TYPELESS = 9;
const uint32_t DDS10_FORMAT_R16G16B16A16_FLOAT = 10;
const uint32_t DDS10_FORMAT_R16G16B16A16_UNORM = 11;
const uint32_t DDS10_FORMAT_R16G16B16A16_UINT = 12;
const uint32_t DDS10_FORMAT_R16G16B16A16_SNORM = 13;
const uint32_t DDS10_FORMAT_R16G16B16A16_SINT = 14;
const uint32_t DDS10_FORMAT_R32G32_TYPELESS = 15;
const uint32_t DDS10_FORMAT_R32G32_FLOAT = 16;
const uint32_t DDS10_FORMAT_R32G32_UINT = 17;
const uint32_t DDS10_FORMAT_R32G32_SINT = 18;
const uint32_t DDS10_FORMAT_R32G8X24_TYPELESS = 19;
const uint32_t DDS10_FORMAT_D32_FLOAT_S8X24_UINT = 20;
const uint32_t DDS10_FORMAT_R32_FLOAT_X8X24_TYPELESS = 21;
const uint32_t DDS10_FORMAT_X32_TYPELESS_G8X24_UINT = 22;
const uint32_t DDS10_FORMAT_R10G10B10A2_TYPELESS = 23;
const uint32_t DDS10_FORMAT_R10G10B10A2_UNORM = 24;
const uint32_t DDS10_FORMAT_R10G10B10A2_UINT = 25;
const uint32_t DDS10_FORMAT_R11G11B10_FLOAT = 26;
const uint32_t DDS10_FORMAT_R8G8B8A8_TYPELESS = 27;
const uint32_t DDS10_FORMAT_R8G8B8A8_UNORM = 28;
const uint32_t DDS10_FORMAT_R8G8B8A8_UNORM_SRGB = 29;
const uint32_t DDS10_FORMAT_R8G8B8A8_UINT = 30;
const uint32_t DDS10_FORMAT_R8G8B8A8_SNORM = 31;
const uint32_t DDS10_FORMAT_R8G8B8A8_SINT = 32;
const uint32_t DDS10_FORMAT_R16G16_TYPELESS = 33;
const uint32_t DDS10_FORMAT_R16G16_FLOAT = 34;
const uint32_t DDS10_FORMAT_R16G16_UNORM = 35;
const uint32_t DDS10_FORMAT_R16G16_UINT = 36;
const uint32_t DDS10_FORMAT_R16G16_SNORM = 37;
const uint32_t DDS10_FORMAT_R16G16_SINT = 38;
const uint32_t DDS10_FORMAT_R32_TYPELESS = 39;
const uint32_t DDS10_FORMAT_D32_FLOAT = 40;
const uint32_t DDS10_FORMAT_R32_FLOAT = 41;
const uint32_t DDS10_FORMAT_R32_UINT = 42;
const uint32_t DDS10_FORMAT_R32_SINT = 43;
const uint32_t DDS10_FORMAT_R24G8_TYPELESS = 44;
const uint32_t DDS10_FORMAT_D24_UNORM_S8_UINT = 45;
const uint32_t DDS10_FORMAT_R24_UNORM_X8_TYPELESS = 46;
const uint32_t DDS10_FORMAT_X24_TYPELESS_G8_UINT = 47;
const uint32_t DDS10_FORMAT_R8G8_TYPELESS = 48;
const uint32_t DDS10_FORMAT_R8G8_UNORM = 49;
const uint32_t DDS10_FORMAT_R8G8_UINT = 50;
const uint32_t DDS10_FORMAT_R8G8_SNORM = 51;
const uint32_t DDS10_FORMAT_R8G8_SINT = 52;
const uint32_t DDS10_FORMAT_R16_TYPELESS = 53;
const uint32_t DDS10_FORMAT_R16_FLOAT = 54;
const uint32_t DDS10_FORMAT_D16_UNORM = 55;
const uint32_t DDS10_FORMAT_R16_UNORM = 56;
const uint32_t DDS10_FORMAT_R16_UINT = 57;
const uint32_t DDS10_FORMAT_R16_SNORM = 58;
const uint32_t DDS10_FORMAT_R16_SINT = 59;
const uint32_t DDS10_FORMAT_R8_TYPELESS = 60;
const uint32_t DDS10_FORMAT_R8_UNORM = 61;
const uint32_t DDS10_FORMAT_R8_UINT = 62;
const uint32_t DDS10_FORMAT_R8_SNORM = 63;
const uint32_t DDS10_FORMAT_R8_SINT = 64;
const uint32_t DDS10_FORMAT_A8_UNORM = 65;
const uint32_t DDS10_FORMAT_R1_UNORM = 66;
const uint32_t DDS10_FORMAT_R9G9B9E5_SHAREDEXP = 67;
const uint32_t DDS10_FORMAT_R8G8_B8G8_UNORM = 68;
const uint32_t DDS10_FORMAT_G8R8_G8B8_UNORM = 69;
const uint32_t DDS10_FORMAT_BC1_TYPELESS = 70;
const uint32_t DDS10_FORMAT_BC1_UNORM = 71;
const uint32_t DDS10_FORMAT_BC1_UNORM_SRGB = 72;
const uint32_t DDS10_FORMAT_BC2_TYPELESS = 73;
const uint32_t DDS10_FORMAT_BC2_UNORM = 74;
const uint32_t DDS10_FORMAT_BC2_UNORM_SRGB = 75;
const uint32_t DDS10_FORMAT_BC3_TYPELESS = 76;
const uint32_t DDS10_FORMAT_BC3_UNORM = 77;
const uint32_t DDS10_FORMAT_BC3_UNORM_SRGB = 78;
const uint32_t DDS10_FORMAT_BC4_TYPELESS = 79;
const uint32_t DDS10_FORMAT_BC4_UNORM = 80;
const uint32_t DDS10_FORMAT_BC4_SNORM = 81;
const uint32_t DDS10_FORMAT_BC5_TYPELESS = 82;
const uint32_t DDS10_FORMAT_BC5_UNORM = 83;
const uint32_t DDS10_FORMAT_BC5_SNORM = 84;
const uint32_t DDS10_FORMAT_B5G6R5_UNORM = 85;
const uint32_t DDS10_FORMAT_B5G5R5A1_UNORM = 86;
const uint32_t DDS10_FORMAT_B8G8R8A8_UNORM = 87;
const uint32_t DDS10_FORMAT_B8G8R8X8_UNORM = 88;
const uint32_t DDS10_FORMAT_R10G10B10_XR_BIAS_A2_UNORM = 89;
const uint32_t DDS10_FORMAT_B8G8R8A8_TYPELESS = 90;
const uint32_t DDS10_FORMAT_B8G8R8A8_UNORM_SRGB = 91;
const uint32_t DDS10_FORMAT_B8G8R8X8_TYPELESS = 92;
const uint32_t DDS10_FORMAT_B8G8R8X8_UNORM_SRGB = 93;
const uint32_t DDS10_FORMAT_BC6H_TYPELESS = 94;
const uint32_t DDS10_FORMAT_BC6H_UF16 = 95;
const uint32_t DDS10_FORMAT_BC6H_SF16 = 96;
const uint32_t DDS10_FORMAT_BC7_TYPELESS = 97;
const uint32_t DDS10_FORMAT_BC7_UNORM = 98;
const uint32_t DDS10_FORMAT_BC7_UNORM_SRGB = 99;
const uint32_t DDS10_FORMAT_FORCE_UINT = 0xffffffffUL;
// ASTC extension
const uint32_t DDS10_FORMAT_ASTC_4X4_UNORM = 134;
const uint32_t DDS10_FORMAT_ASTC_4X4_UNORM_SRGB = 135;
const uint32_t DDS10_FORMAT_ASTC_5X4_TYPELESS = 137;
const uint32_t DDS10_FORMAT_ASTC_5X4_UNORM = 138;
const uint32_t DDS10_FORMAT_ASTC_5X4_UNORM_SRGB = 139;
const uint32_t DDS10_FORMAT_ASTC_5X5_TYPELESS = 141;
const uint32_t DDS10_FORMAT_ASTC_5X5_UNORM = 142;
const uint32_t DDS10_FORMAT_ASTC_5X5_UNORM_SRGB = 143;
const uint32_t DDS10_FORMAT_ASTC_6X5_TYPELESS = 145;
const uint32_t DDS10_FORMAT_ASTC_6X5_UNORM = 146;
const uint32_t DDS10_FORMAT_ASTC_6X5_UNORM_SRGB = 147;
const uint32_t DDS10_FORMAT_ASTC_6X6_TYPELESS = 149;
const uint32_t DDS10_FORMAT_ASTC_6X6_UNORM = 150;
const uint32_t DDS10_FORMAT_ASTC_6X6_UNORM_SRGB = 151;
const uint32_t DDS10_FORMAT_ASTC_8X5_TYPELESS = 153;
const uint32_t DDS10_FORMAT_ASTC_8X5_UNORM = 154;
const uint32_t DDS10_FORMAT_ASTC_8X5_UNORM_SRGB = 155;
const uint32_t DDS10_FORMAT_ASTC_8X6_TYPELESS = 157;
const uint32_t DDS10_FORMAT_ASTC_8X6_UNORM = 158;
const uint32_t DDS10_FORMAT_ASTC_8X6_UNORM_SRGB = 159;
const uint32_t DDS10_FORMAT_ASTC_8X8_TYPELESS = 161;
const uint32_t DDS10_FORMAT_ASTC_8X8_UNORM = 162;
const uint32_t DDS10_FORMAT_ASTC_8X8_UNORM_SRGB = 163;
const uint32_t DDS10_FORMAT_ASTC_10X5_TYPELESS = 165;
const uint32_t DDS10_FORMAT_ASTC_10X5_UNORM = 166;
const uint32_t DDS10_FORMAT_ASTC_10X5_UNORM_SRGB = 167;
const uint32_t DDS10_FORMAT_ASTC_10X6_TYPELESS = 169;
const uint32_t DDS10_FORMAT_ASTC_10X6_UNORM = 170;
const uint32_t DDS10_FORMAT_ASTC_10X6_UNORM_SRGB = 171;
const uint32_t DDS10_FORMAT_ASTC_10X8_TYPELESS = 173;
const uint32_t DDS10_FORMAT_ASTC_10X8_UNORM = 174;
const uint32_t DDS10_FORMAT_ASTC_10X8_UNORM_SRGB = 175;
const uint32_t DDS10_FORMAT_ASTC_10X10_TYPELESS = 177;
const uint32_t DDS10_FORMAT_ASTC_10X10_UNORM = 178;
const uint32_t DDS10_FORMAT_ASTC_10X10_UNORM_SRGB = 179;
const uint32_t DDS10_FORMAT_ASTC_12X10_TYPELESS = 181;
const uint32_t DDS10_FORMAT_ASTC_12X10_UNORM = 182;
const uint32_t DDS10_FORMAT_ASTC_12X10_UNORM_SRGB = 183;
const uint32_t DDS10_FORMAT_ASTC_12X12_TYPELESS = 185;
const uint32_t DDS10_FORMAT_ASTC_12X12_UNORM = 186;
const uint32_t DDS10_FORMAT_ASTC_12X12_UNORM_SRGB = 187;



//DDS 10 resource dimension enums
const uint32_t DDS10_RESOURCE_DIMENSION_UNKNOWN = 0;
const uint32_t DDS10_RESOURCE_DIMENSION_BUFFER = 1;
const uint32_t DDS10_RESOURCE_DIMENSION_TEXTURE1D = 2;
const uint32_t DDS10_RESOURCE_DIMENSION_TEXTURE2D = 3;
const uint32_t DDS10_RESOURCE_DIMENSION_TEXTURE3D = 4;


struct DXTColBlock
{
	uint16_t col0;
	uint16_t col1;

	uint8_t row[4];
};

struct DXT3AlphaBlock
{
	uint16_t row[4];
};

struct DXT5AlphaBlock
{
	uint8_t alpha0;
	uint8_t alpha1;

	uint8_t row[6];
};

struct DDS_PIXELFORMAT
{
	uint32_t dwSize;
	uint32_t dwFlags;
	uint32_t dwFourCC;
	uint32_t dwRGBBitCount;
	uint32_t dwRBitMask;
	uint32_t dwGBitMask;
	uint32_t dwBBitMask;
	uint32_t dwABitMask;
};

struct DDS_HEADER
{
	uint32_t dwSize;
	uint32_t dwFlags;
	uint32_t dwHeight;
	uint32_t dwWidth;
	uint32_t dwPitchOrLinearSize;
	uint32_t dwDepth;
	uint32_t dwMipMapCount;
	uint32_t dwReserved1[11];
	DDS_PIXELFORMAT ddspf;
	uint32_t dwCaps1;
	uint32_t dwCaps2;
	uint32_t dwReserved2[3];
};

struct DDS_HEADER_10
{
	uint32_t dxgiFormat;  // check type
	uint32_t resourceDimension; //check type
	uint32_t miscFlag;
	uint32_t arraySize;
	uint32_t reserved;
};

//
//
////////////////////////////////////////////////////////////
static VkFormat TranslateNvFormat(const NvImage &i) {
	uint32_t fmt = i.getInternalFormat();

	switch (fmt) {
	case NVIMAGE_RGBA32F:
		return VK_FORMAT_R32G32B32A32_SFLOAT;

	case NVIMAGE_RGBA32UI:
		return VK_FORMAT_R32G32B32A32_UINT;

	case NVIMAGE_RGBA32I:
		return VK_FORMAT_R32G32B32A32_SINT;

	case NVIMAGE_RGB32F:
		return VK_FORMAT_R32G32B32_SFLOAT;

	case NVIMAGE_RGB32UI:
		return VK_FORMAT_R32G32B32_UINT;

	case NVIMAGE_RGB32I:
		return VK_FORMAT_R32G32B32_SINT;

	case NVIMAGE_RGBA16F:
		return VK_FORMAT_R16G16B16A16_SFLOAT;

	case NVIMAGE_RGBA16:
		return VK_FORMAT_R16G16B16A16_UNORM;

	case NVIMAGE_RGBA16UI:
		return VK_FORMAT_R16G16B16A16_UINT;

	case NVIMAGE_RGBA16_SNORM:
		return VK_FORMAT_R16G16B16A16_SNORM;

	case NVIMAGE_RGBA16I:
		return VK_FORMAT_R16G16B16A16_SINT;

	case NVIMAGE_RG32F:
		return VK_FORMAT_R32G32_SFLOAT;

	case NVIMAGE_RG32UI:
		return VK_FORMAT_R32G32B32_UINT;

	case NVIMAGE_RG32I:
		return VK_FORMAT_R32G32B32_SINT;

	case NVIMAGE_RGB10_A2:
		return VK_FORMAT_A2R10G10B10_UNORM_PACK32;

	case NVIMAGE_R11F_G11F_B10F:
		return VK_FORMAT_B10G11R11_UFLOAT_PACK32;

	case NVIMAGE_RGBA8:
		return VK_FORMAT_R8G8B8A8_UNORM;

	case NVIMAGE_SRGB8_ALPHA8:
		return VK_FORMAT_R8G8B8A8_SRGB;

	case NVIMAGE_RGBA8UI:
		return VK_FORMAT_R8G8B8A8_UINT;

	case NVIMAGE_RGBA8_SNORM:
		return VK_FORMAT_B8G8R8A8_SNORM;

	case NVIMAGE_RG16F:
		return VK_FORMAT_R16G16_SFLOAT;

	case NVIMAGE_RG16:
		return VK_FORMAT_R16G16_SINT;

	case NVIMAGE_RG16UI:
		return VK_FORMAT_R16G16_UINT;

	case NVIMAGE_RG16_SNORM:
		return VK_FORMAT_R16G16_SNORM;

	case NVIMAGE_DEPTH_COMPONENT32F:
		return VK_FORMAT_D32_SFLOAT;

	case NVIMAGE_R32F:
		return VK_FORMAT_R32_SFLOAT;

	case NVIMAGE_R32UI:
		return VK_FORMAT_R32_UINT;

	case NVIMAGE_R32I:
		return VK_FORMAT_R32_SINT;

	case NVIMAGE_RG8:
		return VK_FORMAT_R8G8_UNORM;

	case NVIMAGE_RG8UI:
		return VK_FORMAT_R8G8_UINT;

	case NVIMAGE_RG8_SNORM:
		return VK_FORMAT_R8G8_SNORM;

	case NVIMAGE_RG8I:
		return VK_FORMAT_R8G8_SINT;

	case NVIMAGE_R16F:
		return VK_FORMAT_R16_SFLOAT;

	case NVIMAGE_DEPTH_COMPONENT16:
		return VK_FORMAT_D16_UNORM;

	case NVIMAGE_R16:
		return VK_FORMAT_R16_UNORM;

	case NVIMAGE_R16UI:
		return VK_FORMAT_R16_UINT;

	case NVIMAGE_R16_SNORM:
		return VK_FORMAT_R16_SNORM;

	case NVIMAGE_R16I:
		return VK_FORMAT_R16_SINT;

	case NVIMAGE_R8:
		return VK_FORMAT_R8_UNORM;

	case NVIMAGE_R8UI:
		return VK_FORMAT_R8_UINT;

	case NVIMAGE_R8_SNORM:
		return VK_FORMAT_R8_SNORM;

	case NVIMAGE_R8I:
		return VK_FORMAT_R8_SINT;

	case NVIMAGE_ALPHA8:
		return VK_FORMAT_R8_UNORM;

	case NVIMAGE_RGB9_E5:
		return VK_FORMAT_E5B9G9R9_UFLOAT_PACK32;

	case NVIMAGE_COMPRESSED_RGBA_S3TC_DXT1:
		return VK_FORMAT_BC1_RGBA_UNORM_BLOCK;

	case NVIMAGE_COMPRESSED_RGBA_S3TC_DXT3:
		return VK_FORMAT_BC3_UNORM_BLOCK;

	case NVIMAGE_COMPRESSED_RGBA_S3TC_DXT5:
		return VK_FORMAT_BC5_UNORM_BLOCK;

	case NVIMAGE_COMPRESSED_RED_RGTC1:
	case NVIMAGE_COMPRESSED_SIGNED_RED_RGTC1:
	case NVIMAGE_COMPRESSED_RG_RGTC2:
	case NVIMAGE_COMPRESSED_SIGNED_RG_RGTC2:

	case NVIMAGE_COMPRESSED_RGBA_ASTC_4x4:
	case NVIMAGE_COMPRESSED_RGBA_ASTC_5x4:
	case NVIMAGE_COMPRESSED_RGBA_ASTC_5x5:
	case NVIMAGE_COMPRESSED_RGBA_ASTC_6x5:
	case NVIMAGE_COMPRESSED_RGBA_ASTC_6x6:
	case NVIMAGE_COMPRESSED_RGBA_ASTC_8x5:
	case NVIMAGE_COMPRESSED_RGBA_ASTC_8x6:
	case NVIMAGE_COMPRESSED_RGBA_ASTC_8x8:
	case NVIMAGE_COMPRESSED_RGBA_ASTC_10x5:
	case NVIMAGE_COMPRESSED_RGBA_ASTC_10x6:
	case NVIMAGE_COMPRESSED_RGBA_ASTC_10x8:
	case NVIMAGE_COMPRESSED_RGBA_ASTC_10x10:
	case NVIMAGE_COMPRESSED_RGBA_ASTC_12x10:
	case NVIMAGE_COMPRESSED_RGBA_ASTC_12x12:

	case NVIMAGE_COMPRESSED_SRGB8_ALPHA8_ASTC_4x4:
	case NVIMAGE_COMPRESSED_SRGB8_ALPHA8_ASTC_5x4:
	case NVIMAGE_COMPRESSED_SRGB8_ALPHA8_ASTC_5x5:
	case NVIMAGE_COMPRESSED_SRGB8_ALPHA8_ASTC_6x5:
	case NVIMAGE_COMPRESSED_SRGB8_ALPHA8_ASTC_6x6:
	case NVIMAGE_COMPRESSED_SRGB8_ALPHA8_ASTC_8x5:
	case NVIMAGE_COMPRESSED_SRGB8_ALPHA8_ASTC_8x6:
	case NVIMAGE_COMPRESSED_SRGB8_ALPHA8_ASTC_8x8:
	case NVIMAGE_COMPRESSED_SRGB8_ALPHA8_ASTC_10x5:
	case NVIMAGE_COMPRESSED_SRGB8_ALPHA8_ASTC_10x6:
	case NVIMAGE_COMPRESSED_SRGB8_ALPHA8_ASTC_10x8:
	case NVIMAGE_COMPRESSED_SRGB8_ALPHA8_ASTC_10x10:
	case NVIMAGE_COMPRESSED_SRGB8_ALPHA8_ASTC_12x10:
	case NVIMAGE_COMPRESSED_SRGB8_ALPHA8_ASTC_12x12:
		return VK_FORMAT_UNDEFINED;

	default:
		//these are errors
		return VK_FORMAT_UNDEFINED;
	};

	return VK_FORMAT_UNDEFINED;
}
