//----------------------------------------------------------------------------------
// File:        NvGLUtils/NvUITextureRenderGL.h
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

#ifndef _NV_UI_TEXTURE_RENDER_GL_H
#define _NV_UI_TEXTURE_RENDER_GL_H

#include <NvSimpleTypes.h>

#include <NvUI/NvUI.h>
#include <NV/NvPlatformGL.h>
#include "NvGLUtils/NvImageGL.h"
#include "NvUIGL.h"

class NvUITextureRenderGL : public NvUITextureRender {
public:
    static NvUITextureRender* Create(NvImage* image);

    NvUITextureRenderGL(NvImage *image) {
        glActiveTexture(GL_TEXTURE0);
        m_glID = NvImageGL::UploadTexture(image);
        //TestPrintGLError("Error 0x%x after texture upload...\n");

        // set up any tweaks to texture state here...
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, m_glID);
        glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
        glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
        // then turn the bind back off!
        glBindTexture(GL_TEXTURE_2D, 0);
    }

    virtual ~NvUITextureRenderGL() { 
        if (m_glID)
        { // if this is bound, we could be in bad place... !!!TBD
            glDeleteTextures(1, &m_glID);
        }
        m_glID = 0;
    }
    virtual bool IsTexValid() { return m_glID != 0;  }

    GLuint m_glID;
};

#endif
