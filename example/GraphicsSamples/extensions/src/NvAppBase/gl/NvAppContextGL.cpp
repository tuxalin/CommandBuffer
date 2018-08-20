//----------------------------------------------------------------------------------
// File:        NvAppBase\gl/NvAppContextGL.cpp
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
#include "NvAppBase/gl/NvAppContextGL.h"
#include "NvGLUtils/NvSimpleFBO.h"
#include "NvGLUtils/NvImageGL.h"

#include "NV/NvLogs.h"
NvAppContextGL::NvAppContextGL(NvPlatformInfo info) :
NvAppContext(info)
, m_windowWidth(0)
, m_windowHeight(0)
, mMainFBO(0)
, mUseFBOPair(false)
, mCurrentFBOIndex(0)
, m_fboWidth(0)
, m_fboHeight(0)
{
    mFBOPair[0] = NULL;
    mFBOPair[1] = NULL;
}

NvAppContextGL::~NvAppContextGL()
{
    delete mFBOPair[0];
    delete mFBOPair[1];
}

bool NvAppContextGL::useOffscreenRendering(int32_t w, int32_t h) {
    // clear the main framebuffer to black for later testing
    glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
    glClear(GL_COLOR_BUFFER_BIT);

    mUseFBOPair = false;
    endFrame();
    mUseFBOPair = true;

    m_fboWidth = w;
    m_fboHeight = h;

    NvSimpleFBO::Desc desc;
    desc.width = m_fboWidth;
    desc.height = m_fboHeight;
    desc.color.format = GL_RGBA;
    desc.color.filter = GL_LINEAR;
    desc.color.type = GL_UNSIGNED_BYTE;
    desc.color.wrap = GL_CLAMP_TO_EDGE;
    if (getConfiguration().apiVer.api == NvGLAPI::GL ||
        getConfiguration().apiVer.majVersion >= 3 ||
        isExtensionSupported("GL_OES_packed_depth_stencil"))
    {
        desc.depthstencil.format = 0x88F0; // GL_DEPTH24_STENCIL8_EXT
    }
    else
    {
        desc.depth.format = GL_DEPTH_COMPONENT;
        desc.depth.type = GL_UNSIGNED_INT;
        desc.depth.filter = GL_NEAREST;
    }

    mFBOPair[0] = new NvSimpleFBO(desc);
    mFBOPair[1] = new NvSimpleFBO(desc);

    endFrame();

    return true;
}

bool NvAppContextGL::isRenderingToMainScreen() {
    if (mUseFBOPair) {
        // Check if the app bound FBO 0 in FBO mode
        GLuint currFBO = 0;
        // Enum has MANY names based on extension/version
        // but they all map to 0x8CA6
        glGetIntegerv(0x8CA6, (GLint*)&currFBO);

        return (currFBO == 0);
    }
    else {
        return true;
    }
}

void NvAppContextGL::platformReshape(int32_t& w, int32_t& h) {
    m_windowWidth = w;
    m_windowHeight = h;

    if (mUseFBOPair) {
        w = m_fboWidth;
        h = m_fboHeight;
    }
}

void NvAppContextGL::beginFrame() {

}

void NvAppContextGL::beginScene() {

}

void NvAppContextGL::endScene() {

}

bool NvAppContextGL::swapFBO() {
    mCurrentFBOIndex = mCurrentFBOIndex ? 0 : 1;
    mMainFBO = mFBOPair[mCurrentFBOIndex]->fbo;
    glBindFramebuffer(GL_FRAMEBUFFER, mMainFBO);
    return true;
}

void NvAppContextGL::endFrame() {
    if (mUseFBOPair) {
        swapFBO();
    }
    else {
        swap();
    }
}

void NvAppContextGL::contextInitRendering() {
    NvImageGL::SupportsFormatConversion(getConfiguration().apiVer.api != NvGLAPI::GLES);
    NvImage::setSupportsBGR(getConfiguration().apiVer.api != NvGLAPI::GLES);
}

void NvAppContextGL::initUI() {
    extern void NvUIUseGL();
    NvUIUseGL();
}

bool NvAppContextGL::readFramebufferRGBX32(uint8_t *dest, int32_t& w, int32_t& h) {
    // This above TEST_MODE_FBO_ISSUE only checks the flag from the end of each frame;
    // it only detects if the app left FBO 0 bound at the end of the frame.  We could
    // still miss a mid-frame binding of FBO 0.  The best way to test for that is to
    // read back FBO 0 at the end of the app and test if any pixel is non-zero:
    if (mUseFBOPair && dest) {
        glBindFramebuffer(GL_FRAMEBUFFER, 0);

        const int32_t size = 4 * m_windowWidth * m_windowHeight;
        const uint8_t* onscreenData = new uint8_t[size];

        glReadPixels(0, 0, m_windowWidth, m_windowHeight,
            GL_RGBA, GL_UNSIGNED_BYTE, (GLvoid*)onscreenData);

        const uint8_t* ptr = onscreenData;
        for (int i = 0; i < size; i++) {
            if (*(ptr++)) {
                return false;
            }
        }

        delete[] onscreenData;

        glBindFramebuffer(GL_FRAMEBUFFER, getMainFBO());
    }

    w = mUseFBOPair ? m_fboWidth : m_windowWidth;
    h = mUseFBOPair ? m_fboHeight : m_windowHeight;

    if (dest) {
        glReadPixels(0, 0, w, h, GL_RGBA, GL_UNSIGNED_BYTE, (GLvoid*)dest);
    }

    return true;
}
