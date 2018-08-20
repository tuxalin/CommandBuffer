//----------------------------------------------------------------------------------
// File:        NvUI/NvUIValueText.cpp
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

#include <stdio.h>
#include <string>
#include <sstream>
#include <iomanip>

//======================================================================
//======================================================================
NvUIValueText::NvUIValueText(const char* str, NvUIFontFamily::Enum font, float size, NvUITextAlign::Enum titleAlign,
                            float value, uint32_t precision, NvUITextAlign::Enum valueAlign, uint32_t actionCode/*=0*/)
: NvUIText(str, font, size, titleAlign)
, m_value(value)
, m_integral(false)
, m_precision((precision>6)?6:precision) // cap precision to 6 for now to limit overflow issues.
, m_valueText(new NvUIText("0", font, size, valueAlign))
, m_titleAlign(titleAlign)
, m_valueAlign(valueAlign)
, m_action(actionCode)
{
    SetValue(value);
}

//======================================================================
//======================================================================
NvUIValueText::NvUIValueText(const char* str, NvUIFontFamily::Enum font, float size, NvUITextAlign::Enum titleAlign,
                            uint32_t value, NvUITextAlign::Enum valueAlign, uint32_t actionCode/*=0*/)
: NvUIText(str, font, size, titleAlign)
, m_value((float)value)
, m_integral(true)
, m_precision(0)
, m_valueText(new NvUIText("0", font, size, valueAlign))
, m_titleAlign(titleAlign)
, m_valueAlign(valueAlign)
, m_action(actionCode)
{
    SetValue(value);
}

//======================================================================
//======================================================================
NvUIValueText::~NvUIValueText()
{
    delete m_valueText;
}


//======================================================================
//======================================================================
void NvUIValueText::SetDimensions(float w, float h)
{
    INHERITED::SetDimensions(w, h);
    PositionOutput();
}

//======================================================================
//======================================================================
void NvUIValueText::SetOrigin(float x, float y)
{
    INHERITED::SetOrigin(x, y);
    PositionOutput();
}


//======================================================================
// valid combinations for now are:
// title: left, value: left   == [TITLE VALUE         ]
// title: left, value: right  == [TITLE          VALUE]
// title: right, value: right == [         VALUE TITLE]
//======================================================================
void NvUIValueText::PositionOutput()
{
    const float enSpace = GetFontSize() * 0.25f; // !!!!TBD TODO look up real en space size?
    NvUIRect tr;
    GetScreenRect(tr);
    // reset.
    m_valueText->SetDimensions(tr.width, tr.height);
    m_valueText->SetOrigin(tr.left, tr.top);

    const float tw = INHERITED::GetStringPixelWidth();

    if (m_valueAlign == NvUITextAlign::RIGHT)
    {
        // if both alignments are right, then put number on the LEFT
        if (m_titleAlign == NvUITextAlign::RIGHT)
        {// reset WIDTH of valuetext to base width minus title width minus padding
            if (tw>0) // then we need to offset, else we're fine.
                m_valueText->SetOrigin(tr.left - tw - enSpace, tr.top);
        }
        else // put number far right of rect
        {
            m_valueText->SetOrigin(tr.left + tr.width, tr.top);
        }
    }
    else // else value must be left-aligned, base off title
    {
        if (tw>0) // then we need to offset, else we're fine.
            m_valueText->SetOrigin(tr.left + tw + enSpace, tr.top);
    }
}

//======================================================================
//======================================================================
void NvUIValueText::SetAlpha(float alpha)
{
    INHERITED::SetAlpha(alpha);
    m_valueText->SetAlpha(alpha);
}


//======================================================================
//======================================================================
void NvUIValueText::SetColor(const NvPackedColor& color)
{
    INHERITED::SetColor(color);
    SetValueColor(color);
}


//======================================================================
//======================================================================
void NvUIValueText::SetValueColor(const NvPackedColor& color)
{
    m_valueText->SetColor(color);
}


//======================================================================
//======================================================================
void NvUIValueText::SetValueAlignment(NvUITextAlign::Enum halign)
{
    m_valueAlign = halign;
    m_valueText->SetAlignment(halign);
    PositionOutput();
}


//======================================================================
//======================================================================
void NvUIValueText::SetShadow(char offset/*==DEFAULT_SHADOW_OFFSET*/, NvPackedColor color/*==NV_PC_PREDEF_BLACK*/)
{
    INHERITED::SetShadow(offset, color);
    m_valueText->SetShadow(offset, color);
}


//======================================================================
//======================================================================
void NvUIValueText::SetFontSize(float size)
{
    INHERITED::SetFontSize(size);
    m_valueText->SetFontSize(size);
}


//======================================================================
//======================================================================
void NvUIValueText::SetTextBox(float width, float height, uint32_t lines, uint32_t dots)
{
    INHERITED::SetTextBox(width, height, lines, dots);
    m_valueText->SetTextBox(width, height, lines, dots);
}


//======================================================================
//======================================================================
void NvUIValueText::SetValue(float value)
{
    m_value = value;
    std::stringstream str;
    if (m_integral)
        str << value << std::endl;
    else
        str << std::fixed << std::setprecision(m_precision) << value << std::endl;
    m_valueText->SetString(str.str().c_str());
}


//======================================================================
//======================================================================
void NvUIValueText::SetValue(uint32_t value)
{
    m_integral = true;
    SetValue((float)value);
}


//======================================================================
//======================================================================
void NvUIValueText::Draw(const NvUIDrawState &drawState)
{
    if (!m_isVisible) return;

    INHERITED::Draw(drawState);
    m_valueText->Draw(drawState);
}


//======================================================================
//======================================================================
NvUIEventResponse NvUIValueText::HandleReaction(const NvUIReaction& react)
{
    if ((react.flags & NvReactFlag::FORCE_UPDATE)
        || (m_action && (react.code==m_action)))
    {
        if (m_integral) // uses integer value.
            SetValue((float(react.ival)));
        else // uses the float value
            SetValue(react.fval);
    }
    return nvuiEventNotHandled;
}
