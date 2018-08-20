//----------------------------------------------------------------------------------
// File:        NvUI/NvAFont.h
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

#ifndef _NV_AFONT_H
#define _NV_AFONT_H

#include <NvSimpleTypes.h>

#include <map>

#include <NV/NvTokenizer.h>

#define MAX_AFONT_FILENAME_LEN    1024

// new prototype structs for managing font data in "AngelCode format"
// doing everything in floats so no conversions needed on the fly
struct AFontInfo {
    char m_name[MAX_AFONT_FILENAME_LEN];
    float m_size;
    bool m_isBold;
    bool m_isItalic;
    bool m_isUnicode;
    char m_charset[MAX_AFONT_FILENAME_LEN];
    float m_stretchHeight;
    float m_padding[4];
    float m_spacing[2];
    float m_outline;
    int32_t m_charCount;
};

struct AFontCharCommon {
    float m_lineHeight;
    float m_baseline;
    float m_pageWidth;
    float m_pageHeight;
    float m_pageWidthInv; // precalc inverse.
    float m_pageHeightInv;
    char m_filename[MAX_AFONT_FILENAME_LEN];
    int32_t m_pageID;
};

struct AFontChar {
    int32_t m_idKey; // copy of our ascii/u8 index -- also the char lookup key
    float m_x, m_y;
    float m_width, m_height;
    float m_xOff, m_yOff;
    float m_xAdvance;
    int32_t m_pageID; // NVDHC: no plan to implement immediately
    int32_t m_channelIndex; // NVDHC: no plan to implement immediately
};

struct AFont {
    AFontInfo m_fontInfo;
    AFontCharCommon m_charCommon;
    int32_t m_charCount; // the count listed in the file, not necessarily what's in vector.
    typedef std::map<int32_t, AFontChar> afontGlyphMap;
    afontGlyphMap m_glyphs;
};


struct AFontTokenizer : public NvTokenizer
{
    // temp structures used during parsing...
    AFontInfo finfo;
    AFontCharCommon fcommon;
    AFontChar fchar;

    AFontTokenizer(const char *c) : NvTokenizer(c) { };

    // parse font info block
    // info face="RobotoCondensed-Light" size=36 bold=0 italic=0 charset="" unicode=1 stretchH=100 smooth=1 aa=2 padding=1,1,1,1 spacing=0,0 outline=0 
    bool parseAFontInfoBlock(AFontInfo &finfo)
    {
        if (NULL==mSrcBuf || 0==*mSrcBuf)
            return false;

        float tmpf; // so we can eat values we don't care about.
        if (!requireToken("info"))
            return false;
        if (!requireTokenDelim("face") || !getTokenString(finfo.m_name, sizeof(finfo.m_name)))
            return false;
        if (!requireTokenDelim("size") || !getTokenFloat(finfo.m_size))
            return false;
        if (!requireTokenDelim("bold"))
            return false;
        getTokenBool(finfo.m_isBold); // can return false/0
        if (!requireTokenDelim("italic"))
            return false;
        getTokenBool(finfo.m_isItalic);
        if (!requireTokenDelim("charset"))
            return false;
        getTokenString(finfo.m_charset, sizeof(finfo.m_charset));
        if (!requireTokenDelim("unicode"))
            return false;
        getTokenBool(finfo.m_isUnicode);
        if (!requireTokenDelim("stretchH") || !getTokenFloat(finfo.m_stretchHeight))
            return false;
        // don't care about these values or errors on them.
        if (requireTokenDelim("smooth")) getTokenFloat(tmpf);
        if (requireTokenDelim("aa")) getTokenFloat(tmpf);
        // read arrays....
        if (!requireTokenDelim("padding") || 4!=getTokenFloatArray(finfo.m_padding, 4))
            return false;
        if (!requireTokenDelim("spacing") || 2!=getTokenFloatArray(finfo.m_spacing, 2))
            return false;
        // not required, but we will store if we get it.
        if (requireTokenDelim("outline")) getTokenFloat(finfo.m_outline);
        //requireTokenDelim("outline"); getTokenFloat(finfo.m_size);

        consumeToEOL();
        return true;
    }

    // parse common block
    // common lineHeight=42 base=33 scaleW=512 scaleH=204 pages=1 packed=0 alphaChnl=0 redChnl=0 greenChnl=0 blueChnl=0 page id=0 file="RobotoLight+Bold_36.png"
    bool parseAFontCommonBlock(AFontCharCommon &fcommon)
    {
        if (NULL==mSrcBuf || 0==*mSrcBuf)
            return false;

        int32_t tmpi; // so we can eat values we don't care about.
        if (!requireToken("common"))
            return false;
        if (!requireTokenDelim("lineHeight") || !getTokenFloat(fcommon.m_lineHeight))
            return false;
        if (!requireTokenDelim("base") || !getTokenFloat(fcommon.m_baseline))
            return false;
        if (!requireTokenDelim("scaleW") || !getTokenFloat(fcommon.m_pageWidth)
            || fcommon.m_pageWidth<1 )
            return false;
        fcommon.m_pageWidthInv = 1.0f/fcommon.m_pageWidth;
        if (!requireTokenDelim("scaleH") || !getTokenFloat(fcommon.m_pageHeight)
            || fcommon.m_pageWidth<1 )
            return false;
        fcommon.m_pageHeightInv = 1.0f/fcommon.m_pageHeight;

        // don't care about these values or errors on them right now...
        requireTokenDelim("pages"); getTokenInt(tmpi);
        requireTokenDelim("packed"); getTokenInt(tmpi);
        requireTokenDelim("alphaChnl"); getTokenInt(tmpi);
        requireTokenDelim("redChnl"); getTokenInt(tmpi);
        requireTokenDelim("greenChnl"); getTokenInt(tmpi);
        requireTokenDelim("blueChnl"); getTokenInt(tmpi);

        consumeToEOL();

        // pages is a separate block really, but we're only using ONE PAGE per font,
        // one texture, so parse as part of common info.
        if (!requireToken("page"))
            return false;
        if (!requireTokenDelim("id") || !getTokenInt(fcommon.m_pageID))
            return false;
        if (!requireTokenDelim("file") || !getTokenString(fcommon.m_filename, sizeof(fcommon.m_filename)))
            return false;

        consumeToEOL();

        return true;
    }

    // parse a SINGLE char block
    // char id=10   x=19   y=31   width=0    height=0    xoffset=-1   yoffset=32   xadvance=7    page=0    chnl=0
    bool parseAFontChar(AFontChar &fchar)
    {
        if (NULL==mSrcBuf || 0==*mSrcBuf)
            return false;

        //int32_t tmpi; // so we can eat values we don't care about.
        if (!requireToken("char"))
            return false;
        if (!requireTokenDelim("id") || !getTokenInt(fchar.m_idKey))
            return false;
        if (!requireTokenDelim("x") || !getTokenFloat(fchar.m_x))
            return false;
        if (!requireTokenDelim("y") || !getTokenFloat(fchar.m_y))
            return false;
        if (!requireTokenDelim("width") || !getTokenFloat(fchar.m_width))
            return false;
        if (!requireTokenDelim("height") || !getTokenFloat(fchar.m_height))
            return false;
        if (!requireTokenDelim("xoffset") || !getTokenFloat(fchar.m_xOff))
            return false;
        if (!requireTokenDelim("yoffset") || !getTokenFloat(fchar.m_yOff))
            return false;
        if (!requireTokenDelim("xadvance") || !getTokenFloat(fchar.m_xAdvance))
            return false;

        if (!requireTokenDelim("page") || !getTokenInt(fchar.m_pageID))
            return false;
        if (!requireTokenDelim("chnl") || !getTokenInt(fchar.m_channelIndex))
            return false;

        // adjust for texture sampling NOW
        const float fgrowth = 0.5f;
        fchar.m_x -= fgrowth;
        fchar.m_y -= fgrowth;
        fchar.m_width += (fgrowth*2);
        fchar.m_height += (fgrowth*2);
        //fchar.m_xAdvance += fgrowth; //!!!!TBD TODO should adjust advance??
        
        // offset backward to account for the position changes above.
        fchar.m_xOff -= fgrowth; 
        fchar.m_yOff -= fgrowth; // offset backward for the position change?

        consumeToEOL();
        return true;
    }

    AFont *parseAFont()
    {
        if (NULL==mSrcBuf || 0==*mSrcBuf)
            return NULL;

        // clear all temp structures!!!!. !!!!TBD TODO
    
        // parse info block
        if (!parseAFontInfoBlock(finfo))
            return NULL;

        if (!parseAFontCommonBlock(fcommon))
            return NULL;

        // parse chars block
        // chars count=96
        int32_t charCount;
        if (!requireToken("chars"))
            return NULL;
        if (!requireTokenDelim("count") || !getTokenInt(charCount))
            return NULL;
        consumeToEOL();

        // okay, we've gotten far enough to allocate and not worry about throwing away...
        AFont *font = new AFont(); // so we can start storing stuff...
        font->m_fontInfo = finfo;
        font->m_charCommon = fcommon;
        font->m_charCount = charCount;

        AFontChar fchar;
        for (int32_t i=0; i<charCount; i++) {
            if (!parseAFontChar(fchar))
                break;
            font->m_glyphs[fchar.m_idKey] = fchar;
        }

        // IGNORING kerning for now
        // kernings count=0

        return font;
    }
};

#endif //_NV_AFONT_H
