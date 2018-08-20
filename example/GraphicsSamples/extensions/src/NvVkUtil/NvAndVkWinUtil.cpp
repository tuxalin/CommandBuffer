//----------------------------------------------------------------------------------
// File:        NvVkUtil/NvAndVkWinUtil.cpp
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
#if defined(ANDROID)

#define VK_USE_PLATFORM_ANDROID_KHR 1
#include "NvAndVkWinUtil.h"
#include "NvVkRenderTargetImpls.h"

typedef VkResult(VKAPI_PTR *PFN_vkCreateAndroidSurfaceKHR)(VkInstance instance, const VkAndroidSurfaceCreateInfoKHR* pCreateInfo, const VkAllocationCallbacks* pAllocator, VkSurfaceKHR* pSurface);
typedef void (VKAPI_PTR *PFN_vkDestroySurfaceKHR)(VkInstance instance, VkSurfaceKHR surface, const VkAllocationCallbacks* pAllocator);

static PFN_vkCreateAndroidSurfaceKHR pfn_vkCreateAndroidSurfaceKHR = NULL;
static PFN_vkDestroySurfaceKHR pfn_vkDestroySurfaceKHR = NULL;

bool NvAndVkWinUtil::initialize(NvVkContext* vk, VkFormat cbFormat, VkFormat dsFormat) {
	m_cbFormat = cbFormat;
	m_dsFormat = dsFormat;
	m_vk = vk;
	return true;
}

// Shuts down any active EGL resources and deletes
NvAndVkWinUtil::~NvAndVkWinUtil() {

}

// used to set or update the native window given to us by Android
// Must be called with NULL when the window is destroyed
bool NvAndVkWinUtil::setWindow(ANativeWindow* window) {
	m_window = window;
    if (!m_window && m_renderTarget) {
        extern void* NvAndroidGetVKProcAddr(const char* name);
        pfn_vkDestroySurfaceKHR = (PFN_vkDestroySurfaceKHR)NvAndroidGetVKProcAddr("vkDestroySurfaceKHR");

        pfn_vkDestroySurfaceKHR(m_vk->instance(), m_renderTarget->getSurface(), NULL);
        delete m_renderTarget;
        m_renderTarget = NULL;
    }
	return true;
}

// Returns true if the window size has changed since the last call to this
// function, and indicates that the app needs to re-set its viewport, etc
bool NvAndVkWinUtil::checkWindowResized() {
	return true;
}

// Binds the rendering context and on-screen surface; 
// fails if there is no rendering surface available
bool NvAndVkWinUtil::bind() {
	return true;
}

// Unbinds the current rendering context and surface
bool NvAndVkWinUtil::unbind() {
	return true;
}

// Preps a new thread for use with EGL/GL[ES]
bool NvAndVkWinUtil::prepareThread() {
	return true;
}

// Swaps the current on-screen surface.  Fails if the surface and context
// are not already bounds
bool NvAndVkWinUtil::swap() {
	return true;
}

bool NvAndVkWinUtil::requestResetContext() {
	return true;
}

// Returns true if there is a bound surface and context.
// If allocateIfNeeded is true, then the function will 
// attempt to allocate the rendering surface and bind the
// surface and context if possible.  Otherwise, the app
// takes no actions and only returns whether or not the
// system was already ready for rendering
bool NvAndVkWinUtil::isReadyToRender(bool allocateIfNeeded) {
	if (m_renderTarget)
		return true;

	if (allocateIfNeeded) {
		if (!m_window || m_cbFormat == VK_FORMAT_UNDEFINED || m_dsFormat == VK_FORMAT_UNDEFINED) {
			return false;
		}
		if (!m_renderTarget) {
			m_renderTarget = NvVkMultibufferedRenderTarget::create(*m_vk, m_cbFormat, m_dsFormat, true);

			if (m_renderTarget) {
				VkSurfaceKHR surface;
				VkAndroidSurfaceCreateInfoKHR info = { VK_STRUCTURE_TYPE_ANDROID_SURFACE_CREATE_INFO_KHR };
				info.window = m_window;

				extern void* NvAndroidGetVKProcAddr(const char* name);
				pfn_vkCreateAndroidSurfaceKHR = (PFN_vkCreateAndroidSurfaceKHR)NvAndroidGetVKProcAddr("vkCreateAndroidSurfaceKHR");

				VkResult result = pfn_vkCreateAndroidSurfaceKHR(m_vk->instance(), &info, NULL, &surface);

				m_renderTarget->setSurface(surface);
				return true;
			}
			else {
				return false;
			}

		}
		return true;
	}

	return false;
}

#endif
