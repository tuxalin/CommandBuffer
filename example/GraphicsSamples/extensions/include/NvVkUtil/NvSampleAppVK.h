//----------------------------------------------------------------------------------
// File:        NvVkUtil/NvSampleAppVK.h
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

// simple SDK app interface
#ifndef NV_SAMPLE_APP_VK_H
#define NV_SAMPLE_APP_VK_H

#include <NvSimpleTypes.h>

#include "NvAppBase/NvSampleApp.h"
#include "NvAppContextVK.h"

/// \file
/// VK-based Sample app base class.

class NvGLDrawCallbacks;
class NvAppWrapperContextVK;
class NvAppContextGL;

/// Callbacks for rendering GL in a VK app (only supported on NV HW)
class NvGLDrawCallbacks {
public:
	virtual ~NvGLDrawCallbacks() { }
	/// Indicates whether the app wants to turn off VK rendering (per-frame)
	/// \return should return true if VK rendering (::draw) should be skipped, false if both GL and VK rendering are needed
	virtual bool skipVKDraw() { return false; }
	/// Called for the app to initialize its GL resources
	/// \param[in] gl the GL app context to use for loading rendering data
	virtual void initGLResources(NvAppContextGL* gl) = 0;
	/// Callback to draw the GL rendering
	/// \param[in] gl the GL app context to use to render
	virtual void drawGL(NvAppContextGL* gl) = 0;
};


/// Base class for VK sample apps.
class NvSampleAppVK : public NvSampleApp
{
public:
    /// Constructor
    /// Do NOT make rendering API calls in the constructor
    /// The rendering context is not bound until the entry into initRendering
    /// \param[in] platform the platform context representing the system, normally
    /// passed in from the #NvAppFactory
    /// \param[in] appTitle the null-terminated string title of the application
	NvSampleAppVK();

    /// Destructor
	virtual ~NvSampleAppVK();

	/// Return the VK helper context
	/// \return the NvAppContextVK object
	NvAppContextVK& vk() { return *((NvAppContextVK*)mContext); }

	/// Sets GL-in-VK-app callbacks (if supported). Must be called in the constructor, may still not be supported if true is returned
	/// \param[in] cb the callback object
	/// \return true if supported false if not
	bool setGLDrawCallbacks(NvGLDrawCallbacks *cb);
	
	/// Returns whether the HW supports GL in a VK app.  Not valid until initRendering
	/// \return true if GL callbacks are supported, false if VK-only
	bool isGLSupported() { return mGLSupported; }

	/// Direct device access
	/// \return the VkDevice
	VkDevice device() { return vk().device();  }
	/// Direct queue access
	/// \return the main rendering VkQueue
	VkQueue queue() { return vk().queue(); }

	/// Extension support query.
	/// \return true if the given string is found in the device and instance extension set for the
	/// context, false if not.
	bool isExtensionSupported(const char* ext) { return vk().isExtensionSupported(ext); }

	/// Layer support query.
	/// \return true if the given string is found in the device and instance layer set for the
	/// context, false if not.
	bool isLayerSupported(const char* ext) { return vk().isLayerSupported(ext); }


	/// \privatesection
	virtual int32_t getUniqueTypeID();
	static bool isType(NvAppBase* app);

	virtual bool initialize(const NvPlatformInfo& platform, int32_t width, int32_t height);
	virtual void configurationCallback(NvVKConfiguration& config) {}

protected:
    /// \privatesection
    virtual bool platformInitRendering(void);

    virtual void platformInitUI(void);

    virtual void platformLogTestResults(float frameRate, int32_t frames);

private:
	bool mUseWSI;
	bool mUseGLUI;
	bool mGLSupported;
	NvGLDrawCallbacks* mGLCallbacks;
	NvAppWrapperContextVK* mWrapperContext;
};

#endif
