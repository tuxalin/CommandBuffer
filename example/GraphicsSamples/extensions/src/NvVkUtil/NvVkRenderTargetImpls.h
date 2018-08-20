//----------------------------------------------------------------------------------
// File:        NvVkUtil/NvVkRenderTargetImpls.h
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
#ifndef NV_VK_RENDER_TARGET_IMPLS_H
#define NV_VK_RENDER_TARGET_IMPLS_H 1
#include <NvSimpleTypes.h>
#include "NvPlatformVK.h"
#include "NvVkContext.h"
#include <vector>
#include <queue>

class NvVkMultibufferedRenderTarget : public NvVkRenderTarget {
public:
	~NvVkMultibufferedRenderTarget() { }

	static NvVkMultibufferedRenderTarget* create(NvVkContext& vk, const VkFormat& targetFormat, const VkFormat& depthStencilFormat, bool useSwapChain,
		int32_t w = 0, int32_t h = 0);

	virtual bool resize(int32_t& w, int32_t& h);

	virtual void setSurface(VkSurfaceKHR surface);

    VkSurfaceKHR getSurface() { return _surface; }

	virtual bool beginFrame();
	virtual bool endFrame();

	virtual NvVkImage& target() { return _frames[_currentBuffer._index]._target; }
	virtual VkImage& image() { return _frames[_currentBuffer._index]._target(); }
	virtual VkImage& depthStencil() { return _depthStencil(); }
	virtual VkImageView& targetView() { return _frames[_currentBuffer._index]._targetView; }
	virtual VkFramebuffer& frameBuffer() { return _frames[_currentBuffer._index]._frameBuffer; }

	virtual VkSemaphore* getWaitSemaphore() { return &_currentBuffer._acquireSemaphore; }
	virtual VkSemaphore* getSignalSemaphore() { return &_currentBuffer._renderSemaphore; }

protected:
	NvVkMultibufferedRenderTarget(NvVkContext& vk) :
		NvVkRenderTarget(vk),
		_depthStencil(),
		_dsView(VK_NULL_HANDLE)
	{}

	struct Frame {
		Frame()
			: _targetView(VK_NULL_HANDLE)
			, _frameBuffer(VK_NULL_HANDLE)
		{}

		NvVkImage _target;
		VkImageView _targetView;
		VkFramebuffer _frameBuffer;
	};

	struct BackBuffer {
		BackBuffer() 
			: _index(0)
			, _acquireSemaphore(VK_NULL_HANDLE)
			, _renderSemaphore(VK_NULL_HANDLE)
			, _presentFence(VK_NULL_HANDLE)
		{}

		uint32_t _index;
		VkSemaphore _acquireSemaphore;
		VkSemaphore _renderSemaphore;
		VkFence _presentFence;
	};

	std::vector<Frame> _frames;
	std::queue<BackBuffer> _backBuffers;
	std::queue<VkDeviceMemory> _memory;

	BackBuffer _currentBuffer;

	NvVkImage _depthStencil;
	VkImageView _dsView;

	VkSwapchainKHR _swapchain;
	VkSurfaceKHR _surface;

	bool _useSwapChain;
};

#endif
