//----------------------------------------------------------------------------------
// File:        NvUI/NvUIInternal.h
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
#ifndef NV_UI_INTERNAL_H
#define NV_UI_INTERNAL_H

#include "NvUI/NvUI.h"

#include <NV/NvLogs.h>
#ifdef BITFONT_VERBOSE_LOGGING
#define MODULE "NvBitFont"
#define ERROR_LOG(...) NVDefaultLogError(MODULE, __VA_ARGS__)
#define DEBUG_LOG(...) NVDefaultLogDebug(MODULE, __VA_ARGS__)
#define VERBOSE_LOG(...) NVDefaultLogInfo(MODULE, __VA_ARGS__)
#else
#define ERROR_LOG   LOGI
#define DEBUG_LOG(...)
#define VERBOSE_LOG LOGI
#endif

#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <memory.h>
#include <string.h>
#include <string>
 
class NvUIRenderFactory {
public:
    typedef int32_t(*GlobalInitPtr)();
    typedef NvUIGraphicRender* (*GraphicRenderCreatePtr)(NvUIGraphic* graphic);
    typedef NvUIGraphicFrameRender* (*GraphicFrameRenderCreatePtr)(NvUIGraphicFrame* graphic);
    typedef NvUITextureRender* (*TextureRenderCreatePtr)(NvImage* image);
    typedef void(*GlobalRenderPrepPtr)();
    typedef void(*GlobalRenderDonePtr)();
    typedef void(*GlobalShutdownPtr)();


    static GlobalInitPtr GlobalInit;
    static GraphicRenderCreatePtr GraphicCreate;
    static GraphicFrameRenderCreatePtr GraphicFrameCreate;
    static TextureRenderCreatePtr TextureRenderCreate;
    static GlobalRenderPrepPtr GlobalRenderPrep;
    static GlobalRenderDonePtr GlobalRenderDone;
    static GlobalShutdownPtr GlobalShutdown;
};

#endif
