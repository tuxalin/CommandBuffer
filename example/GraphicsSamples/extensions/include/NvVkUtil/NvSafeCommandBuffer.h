//----------------------------------------------------------------------------------
// File:        NvVkUtil/NvSafeCommandBuffer.h
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
#ifndef NV_SAFE_COMMAND_BUFFER
#define NV_SAFE_COMMAND_BUFFER

#include <NvSimpleTypes.h>
#include <NvVkUtil/NvVkContext.h>
#include <NvVkUtil/NvSimpleSyncRing.h>
#include <NvAppBase/NvThread.h>

template <uint32_t m_copies = 4, uint32_t m_buffers = 1>
class NvSafeCommandBuffer {
public:
	NvSafeCommandBuffer(void) {}
	virtual ~NvSafeCommandBuffer(void) {}

	bool Initialize(NvVkContext* vk, NvMutex* mutex, bool primary = true) {
		m_vk = vk;

		if (!m_localSync.Initialize(*vk))
			return false;

		VkResult result;

		for (uint32_t i = 0; i < m_buffers; i++) {
			{
				VkCommandPoolCreateInfo cmdPoolInfo = { VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO };
                cmdPoolInfo.queueFamilyIndex = vk->queueFamilyIndex();
				cmdPoolInfo.flags = VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT | VK_COMMAND_POOL_CREATE_TRANSIENT_BIT;
				result = vkCreateCommandPool(vk->device(), &cmdPoolInfo, NULL, &m_pools[i]);
				if (result != VK_SUCCESS)
					return false;
			}

			// Create the command buffer.
			VkCommandBufferAllocateInfo cmdInfo = { VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO };
			cmdInfo.commandPool = m_pools[i];
			if (!cmdInfo.commandPool)
				return 0;

			cmdInfo.level = primary ? VK_COMMAND_BUFFER_LEVEL_PRIMARY :
				VK_COMMAND_BUFFER_LEVEL_SECONDARY;
			cmdInfo.commandBufferCount = m_copies;
			result = vkAllocateCommandBuffers(vk->device(), &cmdInfo, &(m_cbs[i][0]));
			if (result != VK_SUCCESS)
				return 0;
		}

		return true;
	}

	bool Reset() {
		if (!m_localSync.BeginUpdate())
			return false;

		for (uint32_t index = 0; index < m_copies; index++) {
			for (uint32_t j = 0; j < m_buffers; j++) {
//				m_cbs[index][j].Reset(*m_vk);
			}
		}

		return true;
	}

	void Finalize(void) {

		// Finalize NvSyncRing; we own it
		m_localSync.Finalize();

		//TODO finalize command buffers
	}

	bool BeginFrame() {
		if (!m_localSync.BeginUpdate())
			return false;

		// safe to reset
		uint32_t index = m_localSync.getIndex();

		return true;
	}

	bool DoneWithFrame() {
//		return m_localSync.doneWithIndex();
		return true;
	}
	/// Get the CommandBuffer for recording purposes
	/// \return a pointer to the command buffer
	operator VkCommandBuffer&(void)
	{
		return *(m_cbs[0][m_localSync.getIndex()]);
	}

	VkCommandBuffer& operator[] (const int item)
	{
		return (m_cbs[item][m_localSync.getIndex()]);
	}

	uint32_t bufferCount() { return m_buffers; }

protected:
	VkCommandPool m_pools[m_buffers];
	VkCommandBuffer m_cbs[m_buffers][m_copies];
	NvSyncRing<m_copies> m_localSync;
	NvVkContext* m_vk;
};

#endif // NV_SAFE_COMMAND_BUFFER
