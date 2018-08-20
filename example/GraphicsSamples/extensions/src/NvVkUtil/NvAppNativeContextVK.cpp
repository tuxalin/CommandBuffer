//----------------------------------------------------------------------------------
// File:        NvVkUtil/NvAppNativeContextVK.cpp
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
#include "NvAppNativeContextVK.h"
#include "NvImage/NvImage.h"
#include "NvVkUtil/NvVkUtil.h"
#include "NV/NvLogs.h"
#include "NvVkRenderTargetImpls.h"

#ifndef ANDROID
#include <GL/glew.h>
#endif

#include <vulkan/vulkan.h>

#define ARRAY_SIZE(a) ( sizeof(a) / sizeof( (a)[0] ))


NvAppNativeContextVK::NvAppNativeContextVK(NvVKConfiguration config, const std::string& appTitle, NvPlatformInfo info) :
NvAppContextVK(config, info)
, mCurrentWidth(0)
, mCurrentHeight(0)
, mAppTitle(appTitle)
{
}

NvAppNativeContextVK::~NvAppNativeContextVK()
{
}

static VkInstance getVkProcInstance = NULL;

#ifdef ANDROID
#include <dlfcn.h>
static void* vulkan_so = NULL;
static PFN_vkGetInstanceProcAddr android_getVkProc = NULL;
static bool AndroidInitGetProcAddr() {
	vulkan_so = dlopen("libvulkan.so", RTLD_NOW | RTLD_LOCAL);
	if (!vulkan_so)
		return false;

	android_getVkProc = (PFN_vkGetInstanceProcAddr)dlsym(vulkan_so, "vkGetInstanceProcAddr");
	return android_getVkProc ? true : false;
}

void* NvAndroidGetVKProcAddr(const char* name) {
	return (void*)android_getVkProc(getVkProcInstance, name);
}
#endif

bool NvAppNativeContextVK::initialize() {

	if (m_cbFormat != VK_FORMAT_UNDEFINED)
		return true;

	VkResult result = VK_ERROR_INITIALIZATION_FAILED;

	NvImage::setSupportsBGR(false);

#ifdef ANDROID
	AndroidInitGetProcAddr();
#endif

	if (!initializeInstance(mAppTitle))
		return false;

	getVkProcInstance = _instance;

	if (!initializeDevice())
		return false;

	// Query for a valid render target format.
	static VkFormat const cbFormats[] = {
		VK_FORMAT_B8G8R8A8_UNORM,
	};

    // Query for a valid depth stencil format.
    static VkFormat const dsFormats[] = {
        VK_FORMAT_D24_UNORM_S8_UINT,
        VK_FORMAT_D32_SFLOAT_S8_UINT,
        VK_FORMAT_D16_UNORM_S8_UINT,
    };


	m_cbFormat = pickOptimalFormat(ARRAY_SIZE(cbFormats), cbFormats, VK_FORMAT_FEATURE_COLOR_ATTACHMENT_BIT);
    m_dsFormat = pickOptimalFormat(ARRAY_SIZE(dsFormats), dsFormats, VK_FORMAT_FEATURE_DEPTH_STENCIL_ATTACHMENT_BIT);

	if (m_cbFormat == VK_FORMAT_UNDEFINED) {
		// Could not find a format we want to render with.
		CHECK_VK_RESULT();
	}

    if (m_dsFormat == VK_FORMAT_UNDEFINED) {
        // Could not find a format we want to render with.
        CHECK_VK_RESULT();
    }

	if (!initRenderTarget())
		return false;

	return true;
}

void NvAppNativeContextVK::contextInitRendering() {
}


bool NvAppNativeContextVK::bindContext() {
	return true;
}

bool NvAppNativeContextVK::unbindContext() {
	return true;
}

bool NvAppNativeContextVK::setSwapInterval(int32_t interval) {
	return false;
}

int32_t NvAppNativeContextVK::width() {
	return mCurrentWidth;
}

int32_t NvAppNativeContextVK::height() {
	return mCurrentHeight;
}

bool NvAppNativeContextVK::useOffscreenRendering(int32_t w, int32_t h) {
	return false;
}

bool NvAppNativeContextVK::isRenderingToMainScreen() {
	return true;
}

void NvAppNativeContextVK::platformReshape(int32_t& w, int32_t& h) {
	VkResult result = VK_SUCCESS;

	if ((mCurrentWidth == w) && (mCurrentHeight == h))
		return;

	if (!reshape(w, h))
		return;

	mCurrentWidth = w;
	mCurrentHeight = h;
}

void NvAppNativeContextVK::beginFrame() {
	if (mainRenderTarget())
		mainRenderTarget()->beginFrame();
}

void NvAppNativeContextVK::beginScene() {
	
}

void NvAppNativeContextVK::endScene() {
	VkResult result = VK_ERROR_INITIALIZATION_FAILED;

	
}

void NvAppNativeContextVK::endFrame() {
	NvAppContextVK::endFrame();

	if (mainRenderTarget())
		mainRenderTarget()->endFrame();
}

bool NvAppNativeContextVK::swap(void) {
	return true;
}

void NvAppNativeContextVK::initUI() {
	// TODO TBD for now, VK uses the GL UI back-end, but this should really be set in the AppContext, not here...
	extern void NvUIUseVK(NvVkContext* context);
	NvUIUseVK(this);
}

bool NvAppNativeContextVK::readFramebufferRGBX32(uint8_t *dest, int32_t& w, int32_t& h) {
	return false;
}
