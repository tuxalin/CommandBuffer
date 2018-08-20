//----------------------------------------------------------------------------------
// File:        NvAppBase\gl/NvAppContextGL.h
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

#ifndef NV_GLAPPCONTEXT_H
#define NV_GLAPPCONTEXT_H

#include <NvAppBase/NvAppContext.h>
#include <NvSimpleTypes.h>
#include <NV/NvPlatformGL.h>
#include <NV/NvGfxConfiguration.h>

class NvSimpleFBO;

/// GL config representation.
struct NvGLConfiguration : public NvGfxConfiguration {
    NvGLConfiguration(const NvGfxConfiguration& gfx,
        const NvGLAPIVersion& _apiVer = NvGLAPIVersionES2()) :
        NvGfxConfiguration(gfx), apiVer(_apiVer) {}

    /// Inline all-elements constructor.
    /// \param[in] _apiVer the API and version information
    /// \param[in] r the red color depth in bits
    /// \param[in] g the green color depth in bits
    /// \param[in] b the blue color depth in bits
    /// \param[in] a the alpha color depth in bits
    /// \param[in] d the depth buffer depth in bits
    /// \param[in] s the stencil buffer depth in bits
    /// \param[in] msaa the MSAA buffer sample count
    NvGLConfiguration(const NvGLAPIVersion& _apiVer = NvGLAPIVersionES2(),
        uint32_t r = 8, uint32_t g = 8,
        uint32_t b = 8, uint32_t a = 8,
        uint32_t d = 24, uint32_t s = 0, uint32_t msaa = 0) :
        NvGfxConfiguration(r, g, b, a, d, s, msaa),
        apiVer(_apiVer) {}

    NvGLAPIVersion apiVer; ///< API and version
};

/// OpenGL[ES] Context wrapper.
class NvAppContextGL : public NvAppContext, public NvGLExtensionsAPI {
public:
    virtual ~NvAppContextGL();

    /// Read back the current framebuffer into app-supplied RGBX (32-bit-per-pixel) memory
    /// \param[in,out] dest The destination memory.  Must be NULL (in which case on the width and height
    /// are returned) or else must point to width*height*4 bytes of memory.  In the latter case, on success,
    /// the screenshot will have been written to the buffer
    /// \param[out] w the returned width of the image
    /// \param[out] h the returned height of the image
    /// \return true on success and false if the implementation does not support screenshots
    virtual bool readFramebufferRGBX32(uint8_t *dest, int32_t& w, int32_t& h);

    /// Return the FBO ID of the main screen render target
    /// \return the GL ID of the main screen FBO
    GLuint getMainFBO() { return mMainFBO; }

    /// The selected [E]GL configuration.
    /// \return the selected configuration information for the platform
    const NvGLConfiguration& getConfiguration() const { return mConfig; }

    /// Cross-platform extension function retrieval.
    /// \return the named extension function if available.  Note that on
    /// some platforms, non-NULL return does NOT indicate support for the
    /// extension.  The only safe way to know if an extension is supported is
    /// via the extension string.
    virtual GLproc getGLProcAddress(const char* procname) { return NULL; }

    /// Extension support query.
    /// \return true if the given string is found in the extension set for the
    /// context, false if not.  Should only be called with a bound context for
    /// safety across all platforms
    virtual bool isExtensionSupported(const char* ext) { return false; }

    /// Force context reset.
    /// Optional per-platform function to request that the GL context be
    /// shut down and restarted on demand.  Used to test an app's implementation
    /// of the initRendering/shutdownRendering sequence
    /// \return true if the feature is supported and the context has been reset,
    /// false if not supported or could not be completed
    virtual bool requestResetContext() { return false; }

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

    //// End of frame implementation for FBO mode
    virtual bool swapFBO();

    /// \privatesection
    virtual bool useOffscreenRendering(int32_t w, int32_t h);

    virtual bool isRenderingToMainScreen();

    virtual void platformReshape(int32_t& w, int32_t& h);

    virtual void beginFrame();

    virtual void beginScene();

    virtual void endScene();

    virtual void endFrame();

    virtual void initUI();

    /// Initialize the rendering context
    virtual void contextInitRendering();
    virtual bool initialize() { return true; }

protected:

    /// \privatesection
    NvAppContextGL(NvPlatformInfo info);

    NvGLConfiguration mConfig;

    int32_t m_windowWidth;
    int32_t m_windowHeight;

    GLuint mMainFBO;
    bool mUseFBOPair;
    int32_t mCurrentFBOIndex;
    NvSimpleFBO* mFBOPair[2];
    int32_t m_fboWidth;
    int32_t m_fboHeight;
};

#endif
