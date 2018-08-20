//----------------------------------------------------------------------------------
// File:        NvUI/NvUIText.cpp
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

#include "NvUI/NvUI.h"
#include "NvUI/NvBitFont.h"

// !!!!TBD temp until we switch to nvstring across the board
#include <string.h>

//======================================================================
//======================================================================
//======================================================================

//======================================================================
static const char *fontFiles[NvUIFontFamily::COUNT][2] =
{
    {"RobotoCondensed-Regular-24.fnt", "RobotoCondensed-Bold-24.fnt"},
    {"Courier-24.fnt", "Courier-Bold-24.fnt"},
};

//======================================================================
//======================================================================
static bool ready = false;
bool NvUIText::StaticInit(float width/*==1280*/, float height/*==720*/)
{
    if (!ready)
    {
        //int32_t i=0;
        if (NvBFInitialize(NvUIFontFamily::COUNT, fontFiles))
            return false;        
            
        // then set up size, orientation...
        
        // these two are now BF defaults, no need to set.
        //NvBFSetOrthoMode(kOrthoTL00);
        //NvBFSetVGANormalized(0); // disable the normalizations.

        // until people are calling this or the ResChanged function,
        // need to still call explicitly or some apps won't have text
        // res set at all...
        NvBFSetScreenRes(width, height);
        
        ready = true;
    }
   
    return true;
}

//======================================================================
//======================================================================
void NvUIText::StaticCleanup()
{
    if (!ready)
        return;

    NvBFCleanup();

    ready = false;
}

//======================================================================
//======================================================================
NvUIText::NvUIText(const char* str, NvUIFontFamily::Enum font, float size, NvUITextAlign::Enum halign)
: m_size(size)
{
    m_bftext = new NvBFText();
    
    SetColor(NV_PC_PREDEF_WHITE); // white for modulated color == no change.

    // !!!!TBD how to correlate font enum to font index?
    uint8_t fontID = NvUIText::GetFontID(font);
    m_bftext->SetFont(fontID);
    m_bftext->SetSize(size);
    m_bftext->SetString(str);

    SetAlignment(halign);
    
    m_bftext->RebuildCache(0); // so we can get the width!    
    SetDimensions(m_bftext->GetWidth(), m_bftext->GetHeight());
}

//======================================================================
//======================================================================
NvUIText::~NvUIText()
{
    delete m_bftext;
}


//======================================================================
// static method
//======================================================================
uint8_t NvUIText::GetFontID(NvUIFontFamily::Enum font)
{
    if (font >= NvUIFontFamily::COUNT)
        font = NvUIFontFamily::DEFAULT;
    return (NvBFGetFontID(fontFiles[font][0]));
}

//======================================================================
//======================================================================
void NvUIText::SetAlignment(NvUITextAlign::Enum halign)
{
    // pass right on through.
    m_bftext->SetCursorAlign((NvBftAlign::Enum)halign, NvBftAlign::TOP);
}

//======================================================================
//======================================================================
void NvUIText::SetAlpha(float alpha)
{
    NvUIElement::SetAlpha(alpha);
    NV_PC_SET_ALPHA(m_color, (uint8_t)(alpha * 255));
    SetColor(m_color);
}

//======================================================================
//======================================================================
void NvUIText::SetColor(const NvPackedColor& color)
{
    m_color = color;
    m_bftext->SetColor(color);
}

//======================================================================
//======================================================================
void NvUIText::SetShadow(char offset/*==DEFAULT_SHADOW_OFFSET*/, NvPackedColor color/*==NV_PC_PREDEF_BLACK*/)
{
    m_bftext->SetShadow(offset, color);
}

//======================================================================
//======================================================================
void NvUIText::SetTextBox(float width, float height, uint32_t lines, uint32_t dots)
{
    float w,h;

    w = width; // just to store locally, for hittest, etc.
    if (height>0)
        h = height;
    else
        h = m_size; // seems appropriate? !!!!TBD
    
    m_wrap = false;
    if (lines>1)
        m_wrap = true;

    m_bftext->SetBox(w, h, lines, dots);
    
    // update our internal dimensions?  yes, for now. !!!!TBD
    SetDimensions(w, h); // ends up calling UpdateBox, c'est la vie.
}


//======================================================================
//======================================================================
void NvUIText::SetDimensions(float w, float h)
{
    NvUIElement::SetDimensions(w, h);
    m_bftext->UpdateBox(w, h);
}


//======================================================================
//======================================================================
void NvUIText::Draw(const NvUIDrawState &drawState)
{
    if (m_isVisible)
    {
        float myAlpha = m_alpha;
        if (drawState.alpha != 1.0f)
        {
            myAlpha *= drawState.alpha;
            NvPackedColor col = m_color;
            NV_PC_SET_ALPHA(col, (uint8_t)(myAlpha * 255));
            m_bftext->SetColor(col);
        }

        m_bftext->RenderPrep();
        
        m_bftext->SetCursorPos(m_rect.left, m_rect.top);

        m_bftext->Render();
        
        m_bftext->RenderDone();
    }
}

//======================================================================
//======================================================================
void NvUIText::SetFontSize(float size)
{
    m_bftext->SetSize(size);
}

//======================================================================
//======================================================================
void NvUIText::SetString(const char* in)
{
    m_bftext->SetString(in);
}

//======================================================================
//======================================================================
float NvUIText::GetStringPixelWidth()
{
    // call RebuildCache to guarantee we're cached and have width.
    m_bftext->RebuildCache(false);
    return m_bftext->GetWidth();
}

//======================================================================
//======================================================================
void NvUIText::SetDrawnChars(int32_t count)
{
    m_bftext->SetDrawnChars(count);
}
