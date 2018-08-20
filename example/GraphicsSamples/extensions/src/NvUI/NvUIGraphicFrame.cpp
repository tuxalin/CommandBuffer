//----------------------------------------------------------------------------------
// File:        NvUI/NvUIGraphicFrame.cpp
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

static float s_gfpixelToClipMatrix[4][4];

//======================================================================
//======================================================================
static void StaticInit()
{
    // The following entries are const
    // so we set them up now and never change
    s_gfpixelToClipMatrix[2][0] = 0.0f;
    s_gfpixelToClipMatrix[2][1] = 0.0f;

    s_gfpixelToClipMatrix[0][2] = 0.0f;
    s_gfpixelToClipMatrix[1][2] = 0.0f;
    s_gfpixelToClipMatrix[2][2] = 1.0f;
    s_gfpixelToClipMatrix[3][2] = 0.0f;

    s_gfpixelToClipMatrix[0][3] = 0.0f;
    s_gfpixelToClipMatrix[1][3] = 0.0f;
    s_gfpixelToClipMatrix[2][3] = 0.0f;
    s_gfpixelToClipMatrix[3][3] = 1.0f;
}

//======================================================================
//======================================================================
NvUIGraphicFrame::NvUIGraphicFrame(const std::string& texname, float border)
    : NvUIGraphic(texname), m_drawCenter(true)
{
    m_texBorder.x = border;
    m_texBorder.y = border;
    m_borderThickness.x = border;
    m_borderThickness.y = border;
    m_render = NvUIRenderFactory::GraphicFrameCreate(this);
    ::StaticInit();
}

//======================================================================
//======================================================================
NvUIGraphicFrame::NvUIGraphicFrame(const std::string& texname, float borderX, float borderY)
    : NvUIGraphic(texname), m_drawCenter(true)
{
    m_texBorder.x = borderX;
    m_texBorder.y = borderY;
    m_borderThickness.x = borderX;
    m_borderThickness.y = borderY;
    m_render = NvUIRenderFactory::GraphicFrameCreate(this);
    ::StaticInit();
}


//======================================================================
//======================================================================
NvUIGraphicFrame::NvUIGraphicFrame(NvUITexture *uiTex, float border)
    : NvUIGraphic(uiTex), m_drawCenter(true)
{
    m_texBorder.x = border;
    m_texBorder.y = border;
    m_borderThickness.x = border;
    m_borderThickness.y = border;
    m_render = NvUIRenderFactory::GraphicFrameCreate(this);
    ::StaticInit();
}


//======================================================================
//======================================================================
NvUIGraphicFrame::~NvUIGraphicFrame()
{
    delete m_render;
}

//======================================================================
//======================================================================
bool NvUIGraphicFrame::LoadTexture(const std::string& texname, bool resetDimensions/*==true*/)
{
    // We pass false to inherited call to be explicit to leave our dimensions alone, and
    // not change them to match the texture's size, as frames inherently 'stretch' at draw
    // time to match the destination size.
    if (INHERITED::LoadTexture(texname, false))
        return true;    
    return false;
}


//======================================================================
//======================================================================
void NvUIGraphicFrame::SetBorderThickness(float thickness)
{
    m_borderThickness.x = thickness;
    m_borderThickness.y = thickness;
}

//======================================================================
//======================================================================
void NvUIGraphicFrame::SetBorderThickness(float width, float height)
{
    m_borderThickness.x = width;
    m_borderThickness.y = height;
}


//======================================================================
//======================================================================
void NvUIGraphicFrame::GetBorderThickness(float *x, float *y)
{
    if (x)
        *x = m_borderThickness.x;
    if (y)
        *y = m_borderThickness.y;
}


//======================================================================
//======================================================================
void NvUIGraphicFrame::SetDrawCenter(bool drawCenter)
{
    m_drawCenter = drawCenter;
}

//======================================================================
//======================================================================
void NvUIGraphicFrame::Draw(const NvUIDrawState &drawState)
{
    if (!m_isVisible) return;
    if (!m_tex) return;

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
    float cosf = (float)cos(rad);
    float sinf = (float)sin(rad);

    const float wNorm = 2.0f / (int32_t)designWidth;
    const float hNorm = 2.0f / (int32_t)designHeight;

    s_gfpixelToClipMatrix[0][0] = wNorm * m_rect.width  * cosf;
    s_gfpixelToClipMatrix[1][0] = hNorm * m_rect.height * -sinf;
    s_gfpixelToClipMatrix[0][1] = wNorm * m_rect.width  * sinf;
    s_gfpixelToClipMatrix[1][1] = hNorm * m_rect.height * cosf;

    s_gfpixelToClipMatrix[3][0] = ( wNorm * m_rect.left - 1) * cosf
                              - ( 1 - hNorm * (m_rect.top + m_rect.height))  * sinf;
    s_gfpixelToClipMatrix[3][1] = ( wNorm * m_rect.left - 1 ) * sinf
                              + ( 1 - hNorm * (m_rect.top + m_rect.height))  * cosf;

    nv::vec2<float> thickness;
    thickness.x = m_borderThickness.x; 
    thickness.y = m_borderThickness.y;
    if (thickness.x > m_rect.width / 2)
        thickness.x = m_rect.width / 2;
    if (thickness.y > m_rect.height / 2)
        thickness.y = m_rect.height / 2;
    thickness.x /= m_rect.width/2;
    thickness.y /= m_rect.height/2;

    m_render->Draw(m_alpha, m_color, s_gfpixelToClipMatrix, thickness, m_texBorder, m_drawCenter);
}