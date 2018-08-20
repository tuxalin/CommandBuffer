//----------------------------------------------------------------------------------
// File:        NvAppBase\gl/NvSampleAppGL.h
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
#ifndef NV_SAMPLE_APP_GL_H
#define NV_SAMPLE_APP_GL_H

#include <NvSimpleTypes.h>

#include "NvAppBase/NvSampleApp.h"
#include "NvAppBase/gl/NvAppContextGL.h"

/// \file
/// GL-based Sample app base class.

/// Base class for GL sample apps.
class NvSampleAppGL : public NvSampleApp
{
public:
    /// Constructor
    /// Do NOT make rendering API calls in the constructor
    /// The rendering context is not bound until the entry into initRendering
    NvSampleAppGL();

    /// Destructor
    virtual ~NvSampleAppGL();

    /// Extension requirement declaration.
    /// Allow an app to declare an extension as "required".
    /// \param[in] ext the extension name to be required
    /// \param[in] exitOnFailure if true,
    /// then #errorExit is called to indicate the issue and exit
    /// \return true if the extension string is exported and false
    /// if it is not
    bool requireExtension(const char* ext, bool exitOnFailure = true);

    /// GL Minimum API requirement declaration.
    /// \param[in] minApi the minimum API that is required
    /// \param[in] exitOnFailure if true,
    /// then errorExit is called to indicate the issue and exit
    /// \return true if the platform's GL[ES] API version is at least
    /// as new as the given minApi.  Otherwise, returns false
    bool requireMinAPIVersion(const NvGLAPIVersion& minApi, bool exitOnFailure = true);

    /// GL configuration request callback.
    /// This function passes in the default set of GL configuration requests
    /// The app can override this function and change the defaults before
    /// returning.  These are still requests, and may not be met.  If the
    /// platform supports requesting GL options, this function will be called
    /// before initGL.  Optional.
    /// \param[in,out] config the default config to be used is passed in.  If the application
    /// wishes anything different in the GL configuration, it should change those values before
    /// returning from the function.  These are merely requests.
    virtual void configurationCallback(NvGLConfiguration& config) { }

	/// Return the GL-specific app context structure for easy use in the app
	/// \return Pointer to the GL-specific app context or NULL if not initialized
    NvAppContextGL* getGLContext() { return (NvAppContextGL*)mContext; }

    /// Retrieve the main "onscreen" framebuffer; this may actually
    /// be an offscreen FBO that the framework uses internally, and then
    /// resolves to the window.  Apps should ALWAYS use this value when
    /// binding the onscreen FBO and NOT use FBO ID 0 in order to ensure
    /// that they are compatible with test mode, etc.
    /// This should be queried on a per-frame basis.  It may change every frame
    /// \return the GL ID of the main, "onscreen" FBO
    GLuint getMainFBO() const { return ((NvAppContextGL*)mContext)->getMainFBO(); }

	/// \privatesection
	virtual int32_t getUniqueTypeID();
	static bool isType(NvAppBase* app);

	virtual bool initialize(const NvPlatformInfo& platform, int32_t width, int32_t height);

protected:
    /// \privatesection
    virtual bool platformInitRendering(void);

    virtual void platformInitUI(void);

    virtual void platformLogTestResults(float frameRate, int32_t frames);

private:
};

#endif
