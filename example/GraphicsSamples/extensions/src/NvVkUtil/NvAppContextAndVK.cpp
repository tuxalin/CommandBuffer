//----------------------------------------------------------------------------------
// File:        NvVkUtil/NvAppContextAndVK.cpp
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
#ifdef ANDROID
#include "NvAppContextAndVK.h"
#include "NvImage/NvImage.h"
#include "NvVkUtil/NvVkUtil.h"
#include "NV/NvLogs.h"
#include "NvAndVkWinUtil.h"
#include "NvVkRenderTargetImpls.h"

#define ARRAY_SIZE(a) ( sizeof(a) / sizeof( (a)[0] ))

NvAppContextAndVK::NvAppContextAndVK(NvVKConfiguration config, NvAndVkWinUtil* win, NvPlatformInfo info) :
NvAppNativeContextVK(config, "Android VK App", info)
, mWin(win)
{
}

NvAppContextAndVK::~NvAppContextAndVK()
{
}

/*
static void* vulkan_so = NULL;
static void* dlVkProc(const char* name) {
	if (!vulkan_so)
		vulkan_so = dlopen("libvulkan.so", RTLD_NOW);

	return 	vulkan_so ? dlsym(vulkan_so, name) : NULL;
}
*/

bool NvAppContextAndVK::initialize() {
	if (!NvAppNativeContextVK::initialize())
		return false;

	return mWin->initialize(this, m_cbFormat, m_dsFormat);
}

bool NvAppContextAndVK::bindContext() {
	return true;// mGLContext->bindContext();
}

bool NvAppContextAndVK::unbindContext() {
	return true;// mGLContext->unbindContext();
}

bool NvAppContextAndVK::setSwapInterval(int32_t interval) {
	return true;// mGLContext->setSwapInterval(interval);
}

int32_t NvAppContextAndVK::width() {
	return mWin ? (mWin->getRenderTarget() ? mWin->getRenderTarget()->width() : 0) : 0;
}

int32_t NvAppContextAndVK::height() {
	return mWin ? (mWin->getRenderTarget() ? mWin->getRenderTarget()->height() : 0) : 0;
}

NvVkRenderTarget* NvAppContextAndVK::mainRenderTarget() {
	return mWin ? mWin->getRenderTarget() : NULL;
}

bool NvAppContextAndVK::initRenderTarget() {
	return true;
}


#endif
