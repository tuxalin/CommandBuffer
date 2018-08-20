//----------------------------------------------------------------------------------
// File:        NvVkUtil/NvAndVkWinUtil.h
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
#ifndef NV_AND_VK_WIN_UTIL_H
#define NV_AND_VK_WIN_UTIL_H

#include <NvSimpleTypes.h>
#include <android/native_window.h>
#include <NvVkUtil/NvPlatformVK.h>
#include "../NvAppBase/android/NvAndroidWinUtil.h"

class NvVkContext;
class NvVkMultibufferedRenderTarget;

class NvAndVkWinUtil : public NvAndroidWinUtil {
public:
	// returns a wrapper that is not initialized
	// no surface is created yet
	// A context is created but is not bound
	static NvAndVkWinUtil* create() { return new NvAndVkWinUtil; }

	bool initialize(NvVkContext* vk, VkFormat cbFormat, VkFormat dsFormat);

	// Shuts down any active EGL resources and deletes
	~NvAndVkWinUtil();

	// valid only when getWindow()!=NULL
	int32_t getWidth() { return m_width; }
	int32_t getHeight() { return m_height; }

	// used to set or update the native window given to us by Android
	// Must be called with NULL when the window is destroyed
	bool setWindow(ANativeWindow* window);

	// Returns true if the app has an allocated surface (bound or not)
	// returns false otherwise
	bool hasSurface() { return m_status >= NV_HAS_SURFACE; }

	// Returns true if the window size has changed since the last call to this
	// function, and indicates that the app needs to re-set its viewport, etc
	bool checkWindowResized();

	// Returns true if the rendering surface and context are bound
	// false otherwise
	bool isBound() { return m_status == NV_HAS_SURFACE; }

	// Binds the rendering context and on-screen surface; 
	// fails if there is no rendering surface available
	bool bind();

	// Unbinds the current rendering context and surface
	bool unbind();

	// Preps a new thread for use with EGL/GL[ES]
	bool prepareThread();

	// Swaps the current on-screen surface.  Fails if the surface and context
	// are not already bounds
	bool swap();

	bool requestResetContext();

	// Returns true if there is a bound surface and context.
	// If allocateIfNeeded is true, then the function will 
	// attempt to allocate the rendering surface and bind the
	// surface and context if possible.  Otherwise, the app
	// takes no actions and only returns whether or not the
	// system was already ready for rendering
	bool isReadyToRender(bool allocateIfNeeded = false);

	bool hasContext() { return true; }

	NvVkMultibufferedRenderTarget* getRenderTarget() { return m_renderTarget; }

protected:
	enum {
		// Numbering matters here, as we use >/< for state hierarchy
		NV_UNINITIALIZED = 0,
		NV_INITIALIZED = 1,
		NV_HAS_SURFACE = 2
	};

	NvAndVkWinUtil()
	{
		ms_defaultWindow = this;
		m_window = NULL;
		m_renderTarget = NULL;
		m_status = NV_UNINITIALIZED;
		m_width = 0;
		m_height = 0;
		m_newWindow = true;
	}

	NvVkMultibufferedRenderTarget* m_renderTarget;
	NvVkContext* m_vk;
	VkFormat m_cbFormat;
	VkFormat m_dsFormat;
	int32_t m_width;
	int32_t m_height;

	ANativeWindow* m_window;
	bool m_newWindow;

	uint32_t m_status;
};

#endif
