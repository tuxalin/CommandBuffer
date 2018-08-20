//----------------------------------------------------------------------------------
// File:        NvVkUtil/NvAppContextAndVK.h
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

#ifndef NV_VKAPPCONTEXTANDVK_H
#define NV_VKAPPCONTEXTANDVK_H

#include "NvAppNativeContextVK.h"
#include <NvSimpleTypes.h>

class NvAndVkWinUtil;

///  Context wrapper (VK on top of Android).
class NvAppContextAndVK : public NvAppNativeContextVK {
public:
	NvAppContextAndVK(NvVKConfiguration config, NvAndVkWinUtil* win, NvPlatformInfo info);

	virtual bool initialize();

	virtual ~NvAppContextAndVK();

	virtual bool bindContext();

	virtual bool unbindContext();

	virtual bool setSwapInterval(int32_t interval);

	virtual int32_t width();

	virtual int32_t height();
	
    /// Get platform-specific context.
    /// This function is for use in special circumstances where the WGL, EGL, GLX, etc
    /// context is required by the application.  Most applications should avoid this function.
    /// \return the platform-specific context handle, cast to void* or NULL if not supported
    virtual void* getCurrentPlatformContext() { return NULL; }

    /// Get platform-specific display.
    /// This function is for use in special circumstances where the WGL, EGL, GLX, etc
    /// display is required by the application.  Most applications should avoid this function.
    /// \return the platform-specific display handle, cast to void* or NULL if not supported
    virtual void* getCurrentPlatformDisplay() { return NULL; }

	virtual NvVkRenderTarget* mainRenderTarget();

	virtual VkSemaphore* getWaitSync() { return mainRenderTarget()->getWaitSemaphore(); }
	virtual VkSemaphore* getSignalSync() { return mainRenderTarget()->getSignalSemaphore(); }

protected:
	virtual bool initRenderTarget();

	NvAndVkWinUtil* mWin;
	std::string mAppTitle;
};

#endif
