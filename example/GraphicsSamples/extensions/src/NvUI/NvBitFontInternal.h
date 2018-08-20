//----------------------------------------------------------------------------------
// File:        NvUI/NvBitFontInternal.h
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
#ifndef NV_BITFONT_INTERNAL_H
#define NV_BITFONT_INTERNAL_H

#include "NvUI/NvBitFont.h"
#include "NvAFont.h" // PRIVATE header for afont structs and parser.

#include "NvAssetLoader/NvAssetLoader.h"

#include "NvEmbeddedAsset.h" // NvUI-local system.

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

struct BFVert
{
    float pos[2]; // TBD where we add Z support. !!!!TBD
    float uv[2];
    uint32_t color; // packed 4 byte color.  ABGR with A << 24...
};

//========================================================================
// datatypes & defines
//========================================================================

#define IND_PER_QUAD      6
#define VERT_PER_QUAD     4

class NvImage;

class NvBitFontRender {
public:
    static NvBitFontRender* Create(NvImage *image);

    virtual ~NvBitFontRender() { /* */ }
};

// structure encapsulating a given loaded font
// including texture and all data to map into it.
class NvBitFont
{
public:
    NvBitFont();
    virtual ~NvBitFont();

    static uint8_t GetFontID(const char* filename);

    uint8_t     m_id;        // no need for more than 255, right??
    bool        m_alpha;
    bool        m_rgb;

    char        m_filename[MAX_AFONT_FILENAME_LEN];

    AFont*      m_afont;
    AFont*      m_afontBold; // if we support bold.

    float       m_canonPtSize;

    NvBitFontRender* m_render;

    NvBitFont   *m_next;
};

class NvBFTextRender {
public:
    static NvBFTextRender* Create();

    virtual ~NvBFTextRender() { /* */ }
    virtual void RenderPrep() = 0;
    virtual void Render(const float* matrix, const NvPackedColor& color, NvBitFont* font, bool outline, int count) = 0;
    virtual void UpdateText(int count, const BFVert* data, bool midrender) = 0;
    virtual void RenderDone() = 0;
};
 
class NvBitFontRenderFactory {
public:
    typedef int32_t(*GlobalInitPtr)();
    typedef NvBitFontRender* (*FontRenderCreatePtr)(NvImage* image);
    typedef NvBFTextRender* (*TextRenderCreatePtr)();
    typedef void (*GlobalShutdownPtr)();

    static GlobalInitPtr GlobalInit;
    static FontRenderCreatePtr FontRenderCreate;
    static TextRenderCreatePtr TextRenderCreate;
    static GlobalShutdownPtr GlobalShutdown;
};

#endif
