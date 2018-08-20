//----------------------------------------------------------------------------------
// File:        NvUI/NvUIButton.cpp
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
#include "NV/NvLogs.h"

#include <stdio.h>

#if defined(_DEBUG) && defined(VERBOSE_BUTTON_LOGGING)
#define VERBOSE_PRINT LOGI
#else
#define VERBOSE_PRINT(...)
#endif

//======================================================================
//======================================================================
void NvUIButton::PrivateInit(NvUIButtonType::Enum btntype, uint32_t actionCode)
{
    m_type = btntype;
    m_action = actionCode;
    m_subcode = 0;
    m_stickyClick = false;
    m_reactOnPress = false;
    m_reactGestureUID = NV_GESTURE_UID_INVALID;
    m_wasHit = false;
    m_failedHitUID = NV_GESTURE_UID_INVALID;
    m_hitMarginWide = 0;
    m_hitMarginTall = 0;
    m_slideInteractGroup = 0;

#ifdef BTN_SUPPORTS_HOVER
    m_wantsHover = false;
    m_wasHover = false;
#endif

/*
    m_currDrawState = 0;
    m_prevDrawState = 0;
    m_maxDrawState = 0;
*/

    for (int32_t c=0; c<NvUIButtonState::MAX; c++)
        m_visrep[c] = NULL;
    m_title = NULL;
}


//======================================================================
//======================================================================
NvUIButton::NvUIButton(NvUIButtonType::Enum btntype, uint32_t actionCode, NvUIRect &size,
    const char *title, float pt, bool shadowed/*==false*/)
{
    PrivateInit(btntype, actionCode);

    m_rect = size;

    if (title!=NULL)
        SetTitle(title, pt, shadowed);
}


//======================================================================
//======================================================================
NvUIButton::NvUIButton(NvUIButtonType::Enum btntype, uint32_t actionCode, NvUIElement *states[NvUIButtonState::MAX],
    const char *title/*==NULL*/, float pt/*==0*/, bool shadowed/*==false*/)
{  
    PrivateInit(btntype, actionCode);
           
    for (int32_t c=0; c<NvUIButtonState::MAX; c++)
        m_visrep[c] = states[c];

    if (m_visrep[1] == NULL)
        SetMaxDrawState(0); // just base
    else
    if (m_visrep[2] == NULL)
        SetMaxDrawState(1); // just base+pressed
    else
        SetMaxDrawState(2); // if all three.
            
    if (m_visrep[0]) // grab the active visrep's rectangle.
        m_visrep[0]->GetScreenRect(m_rect);

    if (title!=NULL)
        SetTitle(title, pt, shadowed);
}

//======================================================================
//======================================================================
NvUIButton::NvUIButton(NvUIButtonType::Enum btntype, uint32_t actionCode, NvUIElement *visrep,
    const char *title/*==NULL*/, float ptsize/*==0*/, bool shadowed/*==false*/)
{
    PrivateInit(btntype, actionCode);

    m_visrep[0] = visrep;
            
    if (m_visrep[0]) // grab the active visrep's rectangle.
        m_visrep[0]->GetScreenRect(m_rect);

    if (title!=NULL)
        SetTitle(title, ptsize, shadowed);
}


//======================================================================
//======================================================================
NvUIButton::~NvUIButton()
{
    for (int32_t c=0; c<NvUIButtonState::MAX; c++)
    {
        if (m_visrep[c])
            delete m_visrep[c];
        m_visrep[c] = NULL;
    }
    if (m_title)
        delete m_title;
}


//======================================================================
//======================================================================
void NvUIButton::SetTitle(const char *title, float ptsize, bool shadowed)
{
    const bool push = (m_type==NvUIButtonType::PUSH);
    if (ptsize==0)
        ptsize = (GetHeight() * 0.75f); // some default...

    if (m_title==NULL)
    {
        m_title = new NvUIText(title, NvUIFontFamily::DEFAULT, ptsize,
                                push ? NvUITextAlign::CENTER : NvUITextAlign::LEFT);
    }
    else
    { // already have one, so just call proper setter methods
        m_title->SetString(title);
        m_title->SetFontSize(ptsize);
        m_title->SetAlignment(push ? NvUITextAlign::CENTER : NvUITextAlign::LEFT);
    }

    SetTitleColor(NV_PC_PREDEF_WHITE);

    if (shadowed)
        m_title->SetShadow(4); // a little deeper shadow than the default 2px.
    // for checkbox/radio, the width is totally wrong at this point.
    //m_title->SetTextBox(m_rect.width, m_title->GetFontSize(), 0, 0);
}


//======================================================================
//======================================================================
void NvUIButton::SetTitleColor(const NvPackedColor& c)
{
    m_title->SetColor(c);
}


//======================================================================
//======================================================================
void NvUIButton::SetHitMargin(float hitwide, float hittall)
{
    m_hitMarginWide = hitwide;
    m_hitMarginTall = hittall;
}


//======================================================================
//======================================================================
NvUIEventResponse NvUIButton::HandleEvent(const NvGestureEvent &gdata, NvUST timeUST, NvUIElement *hasInteract)
{
    bool hit = false;       
    
    if (!m_isVisible
    ||  (GetDrawState()==NvUIButtonState::INACTIVE)
#ifndef BTN_SUPPORTS_HOVER
    ||  (gdata.kind<=NvGestureKind::HOVER)
#else
    ||  (gdata.kind<NvGestureKind::HOVER)
    ||    (gdata.kind==NvGestureKind::HOVER && !m_wantsHover)
#endif
        )
        return nvuiEventNotHandled;
        
    bool possibleSlideTarget = false;
    
    if (gdata.uid == m_reactGestureUID) // then we're done?
    {
        VERBOSE_PRINT("early exit same event UID");
        if (this==hasInteract)
            return nvuiEventHandledInteract;
        else
            return nvuiEventHandled; // since we flagged the triggered UID...
    }

    // check if this is a valid, non-focused/non-active slide-focus target
    if (gdata.kind==NvGestureKind::DRAG    // is a drag/slide going on
    &&  GetSlideInteractGroup()               // we have a focus group ID
    &&  GetSlideInteractGroup() == GetActiveSlideInteractGroup()) // and our ID matches active ID
    {
        if (!m_wasHit       // if we aren't flagged as active/hit
        &&  hasInteract        // focus isn't null (early exit) -- note focus could be a higher container rather than another button.
    //        &&  this!=hasInteract  // aren't the focused element (sorta redundant check)
            )
        {
            possibleSlideTarget = true;
        }
    }
        
    if (!possibleSlideTarget
    &&  gdata.uid == m_failedHitUID)
    {
        VERBOSE_PRINT("early exit failed hit");
        return nvuiEventNotHandled;
    }
        
    // verify we've been hit..
    hit = m_rect.Inside((float)(gdata.x+gdata.dx), (float)(gdata.y+gdata.dy),
                            m_hitMarginWide, m_hitMarginTall);
    //VERBOSE_PRINT("[event 0x%x] hit = %s, focus = 0x%x\n", (unsigned int)(gdata.uid), hit?"yes":"no", (uint32_t)hasInteract);
    
    if (possibleSlideTarget)
    { // use drag dx/dy for hit detection of CURRENT position, not gesture start loc.
        if (!hit)
        {
            m_wasHit = false;
            m_failedHitUID = gdata.uid; // so we don't retest.
            VERBOSE_PRINT("!!!!!> slide target 0x%x not hit\n", (uint32_t)this);
            return nvuiEventNotHandled; // we're done.
        }
        VERBOSE_PRINT("!!!!!> slide target 0x%x hit\n", (uint32_t)this);
        m_wasHit = true;                
        m_failedHitUID = 0; // so we retest.
        SetPrevDrawState(GetDrawState());
        
        // .. anything special here??
        // lostInteract on other element will happen in container above us.
    }
    else
    // !!!!!TBD
    // I'm getting MULTIPLE PRESS EVENTS!!!?!?!
    if (!m_wasHit && gdata.kind==NvGestureKind::PRESS)
    {
        if (!hit)
        {
            m_wasHit = false;
            m_failedHitUID = gdata.uid; // so we don't retest.
            return nvuiEventNotHandled; // we're done.
        }
        m_wasHit = true;                
        VERBOSE_PRINT("!!!!!> not hit -> got hit\n");
        SetPrevDrawState(GetDrawState());
        if (GetDrawState()==0 && GetSlideInteractGroup()) // we set the active group on press.
            SetActiveSlideInteractGroup(GetSlideInteractGroup());
        else
            SetActiveSlideInteractGroup(0);
        if (possibleSlideTarget) // we are now!
        {
            // .. anything special here??
            // lostInteract on other element will happen in container above us.
        }
    }
    else
    if (!m_wasHit)
    {
        VERBOSE_PRINT("!!!!!> not hit -> not hit\n");
#if later
        // if we get here we:
        // weren't a valid slide target
        // weren't getting a press (above)
        // hadn't gotten a press yet (!wasHit)
        // hadn't ignored a press (uid exit would have hit above)
        if (gdata.kind==NV_GESTURE_NONE)
        { // TODO -- I realize, this isn't great code.  trying to get something starter-ish in place.
            // already calc'd hit.
            if (hit)
            {
                if (!m_wasHover)
                    SetDrawState(1);
                return nvuiEventHandledHover;
            }
            else //!hit
            {   
                if (!m_wasHover)
                    SetDrawState(0);
            }            
        }
#endif
        return nvuiEventNotHandled;
    }
    else
    if (m_wasHit && gdata.kind==NvGestureKind::DRAG)
    {
        // we're dragging.  but might not be INSIDE/hit.
        VERBOSE_PRINT("> drag\n");
    }
    else
    if (
    //  gdata.kind==NV_GESTURE_FLICK ||
        gdata.kind&NvGestureKind::MASK_RELEASED) // any release state!!!
    {
        // already calc'd hit.
        VERBOSE_PRINT("!!!!!> got release\n");
    }
    else
    if (m_wasHit || this==hasInteract) // keep focus.
    {
        VERBOSE_PRINT("!!!!!> was hit, keep focus\n");
        return nvuiEventHandledInteract;
    }

    VERBOSE_PRINT("!!!!!> secondary processing...\n");
      
    if (hit)
    {
        VERBOSE_PRINT("}}}} was hit\n");
        if (m_type==NvUIButtonType::CHECK)
        { // !!!!TBD
            if (GetPrevDrawState() == GetDrawState()) // we haven't flipped yet
            {
                if (GetDrawState()==NvUIButtonState::ACTIVE)
                    SetDrawState(NvUIButtonState::SELECTED);
                else
                    SetDrawState(NvUIButtonState::ACTIVE);
            }
        }
        else
            SetDrawState(NvUIButtonState::SELECTED);
    }
    else
    if (!m_stickyClick)
    {
        VERBOSE_PRINT("}}}} not hit\n");
        if (m_type!=NvUIButtonType::PUSH)
        { // !!!!TBD
            //if (m_prevDrawState != m_currDrawState) // we flipped, put BACK
                SetDrawStatePrev();
        }
        else
            SetDrawState(NvUIButtonState::ACTIVE);
    }                
        
    // if we came this far, we're hit.  are we done?
    if ( gdata.kind & NvGestureKind::MASK_RELEASED ||
        (m_reactOnPress && gdata.kind == NvGestureKind::PRESS) )
    {
        VERBOSE_PRINT("}}}} got release !!! !!! !!!! !!! !!!\n");
        NvUIEventResponse r = nvuiEventHandled;

        if (hit) // on the release, what's our curr state??
        {
            VERBOSE_PRINT("}}}}}} hit, any reaction?\n");
            if (gdata.uid != m_reactGestureUID)
            {
                // !!!!TBD if a radio button, we really want to stash the
                // state at PRESSED time, so that we know whether to send
                // an event at all!
                m_reactGestureUID = gdata.uid; // so we don't retrigger.
                
                NvUIReaction &react = GetReactionEdit();
                react.uid = m_uiuid;
                react.code = m_action;
                react.state = GetDrawState();
                // in case someone is looking for a value for this reaction.
                if (m_type==NvUIButtonType::RADIO)
                    react.ival = m_subcode;
                else
                    react.ival = react.state; // pass draw state as value.
                react.flags = NvReactFlag::NONE;
                react.causeKind = gdata.kind;
                react.causeIndex = gdata.index;
                r = nvuiEventHandledInteractReaction; // KEEP FOCUS FOR NOW -- else maybe->tap issues !!!!TBD
                VERBOSE_PRINT("}}}}}} prepped reaction\n");
            }
        }

        // reset draw state if button type is Push
        // AFTER reaction, so we get state correct in the message.
        if (m_type==NvUIButtonType::PUSH)
            SetDrawState(NvUIButtonState::ACTIVE);
        
        m_wasHit = false; // reset!
        return r;
    }        
    
    if (m_wasHit || this==hasInteract) // keep focus.
    {
        NvUIEventResponse r = nvuiEventWantsInteract;
        if (hit) {
            r = nvuiEventHandledInteract;
        } else {
            VERBOSE_PRINT("} keep focus ftm..\n");
        }
        return r;
    }

    return nvuiEventNotHandled;
}

//======================================================================
//======================================================================
NvUIEventResponse NvUIButton::HandleReaction(const NvUIReaction& react)
{
    if (m_action // our actionCode or FORCE_UPDATE.
        && ((react.flags & NvReactFlag::FORCE_UPDATE) || (react.code==m_action)))
    {
        if (react.uid == 0) // sent from 'system' rather than a control.
        {
            if (GetDrawState() != react.state )
            {
                SetDrawState(react.state);
                SetPrevDrawState(0);
            }
        }
        else
        if (m_type==NvUIButtonType::RADIO)
        {
            if (react.uid != m_uiuid) // unselect any other buttons with same actionCode
            {
                if (GetDrawState() == NvUIButtonState::SELECTED )
                {
                    SetDrawState(NvUIButtonState::ACTIVE);
                    SetPrevDrawState(0);
                }
            }
            else // we sent the reaction...
            {
            }
        }
    }

    return nvuiEventNotHandled; // !!!!TBD
}


//======================================================================
//======================================================================
NvUIEventResponse NvUIButton::HandleFocusEvent(NvFocusEvent::Enum evt)
{
    NvUIEventResponse r = nvuiEventNotHandled;
    if (!GetVisibility()) return r;

    // we only handle PRESS focus actions for right now.
    if (evt!=NvFocusEvent::ACT_PRESS) return r;

    uint32_t st = GetDrawState();
    // we don't handle actions if we're inactive.
    if (st == NvUIButtonState::INACTIVE) return r;
    
    if (m_type==NvUIButtonType::CHECK)
    { // then toggle.
        if (st == NvUIButtonState::ACTIVE)
            st = NvUIButtonState::SELECTED;
        else
            st = NvUIButtonState::ACTIVE;
        SetDrawState(st);
    }
    else
    if (m_type==NvUIButtonType::RADIO)
    { // then select THIS radio
        st = NvUIButtonState::SELECTED;
        SetDrawState(st);
    }
    else // push button
    { // fake that we're pushed in...
        st = NvUIButtonState::ACTIVE;
        // but don't set drawstate!
    }

    NvUIReaction &react = GetReactionEdit();
    react.uid = m_uiuid;
    react.code = m_action;
    react.state = st;
    // in case someone is looking for a value for this reaction.
    if (m_type==NvUIButtonType::RADIO)
        react.ival = m_subcode;
    else
        react.ival = react.state; // pass draw state as value.
    react.flags = NvReactFlag::NONE;
    r = nvuiEventHandledInteractReaction;
    VERBOSE_PRINT("}}}}}} prepped reaction\n");

    return r;
}

//======================================================================
// radio and check both shouldn't change state when they lose interaction.
//======================================================================
void NvUIButton::LostInteract()
{
    if (m_wasHit)
    {
        VERBOSE_PRINT("[!] wasHit losing focus...\n");
        m_wasHit = false;
    }

    if (m_type!=NvUIButtonType::CHECK
    &&  m_type!=NvUIButtonType::RADIO)
    {
        SetDrawState(GetPrevDrawState());
        SetPrevDrawState(0);
    }
    INHERITED::LostInteract();
}

//======================================================================
//======================================================================
void NvUIButton::Draw(const NvUIDrawState &drawState)
{
    if (!m_isVisible) return;

    const uint32_t state = GetDrawState();
    // eventually this will do some state-based handling for active/inactive, pushed
    NvUIElement *drawme = m_visrep[state];
    if (drawme!=NULL) // pass to our visrep.
        drawme->Draw(drawState);
    if (m_title)
    {
        NvUIDrawState newState(drawState);
        // !!!!TBD TODO
        // ... instead of using alpha, we could draw some a translucent box using our uirect.
        if (m_visrep[0]==NULL && m_visrep[1]==NULL && m_visrep[2]==NULL)
        {
            // then modulate alpha based on draw state.
            if (state==NvUIButtonState::INACTIVE)
            {
                newState.alpha *= 0.25f; // very 'dim'
            }
            else
            if (state==NvUIButtonState::SELECTED)
            {
                newState.alpha *= 0.75f; // slightly 'dimmed'
            }
            // else leave alone.
        }

        m_title->Draw(newState);
    }
}

//======================================================================
//======================================================================
void NvUIButton::SetOrigin(float x, float y)
{
    INHERITED::SetOrigin(x, y);

    for (int32_t c=0; c<NvUIButtonState::MAX; c++)
        if (m_visrep[c]) // pass to our visrep.
            m_visrep[c]->SetOrigin(x, y);

    if (m_title) // !!!!TBD rough margin inset for the title -- might want to calc center?
    {
        // !!!TBD TODO different origin depending on text alignment desired.
        // checkboxes and radios with a space to the left will be left aligned inset
        // normal pushbutton styling will be centered with centered position.
        const bool push = (m_type==NvUIButtonType::PUSH);
        if (push)
            m_title->SetOrigin(x+(GetWidth()/2), y+((GetHeight() - m_title->GetHeight())*0.2f)); 
        else
        {
            float xin = GetHeight()*1.2f;
            if (m_visrep[0])
                xin = m_visrep[0]->GetWidth()*1.2f;
            m_title->SetOrigin(x+xin, y+((GetHeight() - m_title->GetHeight())*0.3f));
        }
    }
}
