//----------------------------------------------------------------------------------
// File:        NvUI/NvUI.cpp
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

// any base implementation stuff can go in here, shared functions, etc.

NvGestureUID NvGestureEvent::lastID = 0; // 0 means gesture system hasn't started yet.

NvUIRenderFactory::GlobalInitPtr NvUIRenderFactory::GlobalInit = NULL;

NvUIRenderFactory::GraphicRenderCreatePtr NvUIRenderFactory::GraphicCreate = NULL;
NvUIRenderFactory::GraphicFrameRenderCreatePtr NvUIRenderFactory::GraphicFrameCreate = NULL;
NvUIRenderFactory::TextureRenderCreatePtr NvUIRenderFactory::TextureRenderCreate = NULL;
NvUIRenderFactory::GlobalRenderPrepPtr NvUIRenderFactory::GlobalRenderPrep = NULL;
NvUIRenderFactory::GlobalRenderDonePtr NvUIRenderFactory::GlobalRenderDone = NULL;

NvUIRenderFactory::GlobalShutdownPtr NvUIRenderFactory::GlobalShutdown = NULL;

//=============================================================================
// NvUIRect
//=============================================================================
NvUIRect::~NvUIRect()
{
    /* empty. */
}


//=============================================================================
// NvUIElement
//=============================================================================
NvUIReaction NvUIElement::ms_reaction;
uint32_t NvUIElement::ms_uiuid_next = 0;
uint32_t NvUIElement::ms_activeSlideInteractGroup = 0;
int32_t NvUIElement::ms_designWidth = 1280;
int32_t NvUIElement::ms_designHeight = 720;

NvUIElement::~NvUIElement()
{
    /* empty. */
}

void NvUIElement::SystemResChange(int32_t w, int32_t h)
{
#if later /* !!!!TBD TODO doesn't apply well to fixed-PIXEL work. */
    if (h<w) // landscape
    {
        ms_designWidth = 800;
        ms_designHeight = 480;
    }
    else
    {
        ms_designWidth = 480;
        ms_designHeight = 800;
    }

    NVBFSetScreenRes((float)ms_designWidth, (float)ms_designHeight);
#endif
    extern void NvBFSetScreenRes(float width, float height);
    NvBFSetScreenRes((float)w, (float)h);
}
