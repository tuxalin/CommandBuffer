//----------------------------------------------------------------------------------
// File:        NvUI/NvUIContainer.cpp
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

//======================================================================
//======================================================================
//======================================================================

//======================================================================
//======================================================================
NvUIContainer::NvUIContainer(float width, float height, NvUIGraphic *bg/*==NULL*/)
: m_numChildren(0)
, m_childrenHead(NULL)
, m_childrenTail(NULL)
, m_popup(NULL)
, m_childInteracting(NULL)
, m_consumeClicks(false)
, m_childFocused(NULL)
, m_focusHilite(NULL)
{
    SetBackground(bg);
    SetDimensions(width, height);

    m_canMoveFocus = true; // we can by default...
}

//======================================================================
//======================================================================
NvUIContainer::~NvUIContainer()
{
    NvUIElement *delme;
    
    // clean up focus.
    DropFocus();
    delete m_focusHilite;
    SetFocusHilite(NULL);

    // iterate and delete all the children, container OWNS them.
    // easier to use while loop vs for loop for this type of operation.
    LostInteract(); // just in case.
    while (m_childrenHead != NULL)
    {
        delme = m_childrenHead;
        m_childrenHead = m_childrenHead->m_llnext;
        if (delme == m_childrenHead) // looped
            m_childrenHead = NULL;
        delete delme;
    }
    m_childrenHead = m_childrenTail = NULL;

    if (m_background)
        delete m_background;
}


//======================================================================
// this base Add just handles adding to the 'list' and count, nothing more.
// BUT, given draw order, we need to
//======================================================================
void NvUIContainer::Add(NvUIElement *el)
{
    if (el == NULL) // !!!!TBD OOOPS
    {
        LOGE("!!> OOOOPS.  Tried to add NULL to container.");
        return;
    }

    if (el->m_llnext != NULL) // !!!!TBD OOOPS already linked in somewhere.
    {
        LOGE("!!> OOOOPS.  Tried to add a UI element to container, but it already is on a LL apparently.");
        return;
    }

    // !!!!TBD these are really sanity checks, but shouldn't be necessary
    // if everything else is clean...
    el->m_llnext = NULL;
    el->m_llprev = NULL;

    // if no list, we start it.  prev & next are us for single item.
    if (m_childrenHead==NULL)
    {
        m_childrenHead = el;
        m_childrenTail = el;
    }
    else
    {
        el->m_llprev = m_childrenTail; // prev pts to tail, next stays null.
        m_childrenTail->m_llnext = el; // tail pts to us now.
        m_childrenTail = el; // we take over as tail.
    }

    el->SetParent(this);

    m_numChildren++;
}


//======================================================================
//======================================================================
void NvUIContainer::Add(NvUIElement *el, float desiredleft, float desiredtop)
{
    if (el == NULL) // !!!!TBD OOOPS
    {
        LOGE("!!> OOOOPS.  Tried to add NULL to container.");
        return;
    }

    if (el->m_llnext != NULL) // !!!!TBD OOOPS already linked in somewhere.
    {
        LOGE("!!> OOOOPS.  Tried to add a UI element to container, but it already is tagged as being in a container elsewhere.");
        return;
    }

    Add(el);
    el->SetOrigin(m_rect.left+desiredleft, m_rect.top+desiredtop);
}
    

//======================================================================
//======================================================================
bool NvUIContainer::Remove(NvUIElement *child)
{
    LostInteract();

    NvUIElement *findme = m_childrenHead;
    // we search for this child before mucking with
    // next/prev, in case it's not IN our list!
    while (findme)
    {
        if (child==findme) // we're good
        {
            m_numChildren--; // dropping him.
            if (m_numChildren==0) // was all we had, quick out.
            {
                m_childrenHead = NULL;
                m_childrenTail = NULL;
            }
            else
            {
                if (child==m_childrenHead) 
                    m_childrenHead = child->m_llnext;
                else // then prev is valid.
                    child->m_llprev->m_llnext = child->m_llnext;

                if (child==m_childrenTail)
                    m_childrenTail = child->m_llprev;
                else // then next is valid
                    child->m_llnext->m_llprev = child->m_llprev;
            }
                
            child->m_llnext = NULL;
            child->m_llprev = NULL;

            child->SetParent(NULL);

            return true;
        }
        
        findme = findme->m_llnext;
    }

    return false;
}

//======================================================================
//======================================================================
bool NvUIContainer::MakeChildFrontmost(NvUIElement *child)
{
    if (child==NULL)
        return false; // done.
    if (child==m_childrenTail) // already done
        return true;

    NvUIElement *findme = m_childrenHead;
    // we search for this child before mucking with
    // next/prev, in case it's not IN our list!
    while (findme)
    {
        if (child==findme) // we're good
        {
            // first, pull from list.
            if (child==m_childrenHead) 
                m_childrenHead = child->m_llnext;
            else // then prev is valid.
                child->m_llprev->m_llnext = child->m_llnext;
            // child can't be tail, we would have early-exited.
            child->m_llnext->m_llprev = child->m_llprev;

            // then, add to end.                
            child->m_llnext = NULL; // next should be null
            child->m_llprev = m_childrenTail; // prev pts to curr tail
            m_childrenTail->m_llnext = child; // tail pts to us now.
            m_childrenTail = child; // we take over as tail.

            return true;
        }
        
        findme = findme->m_llnext;
    }
    
    return false;
}


//======================================================================
//======================================================================
void NvUIContainer::SetOrigin(float x, float y)
{
    NvUIRect sr;

    GetScreenRect(sr); // grab the container loc

    if (x==sr.left && y==sr.top)
        return; // nothing to do!

    // store delta offset for this 'move'...
    float dx = x - sr.left;
    float dy = y - sr.top;
    
    NvUIElement::SetOrigin(x, y);

    for (NvUIElement *child = m_childrenHead; child; child = child->m_llnext)
    {
        child->GetScreenRect(sr); // grab the loc
        child->SetOrigin(sr.left+dx, sr.top+dy);
    }

    if (m_background)
    {
        m_background->GetScreenRect(sr); // grab the loc
        m_background->SetOrigin(sr.left+dx, sr.top+dy);
    }
}

//======================================================================
//======================================================================
void NvUIContainer::SetDimensions(float w, float h)
{
    NvUIElement::SetDimensions(w, h);
    if (m_background)
        m_background->SetDimensions(w, h);
}


//======================================================================
//======================================================================
void NvUIContainer::LostInteract()
{
    if (m_childInteracting)
    {
        m_childInteracting->LostInteract();
        m_childInteracting = NULL;
    }
}


//======================================================================
//======================================================================
NvUIEventResponse NvUIContainer::HandleEvent(const NvGestureEvent &ev, NvUST timeUST, NvUIElement *hasInteract)
{
    NvUIEventResponse r = nvuiEventNotHandled;
    NvUIElement *childHadInteract = m_childInteracting;
    NvUIElement *childWantsInteract = NULL;

    if (!m_isVisible)
        return r;

// !!!!TBD handling focus changes.
    if (this!=hasInteract) // we lost it, cascade loss FIRST.
        LostInteract();

    if (!m_childInteracting && hasInteract) // temporarily promote it to 'our child' status
        childHadInteract = hasInteract;

    // !!!!TBD for now, don't check/filter ev->type, we assume always basic gesture/input.
    {
        // NEW strategy.  Handle popup before anything else.
        if (m_popup && m_popup!=childHadInteract)
        {
            NvUIElement *dome = m_popup;
            r = dome->HandleEvent(ev, timeUST, childHadInteract);
            if (r&nvuiEventWantsInteract && m_popup) // ensure we didn't unpop
                childWantsInteract = dome;
        }

        if (m_childInteracting && !(r&nvuiEventHandled))
        {
            r = m_childInteracting->HandleEvent(ev, timeUST, m_childInteracting);
            if (!(r&nvuiEventWantsInteract))
                LostInteract(); // will clear the child AND tell it to clear...
        }

        if (!(r&nvuiEventHandled)
        && ((!m_childInteracting) || (GetActiveSlideInteractGroup()) ) // sliding going on...
        && (/*ev.kind>NvGestureKind::PRESS ||*/ m_rect.Inside(ev.x+ev.dx, ev.y+ev.dy)) // if not focused, only care about events inside.
            )
        { // we need to handle events BACKWARDS, so 'top' elements get first shot.
            if (!(r&nvuiEventHandled))
                for (NvUIElement *dome = m_childrenTail; dome; dome = dome->m_llprev)
                {
                    if (dome==childHadInteract)
                        continue; // did me already.                
                    r = dome->HandleEvent(ev, timeUST, childHadInteract);
                    if (r&nvuiEventWantsInteract)
                        childWantsInteract = dome;
                    if (r&nvuiEventHandled)
                        break;
                }
        }

        if (childWantsInteract)
        {
            LostInteract();
            m_childInteracting = childWantsInteract;
        }
        
        if (r)
            return r;
    }
    
    if (m_consumeClicks)
    {
        bool b = m_rect.Inside(ev.x+ev.dx, ev.y+ev.dy);
        if (b && ev.kind==NvGestureKind::PRESS)
        {
            if (m_childInteracting==NULL)
            {
                // we want to fake it up a layer.
                return (NvUIEventResponse)(nvuiEventHandled | nvuiEventWantsInteract); // as for container to keep our focus.
            }
        }
        else
        if (b && ev.kind>NvGestureKind::PRESS && hasInteract==this)
        {
            return nvuiEventHandled;
        }
    }

    return nvuiEventNotHandled;
}


//======================================================================
//======================================================================
NvUIEventResponse NvUIContainer::HandleReaction(const NvUIReaction& react)
{
    NvUIEventResponse r = nvuiEventNotHandled, rme;
//    NvUIElement *active = NULL;
    for (NvUIElement *child = m_childrenHead; child; child = child->m_llnext)
    {
        rme = child->HandleReaction(react); // !!!!TBD
//        if (!active && (rme & nvuiEventHandledFocus))
//            active = child;
        if (rme)
            r = rme;
    }
    
//    if (active)
//        MakeChildFrontmost(active);

    return r;
}


//======================================================================
//======================================================================
void NvUIContainer::HandleReshape(float w, float h)
{
    NvUIRect cr;
    GetScreenRect(cr); // hold onto it.
    
    // in case inherited does something useful, call it.
    INHERITED::HandleReshape(w, h);

    // then loop over children.
    for (NvUIElement *child = m_childrenHead; child; child = child->m_llnext)
    {
        child->HandleReshape(w, h); // !!!!TBD

        // let's do some hacked repositioning for elements that look like they
        // are aligned right or bottom.
        
        // !!!!TBD TODO THIS IS AN UTTER HACK.
        // Should have alignment properties at UIElement level,
        // and have position relative to alignment, like TextBox does...
        NvUIRect uir;
        child->GetScreenRect(uir);

        float newleft, newtop;
        newleft = uir.left;
        newtop = uir.top;
        // within 1/4 of view width, assume right aligned.
        if (uir.left-cr.left > cr.width*0.75f)
        {
            newleft = newleft + (w - cr.width);
            // ONLY adjust y position if we're right-aligned for the moment...
            // within 1/4 of view height, assume bottom aligned.
            if (uir.top-cr.top > cr.height*0.75f)
                newtop = newtop + (h - cr.height);
        }

        // if values changed, set new origin.
        if (newleft != uir.left || newtop != uir.top)
            child->SetOrigin(newleft, newtop);
    }
}


//======================================================================
//======================================================================
NvUIEventResponse NvUIContainer::HandleFocusEvent(NvFocusEvent::Enum evt)
{
    const NvUIEventResponse rno = nvuiEventNotHandled;

    if (!m_isVisible)
    {
        m_hasFocus = false; // make sure state is correct.
        return rno;
    }

    // popup always gets first shot.  if we get a clear, it takes it first to hide.
    if (m_popup)
    {
        NvUIEventResponse pr = m_popup->HandleFocusEvent(evt);
        if (pr & nvuiEventHandled)
        {
            //m_hasFocus = true;
            return pr;
        }
        // else we should have dropped the popup, so move on to next...
    }

    if (evt == NvFocusEvent::FOCUS_CLEAR)
    {
        DropFocus();
        return nvuiEventHandled; // since we handled.
    }

    if (evt & NvFocusEvent::FLAG_MOVE)
    {
        NvUIEventResponse r = nvuiEventNotHandled;

        // let focused child have first shot.
        if (m_childFocused!=NULL && m_childFocused->CanMoveFocus())
            r = m_childFocused->HandleFocusEvent(evt);
        if (r & nvuiEventHandled)
        {
            // focus didn't change at our level, just return response.
            return r;
        }

        // !!!!TBD TODO for now, we're doing moveto==DOWN/NEXT
        NvUIElement *child = NULL;
        if (evt==NvFocusEvent::MOVE_DOWN)
        {
            child = m_childFocused ? m_childFocused->m_llnext : m_childrenHead;
            for (; child; child = child->m_llnext)
                if (child->CanFocus())
                    break; // found eligible child.
        }
        else
        if (evt==NvFocusEvent::MOVE_UP)
        {
            child = m_childFocused ? m_childFocused->m_llprev : m_childrenTail;
            for (; child; child = child->m_llprev)
                if (child->CanFocus())
                    break; // found eligible child.
        }
        else
        {
            return rno; // don't handle that yet...  !!!!TBD TODO
        }

        m_childFocused = child; // which might be NULL.

        UpdateFocusState(); // our local hilite.

        if (child!=NULL && child->CanMoveFocus())
            child->HandleFocusEvent(evt); // note each level of child might set it's own focus hilite...

        if (child==NULL)
        {
            m_hasFocus = false;
            return rno; // so not handled if no child to focus, OR we go off the end.
        }

        // else we handled this.
        m_hasFocus = true;
        return nvuiEventHandled;
    }

    // else FLAG_ACT
    if (m_childFocused == NULL)
    {
        m_hasFocus = false;
        return rno;
    }

    // pass it along as only the individual classes know what to do.
    m_hasFocus = true;
    return m_childFocused->HandleFocusEvent(evt);
}


//======================================================================
//======================================================================
void NvUIContainer::DropFocus()
{
    if (m_popup)
        m_popup->DropFocus(); // TODO!!!TBD

    if (m_childFocused)
    {
        m_childFocused->DropFocus();
        m_childFocused = NULL;
    }

    UpdateFocusState();

    m_hasFocus = false;
}


//======================================================================
//======================================================================
void NvUIContainer::UpdateFocusState()
{
    if (m_focusHilite==NULL) return;

    if (m_childFocused==NULL)
    {
        m_hasFocus = false;
        if (m_focusHilite)
            m_focusHilite->SetVisibility(false);
    }
    else
    {
        m_hasFocus = true;
        if (m_focusHilite)
        {
            NvUIRect cr;
            m_childFocused->GetFocusRect(cr);
            m_focusHilite->SetOrigin(cr.left, cr.top);
            m_focusHilite->SetDimensions(cr.width, cr.height);
            m_focusHilite->SetVisibility(true);
        }
    }
}


//======================================================================
//======================================================================
void NvUIContainer::Draw(const NvUIDrawState &drawState)
{
    if (!m_isVisible) return;

    if (drawState.designHeight && drawState.designWidth)
    {
        if( (m_rect.top>drawState.designHeight) ||
            (m_rect.left>drawState.designWidth) ||
            ((m_rect.top+m_rect.height)<0.0f) ||
            ((m_rect.left+m_rect.width)<0.0f) )
        {
            return;
        }
    }
    
    NvUIDrawState myds = drawState;
    if (m_alpha!=1.0f)
        myds.alpha *= m_alpha;

    if (m_background)
        m_background->Draw(myds);

    if (m_hasFocus && m_childFocused && m_childFocused->ShowFocus())
    {
        if (m_focusHilite) // it should already be in position
            m_focusHilite->Draw(myds);
    }

    for (NvUIElement *drawme = m_childrenHead; drawme; drawme = drawme->m_llnext)
        drawme->Draw(myds);

    if (m_popup)
        m_popup->Draw(myds);
}


//======================================================================
//======================================================================
void NvUIContainer::AddPopup(NvUIElement *el)
{
    NvUIContainer *myparent = GetParent();
    if (myparent)
        myparent->AddPopup(el);
    else
        m_popup = el;
}


//======================================================================
//======================================================================
void NvUIContainer::RemovePopup(NvUIElement *el)
{
    NvUIContainer *myparent = GetParent();
    if (myparent)
        myparent->RemovePopup(el);
    else
    if (m_popup == el)
        m_popup = NULL;
}
