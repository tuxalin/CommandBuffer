//----------------------------------------------------------------------------------
// File:        NvVkUtil/NvAppContextVK.h
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
#ifndef NV_APPCONTEXTVK_H
#define NV_APPCONTEXTVK_H

#include <NvSimpleTypes.h>
#include "NvVkContext.h"
#include <NvAppBase/NvAppContext.h>

///  Vulkan app Context
class NvAppContextVK : public NvAppContext, public NvVkContext {
public:
	virtual ~NvAppContextVK() { }

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

	/// Enable/disable API usage validation via the LunarG validation layers
	void useValidation(bool val) { m_use_validation = val; }
	bool useValidation() const { return m_use_validation; }

	/// Enable debugging of the Vulkan loader via the LunarG vSDK
	void useLoaderDebug(bool val) { m_use_loader_debug = val; }
	bool useLoaderDebug() const { return m_use_loader_debug; }

	/// Enable API use dump via the LunarG vSDK
	void useApiDump(bool val) { m_use_api_dump = val; }
	bool useApiDump() const { return m_use_api_dump; }

	/// \privatesection
	virtual void endFrame() { NvAppContext::endFrame(); nextFrame(); }

protected:

	bool initializeInstance(const std::string& appTitle);
	bool initializeDevice();

    /// \privatesection
	NvAppContextVK(const NvVKConfiguration& config, NvPlatformInfo info) 
        : NvAppContext(info)
		, NvVkContext(config)
        , m_use_validation(false)
        , m_use_loader_debug(false)
        , m_use_api_dump(false)
		, m_cbFormat(VK_FORMAT_UNDEFINED)
		, m_dsFormat(VK_FORMAT_UNDEFINED)

     { }

	VkFormat m_cbFormat;
	VkFormat m_dsFormat;

private:

    bool m_use_validation;
    bool m_use_loader_debug;
    bool m_use_api_dump;

};

#endif
