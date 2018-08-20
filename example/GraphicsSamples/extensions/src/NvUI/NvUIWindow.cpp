//----------------------------------------------------------------------------------
// File:        NvUI/NvUIWindow.cpp
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

#include "NvUI/NvBitFont.h" // !!!TBD TODO for the save/restore state fns.

NvUIWindow::NvUIWindow(float width, float height)
: NvUIContainer(width, height)
{
    // !!!!TBD TODO error handling.
    NvUIText::StaticInit(width, height);
    SystemResChange((int)width, (int)height);
}

NvUIWindow::~NvUIWindow()
{
    NvUIText::StaticCleanup();
}

NvUIEventResponse NvUIWindow::HandleEvent(const NvGestureEvent &ev, NvUST timeUST, NvUIElement *hasInteract)
{
    if (ev.kind > NvGestureKind::HOVER && HasFocus())
        DropFocus();
    // !!!!TBD note we ignore hasInteract, and just pass in the window as the top level focus holder.
    NvUIEventResponse r = INHERITED::HandleEvent(ev, timeUST, this);
    return r;
}

void NvUIWindow::HandleReshape(float w, float h)
{
    // !!!!!TBD TODO !!!!!TBD TODO
    // resize container, notify contents
    // update UIText/BitFont of view size change as needed.

	// CANNOT allow a resize to zero - scaling is done incrementally in NvUI
	// Thus, old*new shows up in the calculations.  Thus, passing in 0 for either
	// dimension kills the positions and sizes forever.  This is a WAR for that
	// design issue
	if (w < 1.0f || h < 1.0f)
		return;

    INHERITED::HandleReshape(w, h);

    // most containers won't just resize, but we're THE WINDOW, we are the VIEW SIZE.
    SetDimensions(w, h);

    // this changes our design w/h statics inside UIElement, so we MUST DO THIS LAST,
    // so that children can compare current design w/h vs incoming new w/h...
    // in the future with gravity positioning, and relative coord spaces, might not be necessary.
    // !!!!TBD TODO for the moment, use SystemResChange to do what we want for text and similar systems.
    SystemResChange((int)w, (int)h);
}

void NvUIWindow::Draw(const NvUIDrawState &drawState)
{
    if (!m_isVisible) return;
	
    NvUIRenderFactory::GlobalRenderPrep();

    INHERITED::Draw(drawState);

    NvUIRenderFactory::GlobalRenderDone();
	
}