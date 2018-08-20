//----------------------------------------------------------------------------------
// File:        NvUI/NvUIPopup.cpp
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
#include <algorithm>

//======================================================================
//======================================================================
NvUIPopupMenu::NvUIPopupMenu(NvUIPopup *btn, float width, float height, NvUIGraphic *bg/*=NULL*/)
: NvUIContainer(width, height, bg)
, m_myButton(btn)
{
    m_canFocus = true;
    m_canMoveFocus = true;
}

NvUIPopupMenu::~NvUIPopupMenu()
{
}

// now we can define the menu handleevent...
NvUIEventResponse NvUIPopupMenu::HandleEvent(const NvGestureEvent &ev, NvUST timeUST, NvUIElement *hasInteract)
{
    NvUIEventResponse r = INHERITED::HandleEvent(ev, timeUST, hasInteract);
    if (r==nvuiEventNotHandled) // if we didn't handle the event...
    { 
        // and it wasn't INSIDE my button, which can handle itself...
        if (!m_myButton->Hit(ev.x, ev.y))
        {
            // and it was a press or a release...
            if (ev.kind==NvGestureKind::PRESS) // || ev.kind&NvGestureKind::MASK_RELEASED)
            { // then take down the popup...
                m_myButton->PopupFinish();
                // and flag that we handled!
                r = nvuiEventHandledInteract; // but NO reaction...
            }
        }
    }
    else // child handled in some form?
    if ((ev.kind&NvGestureKind::MASK_RELEASED) && !(r & nvuiEventHadReaction)) 
    { // no reaction.  means handled, but nothing tripped.  out of hitrect 99.9%.
        m_myButton->PopupFinish();
    }
    else
    {
    }

    return r;
}


NvUIEventResponse NvUIPopupMenu::HandleFocusEvent(NvFocusEvent::Enum evt)
{
    if (!GetVisibility())
        return nvuiEventNotHandled;

    if (evt == NvFocusEvent::FOCUS_CLEAR)
    {
        // close the menu.
        m_myButton->PopupFinish();
        return nvuiEventHandled;
    }

    NvUIEventResponse r = INHERITED::HandleFocusEvent(evt);
    if (!(r & nvuiEventHandled)) 
    {
        if (evt == NvFocusEvent::ACT_PRESS)
        { // then take down the popup...
            m_myButton->PopupFinish();
            // and flag that we handled!
            r = nvuiEventHandledInteract; // but NO reaction...
        }
    }
    return (NvUIEventResponse)(r | nvuiEventHandled); // while we're up, stay up.
}


//======================================================================
//======================================================================
NvUIPopup::NvUIPopup(uint32_t actionCode, NvUIElement *visrep[NvUIButtonState::MAX],
                        NvUIGraphicFrame *popupFrame, NvUIButton *popper,
                        const char *title/*=NULL*/, float pt/*=0*/, bool shadowed/*=false*/)
: NvUIButton(NvUIButtonType::CHECK, actionCode, visrep, title, pt, shadowed)
, m_menu(NULL)
, m_popper(popper)
, m_itemCount(0)
, m_lineHeight(0)
, m_nextItemTop(0)
, m_padTop(8)
, m_padBottom(12)
{   
    m_canFocus = true;

    m_lineHeight = GetHeight(); // !!!!TBD
    m_titlePre = title;

    SetMaxDrawState(1); // just base+pressed
    SetDrawState(0);
    SetStickyClick(true);
    SetSlideInteractGroup(actionCode); // HACK HACK HACK
            
    m_menu = new NvUIPopupMenu(this, GetWidth(), GetHeight(), popupFrame);
    m_menu->SetSlideInteractGroup(actionCode); // HACK HACK HACK
    m_menu->SetVisibility(false);

    if (m_popper)
        m_popper->SetOrigin(m_rect.left+GetWidth()-m_popper->GetWidth()-8, m_rect.top+(GetHeight()-m_popper->GetHeight())*0.5f);
}

NvUIPopup::~NvUIPopup()
{
    delete m_menu;
    delete m_popper;
}

void NvUIPopup::AddItem(NvUIElement *el, const char *name, uint32_t value)
{
    // the various offsets are really specific to the given popup
    // and should be set as SetYPadTop and SetYPadBottom or the like.
    // !!!!TBD TODO

    // add new item at the next top position (plus top padding)
    m_menu->Add(el, 0, m_nextItemTop + m_padTop);

    // attach new item to our slideInteractgroup, so we can slide through list without releasing.
    el->SetSlideInteractGroup(m_action);

    // update the item name list
    m_itemNames[value] = std::string(name);
    m_itemCount++;

    // then update the bounds of the menu, including padding.
    m_nextItemTop += el->GetHeight(); // update based on bottom of curr element...
    m_menu->SetDimensions(GetWidth(), m_nextItemTop + m_padTop + m_padBottom);
}

void NvUIPopup::PopupStart()
{
    // okay, only WE should be calling this, so safe hack here...
    // shouldn't be another reaction in waiting yet.
    // !!!!TBD yet another reason for a 'reaction queue'...
    // clear the state of our interactive children that match our actioncode.
    NvUIReaction &react = GetReactionEdit(true); // true to clear as new.
    react.flags = NvReactFlag::CLEAR_STATE;
    react.code = GetActionCode();
    react.state = 0; // clear
    react.ival = 0xDEADBEEF; // something nobody would ever match.
    // let the menu handle synchronously.
    m_menu->HandleReaction(react);

    // then add popup to parent, make it visible
    m_parent->AddPopup(m_menu);
    m_menu->SetVisibility(true);
    
    // set button to active highlight
    SetDrawState(1);
}

void NvUIPopup::PopupFinish()
{
    // take down the menu and hide it.
    if (m_parent) m_parent->RemovePopup(m_menu);
    m_menu->DropFocus();
    m_menu->SetVisibility(false);

    // clear us back to no-highlight.  TBD! TODO! focus rect for dpad nav.
    SetDrawState(0); 
    SetPrevDrawState(0);
}

NvUIEventResponse NvUIPopup::HandleEvent(const NvGestureEvent &ev, NvUST timeUST, NvUIElement *hasInteract)
{
    NvUIEventResponse r = INHERITED::HandleEvent(ev, timeUST, hasInteract);
    if (r&nvuiEventHandled)
    {
        const bool vis = m_menu->GetVisibility();
        if (ev.kind==NvGestureKind::PRESS)
        {
            if (!vis)
            {
                PopupStart();
            }
            else if (vis) //&& ev.kind & NvGestureKind::MASK_RELEASED)
            {
                PopupFinish();
                ConsumeGesture(ev); // so we don't double-handle.
            }
        }
    }
    // but our button should NEVER cause a reaction on its own, right?? !!!!!TBD TODO
    r = (NvUIEventResponse)(r&nvuiEventNoReaction);
    return r;
}


NvUIEventResponse NvUIPopup::HandleFocusEvent(NvFocusEvent::Enum evt)
{
    if (evt != NvFocusEvent::ACT_PRESS) return nvuiEventNotHandled;

    NvUIEventResponse r = INHERITED::HandleFocusEvent(evt);
    if (r&nvuiEventHandled)
    {
        if (!m_menu->GetVisibility())
            PopupStart();
        else
            PopupFinish();
    }
    // but our button should NEVER cause a reaction on its own, right?? !!!!!TBD TODO
    r = (NvUIEventResponse)(r&nvuiEventNoReaction);
    return r;
}


void NvUIPopup::UpdateTitleValue(uint32_t value)
{
    // set our title appropriately, in case menu set state value.
    if (m_title)
    {
        ItemValueName::const_iterator bind = m_itemNames.find(value);
        if (bind != m_itemNames.end()) {
            // we have a binding.  do something with it.
            std::string tmps;
            tmps = m_titlePre;
            tmps += ": ";
            tmps += bind->second;
            m_title->SetString(tmps.c_str());
        }
    }
}

void NvUIPopup::SetActiveItemValue(uint32_t value)
{
    UpdateTitleValue(value);
}

NvUIEventResponse NvUIPopup::HandleReaction(const NvUIReaction& react)
{
    // do inherited first, in case we override visual results...
    INHERITED::HandleReaction(react);
    
    if (m_action // our actionCode or FORCE_UPDATE.
        && ((react.flags & NvReactFlag::FORCE_UPDATE) || (react.code==m_action)))
    {
        if (react.uid==m_uiuid) // this is the UIPopup
        {
            // when to tear down.
        }
        else // must be one of the menu items
        {
            // get the menu to actually handle the chosen event for us.
            m_menu->HandleReaction(react);

            UpdateTitleValue(react.ival);

            // tear down in case it's up.
            PopupFinish();
        }
    }
    else
    { // hmmm.  not our action.
        // only continue IF NO FLAGS so clear and update
        // don't cause us to tear-down menu... !!!!TBD TODO
        if (react.flags == 0) 
        {
            // are we up?  if so, tear down.
            if (m_menu->GetVisibility())
                PopupFinish();
        }
    }

    return nvuiEventNotHandled; // !!!!TBD
}

void NvUIPopup::Draw(const NvUIDrawState &ds)
{
    INHERITED::Draw(ds);
    if (m_popper)
        m_popper->Draw(ds);
}

void NvUIPopup::SetDrawState(uint32_t n)
{
    INHERITED::SetDrawState(n);
    if (m_popper)
        m_popper->SetDrawState(GetDrawState());
}


void NvUIPopup::SetOrigin(float x, float y)
{
    INHERITED::SetOrigin(x,y);
    m_menu->SetOrigin(x,y+m_lineHeight-2);
    if (m_title) // override what button does...
    {
        m_title->SetAlignment(NvUITextAlign::LEFT);
        m_title->SetOrigin(x+10,y+4);
    }
    if (m_popper)
    {
        m_popper->SetOrigin(x+GetWidth()-m_popper->GetWidth()-8, y+(GetHeight()-m_popper->GetHeight())*0.5f);
    }
}


void NvUIPopup::GetFocusRect(NvUIRect& rect)
{
    NvUIRect tr;
    GetScreenRect(rect);
    // grow to somewhat encapsulate the button
    rect.Grow(8,8);
}
