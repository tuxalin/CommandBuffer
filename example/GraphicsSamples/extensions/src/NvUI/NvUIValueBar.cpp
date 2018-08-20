//----------------------------------------------------------------------------------
// File:        NvUI/NvUIValueBar.cpp
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

//======================================================================
//======================================================================
NvUIValueBar::NvUIValueBar(NvUIGraphicFrame *emptybar, NvUIGraphicFrame *fullbar,
                            bool useRawBorders/*==false*/, bool flipDir/*==false*/)
: NvUIElement()
, m_emptyFrame(emptybar)
, m_fullFrame(fullbar)
, m_flipDirection(flipDir)
, m_minValue(0)
, m_maxValue(100)
, m_value(0)
, m_integral(false)
, m_useRawBorders(useRawBorders)
{
    NvUIRect rect;
    if (m_emptyFrame)
        m_emptyFrame->GetScreenRect(rect);
    else if (m_fullFrame)
        m_fullFrame->GetScreenRect(rect);
    if (m_emptyFrame || m_fullFrame)
        SetDimensions(rect.width, rect.height);
}

//======================================================================
//======================================================================

NvUIValueBar::~NvUIValueBar()
{
    if (m_emptyFrame)
        delete m_emptyFrame;
    if (m_fullFrame)
        delete m_fullFrame;
    m_emptyFrame = NULL;
    m_fullFrame = NULL;
}


//======================================================================
//======================================================================
void NvUIValueBar::SetValue(float value)
{
    if (value < m_minValue) value = m_minValue;
    else if (value > m_maxValue) value = m_maxValue;
    if (m_integral)
    {
        if (value < 0)
            value =  (float) ((int32_t) (value-0.5f));
        else
            value =  (float) ((int32_t) (value+0.5f));
    }
    if (m_value == value) return;

    m_value = value;
    UpdateBar();
}

//======================================================================
//======================================================================
void NvUIValueBar::SetDimensions(float w, float h)
{
    NvUIElement::SetDimensions(w, h);
    if (m_emptyFrame) {
        m_emptyFrame->SetDimensions(w, h);

        if (!m_useRawBorders)
        {
            float thick;
            if (w>=h) // horizontal, use half height
                thick = h/2;
            else // vertical, use half width
                thick = w/2;
            m_emptyFrame->SetBorderThickness(thick);
        }
    }

    UpdateBar();
}

//======================================================================
//======================================================================
void NvUIValueBar::SetOrigin(float x, float y)
{
    NvUIElement::SetOrigin(x, y);
    if (m_emptyFrame)
        m_emptyFrame->SetOrigin(x, y);
    if (m_fullFrame)
        m_fullFrame->SetOrigin(x, y);
        
    UpdateBar();
}

//======================================================================
//======================================================================
void NvUIValueBar::UpdateBar()
{
    if (!m_fullFrame) return;
    float valuePixels;
    float size;
    
    float percent = (m_value - m_minValue) / (m_maxValue - m_minValue);
    if (m_rect.width>=m_rect.height)
    { // horizontal bar
        valuePixels = m_rect.width*percent;

        if (!m_useRawBorders)
            m_fullFrame->SetBorderThickness(m_rect.height/2);

        // Avoid artifacts at the near-zero progress value
        m_fullFrame->GetBorderThickness(&size, NULL);
        if (valuePixels < size*2)
            valuePixels = size*2;

        m_fullFrame->SetDimensions(valuePixels, m_rect.height);
        if (m_flipDirection)
            m_fullFrame->SetOrigin(m_rect.left+(m_rect.width-valuePixels), m_rect.top);
        else
            m_fullFrame->SetOrigin(m_rect.left, m_rect.top);
    }
    else
    { // vertical bar.
        valuePixels = m_rect.height*percent;

        // Avoid artifacts at the near-zero progress value
        if (!m_useRawBorders)
            m_fullFrame->SetBorderThickness(m_rect.width/2);

        // Avoid artifacts at the near-zero progress value
        m_fullFrame->GetBorderThickness(NULL, &size);
        if (valuePixels < size*2)
            valuePixels = size*2;

        m_fullFrame->SetDimensions(m_rect.width, valuePixels);
        if (m_flipDirection)
            m_fullFrame->SetOrigin(m_rect.left, m_rect.top+(m_rect.height-valuePixels));
        else
            m_fullFrame->SetOrigin(m_rect.left, m_rect.top);
    }
}

//======================================================================
//======================================================================
void NvUIValueBar::SetAlpha(float alpha)
{
    NvUIElement::SetAlpha(alpha);
    if (m_fullFrame)
        m_fullFrame->SetAlpha(alpha);
    if (m_emptyFrame)
        m_emptyFrame->SetAlpha(alpha);
}

//======================================================================
//======================================================================
void NvUIValueBar::SetMaxValue(float value)
{
    if (value == m_maxValue) return;

    m_maxValue = value;
    SetValue(m_value);
    UpdateBar();
}

//======================================================================
//======================================================================
void NvUIValueBar::SetMinValue(float value)
{
    if (value == m_minValue) return;

    m_minValue = value;
    SetValue(m_value);
    UpdateBar();
}

//======================================================================
//======================================================================
void NvUIValueBar::SetIntegral(bool isint)
{
    m_integral = isint;
}

//======================================================================
//======================================================================
bool NvUIValueBar::GetIntegral()
{
    return m_integral;
}

//======================================================================
//======================================================================
float NvUIValueBar::GetValue()
{
    return m_value;
}

//======================================================================
//======================================================================
float NvUIValueBar::GetMaxValue()
{
    return m_maxValue;
}

//======================================================================
//======================================================================
float NvUIValueBar::GetMinValue()
{
    return m_minValue;
}

//======================================================================
//======================================================================
void NvUIValueBar::Draw(const NvUIDrawState &drawState)
{
    if (!m_isVisible) return;

    if (m_emptyFrame)
        m_emptyFrame->Draw(drawState);
    if (m_fullFrame)
        m_fullFrame->Draw(drawState);
}