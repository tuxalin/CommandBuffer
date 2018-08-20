//----------------------------------------------------------------------------------
// File:        NvUI/NvPackedColor.h
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

#ifndef _NV_PACKEDCOLOR_H
#define _NV_PACKEDCOLOR_H

#include <NvSimpleTypes.h>

/** @file NvPackedColor.h
    Simple abstraction for RGBA colors as parameters to various libraries/functions.
*/

/** A macro to pack four color bytes into a single uint32 in proper memory order. */
#define NV_PACK_COLOR_CHANNELS(r,g,b,a)    ( ( (((uint32_t)(a))&0xFF)<<24 ) | ( (((uint32_t)(b))&0xFF)<<16 ) | ( (((uint32_t)(g))&0xFF)<<8 ) | ( (((uint32_t)(r))&0xFF) ) )

/** A macro for mapping a single packed color channel into its floating point [0,1] rep. */
#define NV_PC_CHANNEL_TO_FLOAT(ch)   ( ch / 255.0f )

//#define NV_PACKEDCOLOR_AS_STRUCT
#ifdef NV_PACKEDCOLOR_AS_STRUCT // uses a struct to break backwards-compat and accidental uses

class NvPackedColor
{
private:
    uint8_t r;
    uint8_t g;
    uint8_t b;
    uint8_t a;

    uint32_t mColorPacked;

public:
    NvPackedColor()
    : r(0), g(0), b(0), a(0), mColorPacked(0)
    { /* no-op */ }

    NvPackedColor(uint8_t ir, uint8_t ig, uint8_t ib, uint8_t ia)
    : r(ir), g(ig), b(ib), a(ia)
    {
        mColorPacked = NV_PACK_COLOR_CHANNELS(r, g, b, a);
    };

/* TBD uncomment when we NEED this variant...
    NvPackedColor(float ir, float ig, float ib, float ia)
    : r((uint8_t)(255*ir))
    , g((uint8_t)(255*ig))
    , b((uint8_t)(255*ib))
    , a((uint8_t)(255*ia))
    {
        mColorPacked = NV_PACK_COLOR_CHANNELS(r, g, b, a);
    };
*/

    bool operator==(const NvPackedColor& pc) {
        return mColorPacked==pc.mColorPacked;
    };

    void setAlpha(uint8_t ia) {
        a = ia;
        // just repack it, not done that often, not that much extra effort...
        mColorPacked = NV_PACK_COLOR_CHANNELS(r, g, b, a);
    };

    uint8_t red() { return r; };
    uint8_t green() { return g; };
    uint8_t blue() { return b; };
    uint8_t alpha() { return a; };

    float redFloat() { return NV_PC_CHANNEL_TO_FLOAT(r); };
    float greenFloat() { return NV_PC_CHANNEL_TO_FLOAT(g); };
    float blueFloat() { return NV_PC_CHANNEL_TO_FLOAT(b); };
    float alphaFloat() { return NV_PC_CHANNEL_TO_FLOAT(a); };

    bool isWhite() { return (r==0xff)&&(g==0xFF)&&(b=0xFF); };

    uint32_t getPackedLong() { return mColorPacked; }
};

static const NvPackedColor static_nvpcwhite((uint8_t)0xFF,0xFF,0xFF,0xFF);
static const NvPackedColor static_nvpcblack((uint8_t)0x00,0x00,0x00,0xFF);

#define NV_PACKED_COLOR(r, g, b, a)     NvPackedColor((uint8_t)(r),(uint8_t)(g),(uint8_t)(b),(uint8_t)(a))

#define NV_PC_PREDEF_WHITE    static_nvpcwhite
#define NV_PC_PREDEF_BLACK    static_nvpcblack

#define NV_PC_RED(c)        ( c.red() )
#define NV_PC_GREEN(c)      ( c.green() )
#define NV_PC_BLUE(c)       ( c.blue() )
#define NV_PC_ALPHA(c)      ( c.alpha() )

#define NV_PC_RED_FLOAT(c)        ( c.redFloat() )
#define NV_PC_GREEN_FLOAT(c)      ( c.greenFloat() )
#define NV_PC_BLUE_FLOAT(c)       ( c.blueFloat() )
#define NV_PC_ALPHA_FLOAT(c)      ( c.alphaFloat() )

#define NV_PC_SET_ALPHA(c, a)     ( c.setAlpha(a) )

#define NV_PC_PACK_UINT(c)  ( c.getPackedLong() )

#define NV_PC_IS_WHITE(c) ( c.isWhite() )

#define NV_PC_EQUAL(x,y)      ( x==y )

#else /* code that doesn't break old pass-as-uint stuff */

/** Main type declaration for a packed 4-color construct. */
typedef uint32_t NvPackedColor;

/** A macro to build a packed color, passing in RGBA as four 8-bit integer values. */
#define NV_PACKED_COLOR(r,g,b,a)    NV_PACK_COLOR_CHANNELS((r),(g),(b),(a))

/** A predefined constant for WHITE. */
#define NV_PC_PREDEF_WHITE    NV_PACKED_COLOR(0xFF, 0xFF, 0xFF, 0xFF)
/** A predefined constant for BLACK. */
#define NV_PC_PREDEF_BLACK    NV_PACKED_COLOR(0x00, 0x00, 0x00, 0xFF)

/** A macro for 'extracting' the red value from an NvPackedColor. */
#define NV_PC_RED(c)        ( ( c>>0 ) & 0xff )
/** A macro for 'extracting' the green value from an NvPackedColor. */
#define NV_PC_GREEN(c)      ( ( c>>8 ) & 0xff )
/** A macro for 'extracting' the blue value from an NvPackedColor. */
#define NV_PC_BLUE(c)       ( ( c>>16 ) & 0xff )
/** A macro for 'extracting' the alpha value from an NvPackedColor. */
#define NV_PC_ALPHA(c)      ( ( c>>24 ) & 0xff )

/** A macro for 'extracting' the red value from an NvPackedColor as a 0..1 float. */
#define NV_PC_RED_FLOAT(c)        ( NV_PC_RED(c)/255.0f )
/** A macro for 'extracting' the green value from an NvPackedColor as a 0..1 float. */
#define NV_PC_GREEN_FLOAT(c)      ( NV_PC_GREEN(c)/255.0f )
/** A macro for 'extracting' the blue value from an NvPackedColor as a 0..1 float. */
#define NV_PC_BLUE_FLOAT(c)       ( NV_PC_BLUE(c)/255.0f )
/** A macro for 'extracting' the alpha value from an NvPackedColor as a 0..1 float. */
#define NV_PC_ALPHA_FLOAT(c)      ( NV_PC_ALPHA(c)/255.0f )

/** A macro for setting just the alpha value of the color, leaving the rest intact. */
#define NV_PC_SET_ALPHA(c, a)     ( ((c)&0xFFFFFF) & ((((uint32_t)(a))&0xFF)<<24) )

/** A macro requesting the packed color repacked into a 32-bit uint32_t.
    This is a no-op for the color-as-uint approach.
*/
#define NV_PC_PACK_UINT(c)  (c)

/** A macro for testing if the base color (excluding alpha) is white. */
// This is the quick-cheat-check...
#define NV_PC_IS_WHITE(c)   (((c)&0xFFFFFF) == 0xFFFFFF )

/** A macro for testing the equality of two NvPackedColors. */
#define NV_PC_EQUAL(x,y)    (x==y)
#endif

#endif // _NV_PACKEDCOLOR_H 
