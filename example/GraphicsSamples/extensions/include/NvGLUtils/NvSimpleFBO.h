//----------------------------------------------------------------------------------
// File:        NvGLUtils/NvSimpleFBO.h
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

#ifndef NV_SIMPLE_FBO_H
#define NV_SIMPLE_FBO_H

#include <NvSimpleTypes.h>


#include "NV/NvPlatformGL.h"
#include "NV/NvLogs.h"
#include <stdlib.h>

/// \file
/// OpenGL Framebuffer Object wrappers

/// Object representing an FBO or the main framebuffer.
/// An object of this type can represent the back-buffer or FBO 0.  The back-buffer is not strictly an FBO
/// but this allows some abstraction between the back-buffer and FBOs.
class NvWritableFB
{
public:

    /// Base constructor
    /// Creates an object that (by default) represents the main framebuffer (FBO 0)
    /// \param[in] w the width in pixels
    /// \param[in] h the height in pixels
    NvWritableFB(GLuint w, GLuint h): fbo(0), width(w), height(h) {}

    /// Destructor
    /// Deletes any associated (nonzero) FBO
     ~NvWritableFB()
     {
         if (fbo)
         {
             glDeleteFramebuffers(1, &fbo);
            fbo = 0;
         }
     }


    /// Binds the calling framebuffer and sets the viewport based
    /// on the sizes cached by the object
     void bind()
     {
        glBindFramebuffer(GL_FRAMEBUFFER, fbo);
        glViewport(0, 0, width, height);
     }

    GLuint         fbo; ///< The GL framebuffer object handle
    GLuint       width; ///< the width in pixels
    GLuint       height; ///< the height in pixels
};

/// Texture-based framebuffers class.
/// Class encapsulating a color plus (optional) depth framebuffer
/// Both renderbuffers in the framebuffer are also bound to textures
class NvSimpleFBO : public NvWritableFB
{
public:
    /// Texture format information class.
    /// Used for creating the textures upon which the FBOs are based
    struct TextureDesc
    {
        TextureDesc()
        : format(0)
        , type(0)
        , wrap(GL_CLAMP_TO_EDGE)
        , filter(GL_NEAREST)
        {
        }
        GLenum format; ///< GL format value (e.g. GL_RGBA), also used for internal format
        GLenum type; ///< GL type value (e.g. GL_UNSIGNED_BYTE)
        GLint  wrap; ///< GL texture coordinate mapping (e.g. GL_CLAMP_TO_EDGE)
        GLint  filter; ///< GL texture filtering method, applied to min and mag
    };

    /// Framebuffer description class.
    /// Used to represent the color and depth textures as well as the size
    struct Desc
    {
        Desc()
        : width(0)
        , height(0)
        {
        }
        GLuint width; ///< width in pixels
        GLuint height; ///< height in pixels
        TextureDesc color; ///< color texture descriptor (format should be 0 if no color texture desired)
        TextureDesc depth; ///< depth texture descriptor (format should be 0 if no depth texture desired)
		TextureDesc depthstencil;
    };

    /// Create FBO from descriptor.
     NvSimpleFBO(const NvSimpleFBO::Desc& desc)
    : NvWritableFB(desc.width, desc.height)
    , colorTexture(0)
    , depthTexture(0)
    {
        GLuint prevFBO = 0;
        // Enum has MANY names based on extension/version
        // but they all map to 0x8CA6
        glGetIntegerv(0x8CA6, (GLint*)&prevFBO);

        glGenFramebuffers(1, &fbo);

        if (desc.color.format)
        {
            colorTexture = createColorTexture2D(desc);
            CHECK_GL_ERROR();

            glBindFramebuffer(GL_FRAMEBUFFER, fbo);
            glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, colorTexture, 0);
        }

		if (desc.depthstencil.format)
		{
			GLuint stencilRB;
			glGenRenderbuffers(1, &stencilRB);

			glBindRenderbuffer(GL_RENDERBUFFER, stencilRB);
			glRenderbufferStorage(GL_RENDERBUFFER,
				desc.depthstencil.format,
				desc.width, desc.height);

			glBindFramebuffer(GL_FRAMEBUFFER, fbo);
			glFramebufferRenderbuffer(GL_FRAMEBUFFER,
				GL_DEPTH_ATTACHMENT,
				GL_RENDERBUFFER, stencilRB);
			glFramebufferRenderbuffer(GL_FRAMEBUFFER,
				GL_STENCIL_ATTACHMENT,
				GL_RENDERBUFFER, stencilRB);
		}
        else if (desc.depth.format)
        {
            depthTexture = createDepthTexture2D(desc);
            CHECK_GL_ERROR();

            glBindFramebuffer(GL_FRAMEBUFFER, fbo);
            glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, depthTexture, 0);
        }

        checkStatus();

        width = desc.width;
        height = desc.height;

        // restore FBO
        glBindFramebuffer(GL_FRAMEBUFFER, prevFBO);
    }

     ~NvSimpleFBO()
     {
         if (colorTexture)
         {
             glDeleteTextures(1, &colorTexture);
            colorTexture = 0;
         }
        if (depthTexture)
        {
            glDeleteTextures(1, &depthTexture);
            depthTexture = 0;
        }
         if (fbo)
         {
             glDeleteFramebuffers(1, &fbo);
            fbo = 0;
         }
     }

     void checkStatus()
     {
        GLuint status = glCheckFramebufferStatus(GL_FRAMEBUFFER);
        switch (status)
        {
            case GL_FRAMEBUFFER_COMPLETE:
                break;
            case GL_FRAMEBUFFER_INCOMPLETE_ATTACHMENT:
                LOGE("Can't create FBO: GL_FRAMEBUFFER_INCOMPLETE_ATTACHMENT error");
                break;
            case GL_FRAMEBUFFER_INCOMPLETE_MISSING_ATTACHMENT:
                LOGE("Can't create FBO: GL_FRAMEBUFFER_INCOMPLETE_MISSING_ATTACHMENT error");
                break;
#if 0
            case GL_FRAMEBUFFER_INCOMPLETE_DIMENSIONS:
                LOGE("Can't create FBO: GL_FRAMEBUFFER_INCOMPLETE_DIMENSIONS error");
                break;
#endif
            case GL_FRAMEBUFFER_UNSUPPORTED:
                LOGE("Can't create FBO: GL_FRAMEBUFFER_UNSUPPORTED error");
                break;
            default:
                LOGE("Can't create FBO: unknown error");
                break;
        }
     }

    GLuint         colorTexture; ///< The color texture ID
    GLuint         depthTexture; ///< the depth texture ID

protected:
    GLuint createColorTexture2D(const NvSimpleFBO::Desc& desc)
    {
        GLuint texture;
        glGenTextures(1, &texture);
        glBindTexture(GL_TEXTURE_2D, texture);
        glTexImage2D(GL_TEXTURE_2D, 0, desc.color.format, desc.width, desc.height, 0, desc.color.format, desc.color.type, NULL);
        CHECK_GL_ERROR();

        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, desc.color.wrap);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, desc.color.wrap);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, desc.color.filter);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, desc.color.filter);

        return texture;
    }

    GLuint createDepthTexture2D(const NvSimpleFBO::Desc& desc)
    {
        GLuint texture;
        glGenTextures(1, &texture);
        glBindTexture(GL_TEXTURE_2D, texture);
        glTexImage2D(GL_TEXTURE_2D, 0, desc.depth.format, desc.width, desc.height, 0, desc.depth.format, desc.depth.type, NULL);
        CHECK_GL_ERROR();

        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, desc.depth.wrap);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, desc.depth.wrap);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, desc.depth.filter);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, desc.depth.filter);

        return texture;
    }
};

#endif /* NV_SIMPLE_FBO_H */
