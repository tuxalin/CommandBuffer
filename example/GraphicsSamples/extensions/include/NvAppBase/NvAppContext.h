//----------------------------------------------------------------------------------
// File:        NvAppBase/NvAppContext.h
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

#ifndef NV_APPCONTEXT_H
#define NV_APPCONTEXT_H

#include <NvSimpleTypes.h>

/// \file
/// Cross-platform rendering Context APIs and information

/// GPU platform category (high-level).
struct NvPlatformCategory {
    enum Enum {
        PLAT_MOBILE, ///< Mobile/handheld platform
        PLAT_DESKTOP ///< Desktop/laptop-class platform
    };
};

/// Platform OS.
struct NvPlatformOS {
    enum Enum {
        OS_ANDROID, ///< Android
        OS_WINDOWS, ///< Windows
        OS_LINUX, ///< "Desktop" Linux
        OS_MACOSX ///< MacOS
    };
};

/// Basic, combined platform info.
struct NvPlatformInfo {
    /// Inline, all-members constructor
    /// \param[in] category the platform category
    /// \param[in] os the platform OS
    NvPlatformInfo(NvPlatformCategory::Enum category, NvPlatformOS::Enum os) :
        mCategory(category)
        , mOS(os)
    { }

    NvPlatformCategory::Enum mCategory; ///< Platform GPU category
    NvPlatformOS::Enum mOS; ///< Platform OS
};


class NvAppContext {
public:
    virtual ~NvAppContext() {}

    /// Prepare a seconary thread for use with rendering.  Does not bind the context.
    /// Any thread other than the thread used to create the app context must
    /// make this call before binding a context.
    /// \return true on success, false on failure
    virtual bool prepThreadForRender() { return false; }

    /// Bind the rendering context (and current surface) to the current thread, creating if needed.
    /// \return true on success, false on failure
    virtual bool bindContext() = 0;

    /// Unbind the rendering context from the current thread
    /// \return true on success, false on failure
    virtual bool unbindContext() = 0;

    /// Set the swap interval if supported by the platform.
    /// \param[in] interval the number of VSYNCs to wait between swaps (0 == never wait)
    /// \return true if the platform can support swap interval, false if not
    virtual bool setSwapInterval(int32_t interval) = 0;

    /// Surface width.
    /// \return the surface width in pixels
    virtual int32_t width() = 0;

    /// Surface height.
    /// \return the surface height in pixels
    virtual int32_t height() = 0;

	/// Read back the current framebuffer into app-supplied RGBX (32-bit-per-pixel) memory
	/// \param[in,out] dest The destination memory.  Must be NULL (in which case on the width and height
	/// are returned) or else must point to width*height*4 bytes of memory.  In the latter case, on success,
	/// the screenshot will have been written to the buffer
	/// \param[out] w the returned width of the image
	/// \param[out] h the returned height of the image
	/// \return true on success and false if the implementation does not support screenshots
	virtual bool readFramebufferRGBX32(uint8_t *dest, int32_t& w, int32_t& h) { return false; }

    /// The platform description.
    /// \return the platform information for the system
    const NvPlatformInfo& getPlatformInfo() const { return mPlatformInfo; }

	/// Swap the rendering buffers (ie present).
	/// \return true on success, false on failure
	virtual bool swap() = 0;

	/// \privatesection
	virtual void contextInitRendering() {}

	virtual bool useOffscreenRendering(int32_t w, int32_t h) { return false; }

	virtual bool isRenderingToMainScreen() { return true; }

	virtual void platformReshape(int32_t& w, int32_t& h) { /* */ }

	virtual void beginFrame() { }

	virtual void beginScene() { }

	virtual void endScene() { }

	virtual void endFrame() { swap(); }

	virtual void initUI() { }

	virtual bool initialize() = 0;

protected:
    /// \privatesection
    NvAppContext(NvPlatformInfo info) :
        mPlatformInfo(info) {}
    NvPlatformInfo mPlatformInfo;
};

#endif
