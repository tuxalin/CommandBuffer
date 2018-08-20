//----------------------------------------------------------------------------------
// File:        NvUI/NvUISlider.cpp
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

//======================================================================
//======================================================================
NvUISlider::NvUISlider(NvUIGraphicFrame *emptybar, NvUIGraphicFrame *fullbar,
                        NvUIGraphic *thumb, uint32_t actionCode)
    : NvUIValueBar(emptybar, fullbar)
    , m_thumb(thumb)
    , m_wasHit(false)
    , m_startValue(0)
    , m_stepValue(0)
    , m_action(actionCode)
    , m_smooth(false)
    , m_hitMarginWide(0)
    , m_hitMarginTall(0)
{
    PositionThumb();
}

//======================================================================
//======================================================================

NvUISlider::~NvUISlider()
{
    delete m_thumb;
}


//======================================================================
//======================================================================
void NvUISlider::SetDimensions(float w, float h)
{
    INHERITED::SetDimensions(w, h);
    PositionThumb();
}

//======================================================================
//======================================================================
void NvUISlider::SetOrigin(float x, float y)
{
    INHERITED::SetOrigin(x, y);
    PositionThumb();
}


//======================================================================
//======================================================================
void NvUISlider::PositionThumb()
{
    NvUIRect tr, br;
    m_thumb->GetScreenRect(tr);
    GetScreenRect(br);

    float len = m_maxValue - m_minValue;
    float percent = 0;
    if (len > 0)
        percent = (GetValue()-m_minValue) / len;

    if (br.width>=br.height) //horizontal
        m_thumb->SetOrigin(
            br.left - (tr.width/2) + (br.width*percent),
            br.top - (tr.height/2) + (br.height/2) );
    else //vertical
        m_thumb->SetOrigin(
            br.left - (tr.width/2) + (br.width/2),
            br.top - (tr.height/2) + (br.height*percent) );
}

//======================================================================
//======================================================================
void NvUISlider::SetAlpha(float alpha)
{
    INHERITED::SetAlpha(alpha);
    m_thumb->SetAlpha(alpha);
}

//======================================================================
//======================================================================
void NvUISlider::SetValueReal(float value)
{
    INHERITED::SetValue(value);
    // don't ever early-exit here, as we always need this to check
    // value and update thumb... we can NEVER assume
    // those things don't need updating.

    if (m_stepValue)
    {
        // try to hit step points.
        float tmpval = m_value - m_minValue;
        int32_t tmpinc = (int32_t)((tmpval / m_stepValue) + 0.5f); // rounded number of increments.
        tmpval = m_minValue + (tmpinc*m_stepValue);
        if (tmpval != m_value) { // we modified..
            m_value = tmpval;
            UpdateBar(); // since we changed, need to re-update.
        }
    }

    PositionThumb();
}

//======================================================================
//======================================================================
void NvUISlider::SetValue(float value)
{
    if (!m_wasHit)
    {
        // Only update slider if we aren't sliding it manually (by sliding the thumb)
        SetValueReal(value);
    }
}


//======================================================================
//======================================================================
void NvUISlider::SetHitMargin(float hitwide, float hittall)
{
    m_hitMarginWide = hitwide;
    m_hitMarginTall = hittall;
}


//======================================================================
//======================================================================
void NvUISlider::GetFocusRect(NvUIRect& rect)
{
    NvUIRect tr;
    GetScreenRect(rect);
    m_thumb->GetScreenRect(tr);
    // grow to somewhat encapsulate the thumb
    rect.Grow(tr.width/2, tr.height - rect.height*2); // grow width a small amount, height the delta of the two elements.
}


//======================================================================
//======================================================================
void NvUISlider::Draw(const NvUIDrawState &drawState)
{
    if (!m_isVisible) return;

    INHERITED::Draw(drawState);
    m_thumb->Draw(drawState);
}

//======================================================================
//======================================================================
NvUIEventResponse NvUISlider::HandleEvent(const NvGestureEvent &gdata, NvUST timeUST, NvUIElement *hasInteract)
{
    if (!m_isVisible) return nvuiEventNotHandled;

    // verify we've been hit..
    bool hit = false;
    
    if (gdata.kind==NvGestureKind::PRESS)
    {
        NvUIRect tr;
        m_thumb->GetScreenRect(tr);
        hit = tr.Inside(gdata.x, gdata.y, m_hitMarginWide, m_hitMarginTall);
        m_wasHit = hit; // first press, stash
        m_startValue = GetValue();
    }
    else
    {
        if (!m_wasHit) // if initial press wasn't in the thumb, nothing to do.
            return nvuiEventNotHandled;
            
        if (gdata.kind == NvGestureKind::DRAG
        ||  gdata.kind & NvGestureKind::MASK_RELEASED) // then we switch to alt rect for hysterisis (sp?)
            hit = m_rect.Inside((float)(gdata.x+gdata.dx),
                                (float)(gdata.y+gdata.dy), m_hitMarginWide, m_hitMarginTall);
    }
            
    if (hit)
    {
    }
    else
    {
        // !!!!TBD at some point, if released WELL outside of bounds rect, ignore.
        //m_wasHit = false;
    }                


    // move thumb if dragging
    if (gdata.kind == NvGestureKind::DRAG)
    {
        float newoff = m_startValue;
        NvUIRect br;
        GetScreenRect(br);

        float len = (m_maxValue - m_minValue);
        if (br.width>=br.height) //horizontal, just use dx
            newoff += (gdata.dx/br.width)* len;
        else // vertical
            newoff += (gdata.dy/br.height) * len;
        SetValueReal(newoff);

        if (m_smooth)
        {
            NvUIReaction &react = GetReactionEdit();
            react.uid = m_uiuid;
            react.code = m_action;
            // pass along value as adjusted by SetValueReal...
            react.ival = (uint32_t)m_value;
            react.fval = m_value; 
            return nvuiEventHandledReaction;
        }
    }

    // if we came this far, we're hit.  are we done?
    if (gdata.kind & NvGestureKind::MASK_RELEASED) // we're done?
    {
        if (m_wasHit)
        {
            // leave value/thumb in last good position
            //Trigger();
            // broadcast value change
            m_wasHit = false;

            NvUIReaction &react = GetReactionEdit();
            react.uid = m_uiuid;
            react.code = m_action;
            // pass along value as adjusted by SetValueReal...
            react.ival = (uint32_t)m_value;
            react.fval = m_value; 
            return nvuiEventHandledReaction;
        }
        else // put everything back
        {
            SetValueReal(m_startValue);
            m_startValue = 0;
        }
        
        return nvuiEventHandled;
    }
    if (m_wasHit || this==hasInteract) // keep Interact.
        return nvuiEventHandledInteract;
    
    return nvuiEventNotHandled;
}


//======================================================================
//======================================================================
NvUIEventResponse NvUISlider::HandleReaction(const NvUIReaction& react)
{
    //NvUIEventResponse r = INHERITED::HandleReaction(react);
    // look specifically for a case where a UID >other< than ours
    // is sending our reaction code, for us to update our value.
    // also ensure we allow for wider FORCE_UPDATE, where code==0.
    // NOTE: integral values should be in state, float in val.
    if ((react.uid!=GetUID()) && m_action
        && ((react.flags & NvReactFlag::FORCE_UPDATE) || (react.code==m_action)))
    {
        if (m_integral) // uses integer value.
            SetValue((float(react.ival)));
        else // uses the float value
            SetValue(react.fval);
    }
    return nvuiEventNotHandled;
}


//======================================================================
//======================================================================
NvUIEventResponse NvUISlider::HandleFocusEvent(NvFocusEvent::Enum evt)
{
    const NvUIEventResponse rno = nvuiEventNotHandled;
    if (!GetVisibility()) return rno;

    // we only handle INC/DEC focus actions for right now.
    if (evt!=NvFocusEvent::ACT_INC && evt!=NvFocusEvent::ACT_DEC) return rno;

    uint32_t st = GetDrawState();
    // we don't handle actions if we're inactive.
    if (st == NvUIButtonState::INACTIVE) return rno;

    float dval = 1;
    if (evt==NvFocusEvent::ACT_DEC) dval = -1;
    if (m_stepValue) dval *= m_stepValue;
    SetValueReal(m_value + dval);

    NvUIReaction &react = GetReactionEdit();
    react.uid = m_uiuid;
    react.code = m_action;
    // pass along value as adjusted by SetValueReal...
    react.ival = (uint32_t)m_value;
    react.fval = m_value; 
    return nvuiEventHandledReaction;
}
