//----------------------------------------------------------------------------------
// File:        NvUI/NvUIGraphic.cpp
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
/*
 * library for implementing base classes for NV UI framework
 * used for building Widgets and Applications on common code.
 */

#include "NvUIInternal.h"

#include "NvAssetLoader/NvAssetLoader.h"
#include "NvImage/NvImage.h"
#include "NV/NvLogs.h"

#include <math.h>

static float s_pixelToClipMatrix[4][4];

//======================================================================
// ----- NvUIGraphic -----
//======================================================================

//======================================================================
//======================================================================
void NvUIGraphic::PrivateInit(void)
{
    m_tex = NULL;
    m_scale = false;
    m_color = NV_PC_PREDEF_WHITE;

    // The following entries are const
    // so we set them up now and never change
    s_pixelToClipMatrix[2][0] = 0.0f;
    s_pixelToClipMatrix[2][1] = 0.0f;

    // Bitfont obliterates Z right now
    s_pixelToClipMatrix[0][2] = 0.0f;
    s_pixelToClipMatrix[1][2] = 0.0f;
    s_pixelToClipMatrix[2][2] = 0.0f;
    s_pixelToClipMatrix[3][2] = 0.0f;

    s_pixelToClipMatrix[0][3] = 0.0f;
    s_pixelToClipMatrix[1][3] = 0.0f;
    s_pixelToClipMatrix[2][3] = 0.0f;
    s_pixelToClipMatrix[3][3] = 1.0f;
};

//======================================================================
//======================================================================
NvUIGraphic::NvUIGraphic(const std::string& texname, float dstw/*==0*/, float dsth/*==0*/)
{
    PrivateInit();
	LoadTexture(texname);
	m_render = NvUIRenderFactory::GraphicCreate(this);
    if (dstw!=0)
        SetDimensions(dstw, dsth);
}

//======================================================================
//======================================================================
NvUIGraphic::NvUIGraphic(NvUITexture *uiTex, float dstw/*==0*/, float dsth/*==0*/)
{
    PrivateInit();
    m_render = NvUIRenderFactory::GraphicCreate(this);
    m_tex = uiTex;
    m_tex->AddRef();
    if (dstw!=0)
        SetDimensions(dstw, dsth);
    else
        SetDimensions((float)m_tex->GetWidth(), (float)m_tex->GetHeight());
}


//======================================================================
//======================================================================
NvUIGraphic::~NvUIGraphic()
{
    FlushTexture();
    delete m_render;
}


//======================================================================
//======================================================================
void NvUIGraphic::FlushTexture()
{
    if (m_tex)
    {
        m_tex->DelRef();
        m_tex = NULL;
    }
    m_scale = false;
}


//======================================================================
//======================================================================
bool NvUIGraphic::LoadTexture(const std::string& texname, bool resetDimensions/*==true*/)
{
    FlushTexture(); // in case we're being use to re-load new texture.

    m_tex = NvUITexture::CacheTexture(texname);

    if (m_tex && resetDimensions)
    {
        // set screenrect and dest dimensions to match the raw texel size of the texture.
        SetDimensions((float)m_tex->GetWidth(), (float)m_tex->GetHeight());
    }

    return(m_tex && m_tex->IsTexValid());
}


void NvUIGraphic::SetTexture(NvUITexture *tex)
{
    FlushTexture();
    m_tex = tex;
    m_tex->AddRef();

    // TODO - Does this really make sense??  We rarely use texel-to-pixel
    // graphics, do we?  We tend to have set the scale manually.
    // And since SetTextureID doesn't rescale, I think this should not, either
//    SetDimensions((float)m_tex->GetWidth(), (float)m_tex->GetHeight());
}



//======================================================================
// set the color to white to disable colorization
//======================================================================
void NvUIGraphic::SetColor(NvPackedColor color)
{
    m_color = color;
}


//======================================================================
//======================================================================
void NvUIGraphic::Draw(const NvUIDrawState &drawState)
{
    if (!m_isVisible) return;
    if (!m_tex) return;

    // calculate internal alpha value...
    float myAlpha = m_alpha;
    if (drawState.alpha != 1.0f)
        myAlpha *= drawState.alpha;

    // update the transform matrix.
    int32_t designWidth, designHeight;
    if (drawState.designWidth)
    {
        designWidth = drawState.designWidth;
        designHeight = drawState.designHeight;
    }
    else
    {
        designWidth = drawState.width;
        designHeight = drawState.height;
    }
    
    float rad = (float)(drawState.rotation / 180.0f * 3.14159f); // [-1,2]=>[-90,180] in radians...
    float cosf = cos(rad);
    float sinf = sin(rad);

    const float wNorm = 2.0f / (int32_t)designWidth;
    const float hNorm = 2.0f / (int32_t)designHeight;

    {
        s_pixelToClipMatrix[0][0] = wNorm * m_rect.width  * cosf;
        s_pixelToClipMatrix[1][0] = hNorm * m_rect.height * -sinf;
        s_pixelToClipMatrix[0][1] = wNorm * m_rect.width  * sinf;
        s_pixelToClipMatrix[1][1] = hNorm * m_rect.height * cosf;
    }
    
    s_pixelToClipMatrix[3][0] = ( wNorm * m_rect.left - 1) * cosf
                              - ( 1 - hNorm * (m_rect.top + m_rect.height))  * sinf;
    s_pixelToClipMatrix[3][1] = ( wNorm * m_rect.left - 1 ) * sinf
                              + ( 1 - hNorm * (m_rect.top + m_rect.height))  * cosf;

    m_render->Draw(myAlpha, m_color, s_pixelToClipMatrix);
}
