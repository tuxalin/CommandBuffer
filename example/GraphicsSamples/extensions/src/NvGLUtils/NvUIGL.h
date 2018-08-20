//----------------------------------------------------------------------------------
// File:        NvGLUtils/NvUIGL.h
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
#ifndef NV_UI_GL_H
#define NV_UI_GL_H

#include <NvUI/NvUI.h>
#include "NvImage/NvImage.h"
#include "NV/NvPlatformGL.h"
#include <NvGLUtils/NvGLSLProgram.h>
#include "NV/NvLogs.h"
#include "NvUIShadersGL.h"
#include "NvUITextureRenderGL.h"




typedef struct
{
    float x, y;     // position
    float s, t;     // texture coordinate
    float bx, by;   // whether or not this vertex is a border vertex in x and y
} NvFrameVertex;

class NvUITextureRenderGL;

class NvUIGraphicRenderGL : public NvUIGraphicRender {
public:
    static NvUIGraphicRender* Create(NvUIGraphic* graphic);

    NvUIGraphicRenderGL(NvUIGraphic* graphic);
    virtual ~NvUIGraphicRenderGL();
    virtual void Draw(float alpha, const NvPackedColor& color, const float pixelToClipMatrix[4][4]);

    NvUITextureRenderGL* GetTex() { return (NvUITextureRenderGL*)(m_graphic->GetTex()->GetRender()); }
    bool StaticInit();
    bool StaticCleanup();

    NvUIGraphic* m_graphic;

    static NvGraphicShaderGL ms_shader;

    static GLuint ms_vbo;
    static GLuint ms_ibo;
    static int32_t ms_initCount;
};


class NvUIGraphicFrameRenderGL : public NvUIGraphicFrameRender {
public:
    static NvUIGraphicFrameRender* Create(NvUIGraphicFrame* graphic);

    NvUIGraphicFrameRenderGL(NvUIGraphicFrame* graphic);
    virtual ~NvUIGraphicFrameRenderGL();
    virtual void Draw(float alpha, const NvPackedColor& color, const float pixelToClipMatrix[4][4],
        const nv::vec2<float>& thickness, const nv::vec2<float>& texBorder, bool drawCenter);

    bool StaticInit();
    bool StaticCleanup();

    NvUIGraphicFrame* m_graphic;

    static int32_t ms_staticCount;

    static NvGraphicFrameShaderGL ms_shader;

    static GLuint ms_gfvbo;
    static GLuint ms_gfibo;
};


#endif
