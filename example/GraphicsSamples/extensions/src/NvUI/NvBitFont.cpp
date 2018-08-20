//----------------------------------------------------------------------------------
// File:        NvUI/NvBitFont.cpp
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

#include "NvUI/NvBitFont.h"
#include "NvBitFontInternal.h"
#include "NvAFont.h" // PRIVATE header for afont structs and parser.

#include "NvAssetLoader/NvAssetLoader.h"
#include "NvImage/NvImage.h"

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

//========================================================================
// datatypes & defines
//========================================================================

static NvPackedColor s_charColorTable[6] =
{
    NV_PACKED_COLOR(0xFF, 0xFF, 0xFF, 0xFF), //white
    NV_PACKED_COLOR(0x99, 0x99, 0x99, 0xFF), //medium-gray
    NV_PACKED_COLOR(0x00, 0x00, 0x00, 0xFF), //black
    NV_PACKED_COLOR(0xFF, 0x33, 0x33, 0xFF), //brightened red
    NV_PACKED_COLOR(0x11, 0xFF, 0x11, 0xFF), //brighter green
    NV_PACKED_COLOR(0x33, 0x33, 0xFF, 0xFF) //brightened blue
};


//========================================================================
// static vars
//========================================================================

NvBitFontRenderFactory::GlobalInitPtr NvBitFontRenderFactory::GlobalInit = NULL;
NvBitFontRenderFactory::FontRenderCreatePtr NvBitFontRenderFactory::FontRenderCreate = NULL;
NvBitFontRenderFactory::TextRenderCreatePtr NvBitFontRenderFactory::TextRenderCreate = NULL;
NvBitFontRenderFactory::GlobalShutdownPtr NvBitFontRenderFactory::GlobalShutdown = NULL;

static bool s_bfInitialized = false;

#if defined(ANDROID) // android needs higher dpi factoring until real DPI adjustments added...
static float s_bfShadowMultiplier = 0.80f; // around 2x dpi adjust.
#else
static float s_bfShadowMultiplier = 0.40f;
#endif

// these track the bitmap font loading.
static NvBitFont *bitFontLL = NULL;
static uint8_t bitFontID = 1; // NOT ZERO, start at 1 so zero is 'invalid'...

//========================================================================
static float dispW = 0, dispH = 0;
static float dispAspect = 0;
static float dispRotation = 0;

//========================================================================
//========================================================================
static uint8_t lastTextMode = 111;

static float s_pixelToClipMatrix[4][4];
static float s_pixelScaleFactorX = 2.0f / 640.0f;
static float s_pixelScaleFactorY = 2.0f / 480.0f;

const float *m_matrixOverride = NULL;

//========================================================================
//========================================================================
NvBitFont::NvBitFont()
: m_id(0)
, m_alpha(false)
, m_rgb(false)
, m_afont(NULL)
, m_afontBold(NULL)
, m_canonPtSize(10)
, m_render(NULL)
, m_next(NULL)

{
    // hook into ll
    m_next = bitFontLL;
    bitFontLL = this;
}

NvBitFont::~NvBitFont()
{
}

//========================================================================
//========================================================================
static NvBitFont *BitFontFromID(uint8_t fontnum)
{
    NvBitFont *bitfont = bitFontLL;
    while (bitfont)
    {
        if (bitfont->m_id==fontnum) // matched
            return bitfont;
        bitfont = bitfont->m_next;
    }
    
    return NULL;
}


//========================================================================
// this is the ONLY externalized access to the bitfont list
// used to match up a filename with the internal id/fontnum it got.
//========================================================================
uint8_t NvBFGetFontID(const char* filename)
{
    NvBitFont *bitfont = bitFontLL;
    while (bitfont)
    {
        if (0==strcmp(filename, bitfont->m_filename))
            return bitfont->m_id;
        bitfont = bitfont->m_next;
    }
    
    return 0;
}


AFont *LoadFontInfo(const char *fname)
{
    AFont *afont = NULL;
    const uint8_t *data = 0;
    uint32_t len;
    int32_t ilen;
    char *tmpdata = NULL;

    if (0!=strcmp(fname+strlen(fname)-3, "fnt")) // we now only support angelcode format specs.
    {
        ERROR_LOG(">> Invalid font file specified: %s...\n", fname);
        return NULL;
    }

    if (NvEmbeddedAssetLookup(fname, data, len))
    {
        if (data!=NULL && len!=0)
        {
            AFontTokenizer ftok((const char*)data);
            afont = ftok.parseAFont();
            if (NULL==afont)
            {
                ERROR_LOG(">> FAILED TO PARSE afont data file: %s...\n", fname);
                return NULL;
            }
        }
    }

    if (NULL==afont)
    { // didn't init the font yet, no embedded data, check file.
        tmpdata = NvAssetLoaderRead(fname, ilen);
        if (tmpdata==NULL) // ugh
        {
            ERROR_LOG(">> FAILED TO FIND afont data file: %s...\n", fname);
            return NULL;
        }
        // else... got data, load it up.
        AFontTokenizer ftok(tmpdata);
        afont = ftok.parseAFont();
        NvAssetLoaderFree((char*)tmpdata);
        if (NULL==afont)
        {
            ERROR_LOG(">> FAILED TO PARSE afont data file: %s...\n", fname);
            return NULL;
        }
    }

    return afont;
}


//========================================================================
// !!!!TBD needs a lot more error handling with finding the files...
// should also allow a method for being handed off the data from the app,
// rather than opening the files here.  split this into two funcs!!!!!TBD
//========================================================================
int32_t NvBFInitialize(uint8_t count, const char* filename[][2])
                 //, int32_t mipmap) // the mipmap setting is needed only if we add back RAW support.
{
    int32_t j;

    char texFilename[MAX_AFONT_FILENAME_LEN];

    const uint8_t *data = 0;
    uint32_t len = 0;
    NvImage *image = NULL;

    NvBitFont *bitfont = NULL;
    AFont *afont = NULL;

    int32_t fontsLoaded = 0;

    NvBitFontRenderFactory::GlobalInit();

    // The following entries are const
    // so we set them up now and never change
    s_pixelToClipMatrix[2][0] = 0.0f;
    s_pixelToClipMatrix[2][1] = 0.0f;

    // Bitfont obliterates Z right now
    s_pixelToClipMatrix[0][2] = 0.0f;
    s_pixelToClipMatrix[1][2] = 0.0f;
    s_pixelToClipMatrix[2][2] = 0.0f;
    s_pixelToClipMatrix[3][2] = 0.0f;

    s_pixelToClipMatrix[0][3] = 0.0f;
    s_pixelToClipMatrix[1][3] = 0.0f;
    s_pixelToClipMatrix[2][3] = 0.0f;
    s_pixelToClipMatrix[3][3] = 1.0f;

    for (j=0; j<count; j++)
    {
        // !!!!TBD TODO filename could still be garbage here, better safety checks.
        const size_t flen = strlen(filename[j][0]);
        if (flen >= MAX_AFONT_FILENAME_LEN)
        {
            ERROR_LOG("!!> Bitfont file name too long, max %d, name: %s\n", MAX_AFONT_FILENAME_LEN, filename[j][0]);
            continue;
        }

        if (NvBFGetFontID(filename[j][0]))
        {
            fontsLoaded++;
            continue; // already have this one.
        }
            
        image = NULL;
        bitfont = NULL;

        afont = LoadFontInfo(filename[j][0]);
        if (NULL==afont)
        {
            ERROR_LOG(">> FAILED TO PARSE afont file: %s...\n", filename[j][0]);
            continue;
        }
        LOGI("!> NvBF loaded afont: [%s]", afont->m_fontInfo.m_name);

        // build the filename.
        memcpy(texFilename, afont->m_charCommon.m_filename, strlen(afont->m_charCommon.m_filename)+1); // account for null!
        if (0!=strcmp(texFilename+strlen(texFilename)-3, "dds"))
        {
            ERROR_LOG("Font [%s] wasn't a .DDS file, the only supported format.\n", texFilename);
            if (afont)
                delete afont;
            continue;            
        }

        bool oldFlip = NvImage::GetVerticalFlip();
        NvImage::VerticalFlip( false );
        if (NvEmbeddedAssetLookup(texFilename, data, len))
        {
            if (data!=NULL && len!=0)
            {
                image = new NvImage;
                if (!image->loadImageFromFileData(data, len, "dds"))
                {
                    delete image;
                    image = NULL;
                }
            }
        }
        if (image==NULL)
            image = NvImage::CreateFromDDSFile(texFilename);
        NvImage::VerticalFlip( oldFlip );
        if (image==NULL)
        {
            ERROR_LOG("Font [%s] couldn't be loaded by the NVHHDDS library.\n", texFilename);
            if (afont)
                delete afont;
            continue;
        }
        // !!!!!!TBD TODO
        //texw = dds->width;

        if (afont)
        {
            // create now and copy
            bitfont = new NvBitFont();
            bitfont->m_afont = afont;
            bitfont->m_canonPtSize = afont->m_charCommon.m_lineHeight; // !!!!!TBD TODO what to use to get approx ACTUAL font pixel size?

            // try to load second font style if passed in...
            if (filename[j][1][0])
            {
                afont = LoadFontInfo(filename[j][1]);
                if (NULL==afont)
                {
                    ERROR_LOG(">> FAILED TO PARSE secondary style afont file: %s...\n", filename[j][1]);
                    continue;
                }
                LOGI("!> NvBF loaded second style afont: [%s]", afont->m_fontInfo.m_name);
                bitfont->m_afontBold = afont;
            }
        }

        bitfont->m_id = bitFontID++; // store our 'index' away
        // we've already checked earlier that filenames fit in our MAX LEN.
        memcpy(bitfont->m_filename, filename[j][0], strlen(filename[j][0])+1); // copy the null!

        uint32_t fmt = image->getFormat();
        bitfont->m_alpha = image->hasAlpha();
        bitfont->m_rgb = (fmt != NVIMAGE_LUMINANCE && fmt != NVIMAGE_ALPHA && fmt != NVIMAGE_LUMINANCE_ALPHA); // this is a cheat!!

        // GL initialization...
        bitfont->m_render = NvBitFontRenderFactory::FontRenderCreate(image);

        // okay, at this point we're ready to go, so flag we're okay, allocate a font struct, setup GL stuff.
        fontsLoaded++;

        delete image;
    }

    if (fontsLoaded!=count)
        ERROR_LOG("!!> couldn't load all requested fonts\n");

    s_bfInitialized = true;

    return (fontsLoaded!=count);
}


//========================================================================
// this function has a multi-tiered job
// not only should it clean up the fonts themselves
// but it should also make sure that the BFText system is cleaned up
// including the LL objects, the master index ptr & VBO, etc.
//========================================================================
void NvBFCleanup()
{
    if (s_bfInitialized)
    {
    // NvFree each font in the fontLL
        NvBitFont *bitfont = bitFontLL, *currFont;
        while (bitfont)
        {
            currFont = bitfont;
            bitfont = bitfont->m_next;
            delete currFont->m_render;
            currFont->m_render = NULL;

            // delete new AFont objects
            delete currFont->m_afont;
            delete currFont->m_afontBold;
            // free the font itself.
            delete currFont;
        }
        bitFontLL = NULL;
        bitFontID = 1;
    // !!!!TBD

        // for safety, we're going to clear _everything_ here to init's
        bitFontLL = NULL;
        bitFontID = 1; // NOT ZERO, start at 1 so zero is 'invalid'...
        dispW = 0;
        dispH = 0;
        dispAspect = 0;
        dispRotation = 0;
        lastTextMode = 111;
        //s_pixelToClipMatrix[4][4];
        s_pixelScaleFactorX = 2.0f / 640.0f;
        s_pixelScaleFactorY = 2.0f / 480.0f;
        m_matrixOverride = NULL;

        NvBitFontRenderFactory::GlobalShutdown();
    }
    
    s_bfInitialized = false;
}


//========================================================================
void NvBFSetScreenRes(float width, float height)
{
    dispW = width;
    dispH = height;
    dispAspect = dispW/dispH;

    s_pixelScaleFactorX = 2.0f / dispW;
    s_pixelScaleFactorY = 2.0f / dispH;
}

//========================================================================
void NvBFGetScreenRes(float *width, float *height)
{
    if (width)
        *width = dispW;
    if (height)
        *height = dispH;
}

//========================================================================
void NvBFSetScreenRot(float degrees)
{
    dispRotation = degrees;
}


//========================================================================
//========================================================================
//========================================================================

typedef uint32_t NvUTF32Char;

//========================================================================
// !!!!TBD if we want to handle GenBuffers failure, we should have a
// separate Init method.
//========================================================================
NvBFText::NvBFText()
: m_stringChars(0)
, m_stringMax(0)
, m_string(NULL)
, m_stringCharsOut(0)
, m_drawnChars(-1) // no clamping.

, m_data(NULL)
    
, m_numLines(0)
, m_calcLinesMax(0)
, m_calcLineChars(NULL)
, m_calcLineWidth(NULL)

, m_charColor(NV_PC_PREDEF_WHITE)

, m_cached(false)
, m_visible(true)
, m_fontNum(0)
, m_fontSize(10)
, m_font(NULL)
    
, m_hMode(NvBftAlign::LEFT)
, m_vMode(NvBftAlign::TOP)
, m_hPos(-1234)
, m_vPos(-1234)
, m_textLeft(-1234)
, m_textTop(-1234)

, m_boxWidth(0)
, m_boxHeight(0)
, m_boxLines(0)
    
, m_hasBox(false)
, m_doWrap(false)
, m_doScissor(false)
, m_posCached(false)
    
, m_truncChar(0)
    
, m_shadowDir(0)
, m_shadowColor(NV_PC_PREDEF_BLACK)
    
, m_outline(false)
, m_outlineColor(NV_PC_PREDEF_BLACK)

, m_pixelsWide(0)
, m_pixelsHigh(0)
, m_render(NvBitFontRenderFactory::TextRenderCreate())
{
}


//========================================================================
//========================================================================
NvBFText::~NvBFText()
{
    // then clean up and NvFree.
    if (m_calcLineChars)
        free(m_calcLineChars);
    m_calcLineChars = NULL;

    if (m_calcLineWidth)
        free(m_calcLineWidth);
    m_calcLineWidth = NULL;

    if (m_render) {
        delete m_render;
        m_render = 0;
    }

    if (m_string)
        free(m_string);
    m_string = NULL;

    if (m_data)
        free(m_data);
    m_data = NULL;
}


//========================================================================
//========================================================================
void NvBFText::SetVisibility(bool vis)
{
    m_visible = vis;    
    // visibility shouldn't affect caching!
}


//========================================================================
//========================================================================
void NvBFText::SetColor(NvPackedColor color)
{
    if (NV_PC_EQUAL(m_charColor, color))
        return; // no change occurred.
    m_charColor = color;
    // flag we need to recache this bftext
    m_cached = 0;
}


//========================================================================
//========================================================================
void NvBFText::SetShadow(char dir, NvPackedColor color)
{
    if (m_shadowDir==dir && NV_PC_EQUAL(m_shadowColor, color))
        return; // no change.
    m_shadowDir = dir;
    m_shadowColor = color;
    // flag we need to recache this bftext
    m_cached = 0;
}


//========================================================================
//========================================================================
void NvBFText::SetOutline(bool outline, NvPackedColor color)
{
    if (m_outline==outline && NV_PC_EQUAL(m_outlineColor, color))
        return; // no change.
    m_outline = outline;
    m_outlineColor = color;
    // flag we need to recache this bftext
    m_cached = 0;
}

//========================================================================
//========================================================================
void NvBFText::SetDrawnChars(int32_t num)
{
    m_drawnChars = num;    
//    m_cached = 1; // flag that we cached this.
}


//========================================================================
// the cases here...
// lines is 0 for single line, no cropping.
// lines is 1 for single line, cropped.  dots non-zero to append when crop.
// lines is >1 for multiline, space wrapped, cropped on final line only.
//========================================================================
void NvBFText::SetBox(float width, float height, int32_t lines, uint32_t dots)
{   
    if (m_hasBox
    &&  m_boxWidth == width
    &&  m_boxHeight == height
    &&  m_boxLines == lines
    &&  m_truncChar == dots)
        return; // no changes.
    
    m_doWrap = 0;
    m_truncChar = 0;
    if (width==0 || height==0) // invalid, clear the box.
    {
        m_hasBox = 0;
        m_boxWidth = 0;
        m_boxHeight = 0;
        m_boxLines = 0;
    }
    else
    {
        m_hasBox = 1;
        m_boxWidth = width;
        m_boxHeight = height;
        m_boxLines = lines;

        if (lines!=1) // don't wrap if explicitly one line only.
            m_doWrap = 1;
        
        if (0!=dots)
            m_truncChar = dots;       
    }
        
    // flag we need to recache this bftext
    m_cached = 0;
}


//========================================================================
// this is ONLY called to UPDATE an existing box.
// I have it check hasBox to ensure that's the case!
//========================================================================
void NvBFText::UpdateBox(float width, float height)
{
    if (m_hasBox)
    {
        if (m_boxWidth != width
        ||  m_boxHeight != height)
        {
            m_boxWidth = width;
            m_boxHeight = height;
            // flag we need to recache this bftext
            m_cached = 0;
        }
    }
}


//========================================================================
//========================================================================
void NvBFText::SetFont(uint8_t fontnum)
{
    if (fontnum == m_fontNum) // same, we're done before we've even started.
        return;

    // flag we need to recache this bftext
    m_cached = 0;

    // just cache the values.  we'll use them in the recache function.
    if (fontnum==0) // oops...
    {
        m_fontNum = 0; // tag this.
        m_font = NULL;
    }
    else
        m_font = BitFontFromID(fontnum); // find our font and hold onto the ptr, for ease of coding.    
    if (m_font!=NULL)
           m_fontNum = fontnum;
    else // handle the case that font wasn't loaded...
    {
        // !!!!TBD ASSERT????
        m_font = bitFontLL; // whatever the first font is.
        if (m_font)
            m_fontNum = m_font->m_id;
        else
            m_fontNum = 0; // try and have 0 to test later!!!
    }
       
    if (m_fontNum==0) // something went wrong.
    {
        // what can we do? !!!!TBD
        m_cached = 1; // so we try to do no more work!
    }   
}


//========================================================================
//========================================================================
void NvBFText::SetSize(float size)
{
    if (size == m_fontSize) // same, we're done before we've even started.
        return;

    // flag we need to recache this bftext
    m_cached = 0;
    m_fontSize = size;
}


//========================================================================
//========================================================================
void NvBFText::SetString(const char *str)
{
    if (str==NULL)
    {
        DEBUG_LOG("bitfont sent a null string ptr...");
//    if (m_string==NULL)
//        DEBUG_LOG("bitfont has no string allocated yet...");
        if (m_stringMax) // allocated
            m_string[0] = 0;
        m_stringChars = 0;
    }
    
    if (m_stringMax) // allocated
        if (0==strcmp(m_string, str)) // same, we're done before we've even started.
            return;

    // flag we need to recache this bftext
    m_cached = 0;
    // clear other computed values.
    m_pixelsWide = 0;
    m_pixelsHigh = 0;

    // check that we have storage enough for the string, and for calc'd data
    // then, check that we have enough space in our bftext-local storage...
    // !!!!TBD Getuint8_tLength isn't definitively what I want, I don't think... might be multi-word chars in there.
    m_stringChars = int32_t(strlen(str));
    int32_t charsToAlloc = 2*(m_stringChars+1); // doubled for shadow text... !!!!TBD remove if we do shadow in shader.
    if (charsToAlloc > m_stringMax-1) // need to account for null termination and for doubled shadow text
    {
        if (m_stringMax) // allocated, NvFree structs.
        {
            free(m_string);
            free(m_data);
        }
        // reset max to base chars padded to 16 boundary, PLUS another 16 (8+8) for minor growth.
        m_stringMax = charsToAlloc + 16-((charsToAlloc)%16) + 16;
        m_string = (char*)malloc(m_stringMax*sizeof(char)); // !!!!TBD should use TCHAR size here??
        memset(m_string, 0, m_stringMax*sizeof(char));
        m_data = (BFVert*)malloc(m_stringMax*sizeof(BFVert)*VERT_PER_QUAD);
    }

    memcpy(m_string, str, m_stringChars+1); // include the null.
}

//========================================================================
//========================================================================
void NvBFText::AdjustGlyphsForAlignment()
{
    if (m_hMode==NvBftAlign::LEFT)
        return; // nothing to do.

    DEBUG_LOG("Adjusting glyphs for alignment...");
    const bool center = (m_hMode==NvBftAlign::CENTER);
    BFVert *vp = m_data;
    // loop over the lines in this bftext...
    for (int32_t i=0; i<m_numLines; i++)
    {
        const int32_t max = m_calcLineChars[i];
        DEBUG_LOG("line %d, char count %d.", i, max);
        float w = m_calcLineWidth[i];
        if (center)
            w *= 0.5f;
        // loop through chars on this line, shifting X coords
        // note char count covers shadow glyphs properly.
        for (int32_t j=0; j<max; j++)
        {
            for (int32_t k=0; k<4; k++)
            {
                vp->pos[0] -= w; // shift back by half or full linewidth.
                vp++; // next vertex;
            }
        }
    }
}



static inline void AddGlyphVertex(BFVert **vp, // explicit we're modding
                            float x, float y,
                            float uvx, float uvy,
                            NvPackedColor color )
{
    (*vp)->pos[0] = x;
    (*vp)->pos[1] = y;
    (*vp)->uv[0] = uvx;
    (*vp)->uv[1] = uvy;
    (*vp)->color = NV_PC_PACK_UINT(color);
    (*vp)++;
}

//========================================================================
//========================================================================
static void AddOutputGlyph( const AFontChar &fc, const AFont *afont,
                            bool outline,
                            BFVert **vp, // so explicit we're modding it!
                            float *left, float t, float b,
                            float hsizepertex,
                            NvPackedColor color )
{
//    AFont::afontGlyphMap::iterator it;
//    it = afont->m_glyphs.find(indexChar);
//    if (afont->m_glyphs.end() == it) // bad lookup
//    {
//        LOGE("Missing font char '%c' index 0x%x\n", (char)indexChar, indexChar);
//        return;
//    }
    float pX = *left + (fc.m_xOff * hsizepertex);
    float pY = t + (fc.m_yOff * hsizepertex);
    // adjust for baseline and a bit of lineheight, since we're positioning top-corner, NOT baseline...
    pY -= afont->m_charCommon.m_baseline * hsizepertex;
    pY += (afont->m_charCommon.m_lineHeight - afont->m_charCommon.m_baseline) * 0.3f * hsizepertex;
    float pH = fc.m_height * hsizepertex;
    float pW = fc.m_width * hsizepertex;
    *left += (fc.m_xAdvance * hsizepertex);
//    *left += ((fc.m_xAdvance + afont->m_fontInfo.m_outline) * hsizepertex);
    // must invert Y on uv as we flipped texture coming in.
    const float invW = afont->m_charCommon.m_pageWidthInv;
    const float invH = afont->m_charCommon.m_pageHeightInv;
    float tx = fc.m_x * invW ;
    float ty = fc.m_y * invH;
    float tw = fc.m_width * invW;
    float th = fc.m_height * invH;

    if (outline)
    {
        // trying a pixel expansion of src rect for outline.
        tx -= invW;
        ty -= invH;
        tw += 2*invW;
        th += 2*invH;
        pW += 2*hsizepertex;
        pH += 2*hsizepertex;
    }

    float uvt = ty;
    float uvb = ty+th;

    AddGlyphVertex(vp, pX, pY, tx, uvt, color);
    AddGlyphVertex(vp, pX , pY + pH, tx, uvb, color);
    AddGlyphVertex(vp, pX + pW, pY + pH, tx+tw, uvb, color);
    AddGlyphVertex(vp, pX + pW, pY, tx+tw, uvt, color);

//    LOGI("BF> '%c' @ [%0.2f,%0.2f,%0.2f,%0.2f] : [%0.2f,%0.2f,%0.2f,%0.2f]\n",
//        vc, pX, pY, pX + g.pix.width, pY + g.pix.height,
//                g.norm.u, g.norm.v, g.norm.u+g.norm.width, g.norm.v+g.norm.height);
}


//========================================================================
//========================================================================
void NvBFText::TrackOutputLines(float lineWidth)
{
    const int32_t charCount = m_stringCharsOut; // it's in bf, no need to pass.
    
    // then we want to track lines so we can align
    if (m_calcLinesMax==0)
    {
        m_calcLinesMax = 8; // something that seems decent for single lines
                            // or small boxes, we'll grow later.
        m_calcLineChars = (int32_t *)malloc(sizeof(int32_t) * m_calcLinesMax);
        if (m_calcLineChars==NULL)
            return; // TODO FIXME
        m_calcLineWidth = (float *)malloc(sizeof(float) * m_calcLinesMax);
        if (m_calcLineWidth==NULL)
            return; // TODO FIXME
    }
    
    if (m_numLines > m_calcLinesMax)
    { // then resizing.
        int32_t newMax = (m_calcLinesMax*3)/2; // add 50% of current.  reasonable.
        int32_t *newLineChars = (int32_t *)malloc(sizeof(int32_t) * newMax);
        if (newLineChars==NULL)
            return; // TODO FIXME
        float *newLineWidth = (float *)malloc(sizeof(float) * newMax);
        if (newLineWidth==NULL)
            return; // TODO FIXME
        // copy the line data.
        for (int32_t i=0; i < m_calcLinesMax; i++)
        {
            newLineChars[i] = m_calcLineChars[i];
            newLineWidth[i] = m_calcLineWidth[i];
        }
        // free old data
        free(m_calcLineChars);
        free(m_calcLineWidth);
        // swap in new data.
        m_calcLineChars = newLineChars;
        m_calcLineWidth = newLineWidth;
        m_calcLinesMax = newMax;
    }
    
    // NOW, we actually get around to the tracking. :)
    int32_t lineNum = 0;
    int32_t prevChars = 0;
    if (m_numLines>0)
        lineNum = m_numLines - 1;
    for (int32_t i=0; i<lineNum; i++)
        prevChars += m_calcLineChars[i];
    m_calcLineChars[lineNum] = charCount-prevChars;
    m_calcLineWidth[lineNum] = lineWidth;
}

//========================================================================
// this function rebuilds the VBO/rendercache based on a simplistic
// ENGLISH char-walk of the string.
// !!!!TBD handle the actual unicode chars we might get properly
// !!!!TBD handle complex script layouts and break rules of non-roman lang
//========================================================================
void NvBFText::RebuildCache(bool internalCall)
{
    NvBftStyle::Enum bfs = NvBftStyle::NORMAL;

    float vsize, hsizepertex, fullglyphwidth;
    float left, t, b;
    int32_t n;
    BFVert *vp, *lastvp;
    const NvBitFont *bitfont = m_font;
    NvPackedColor color;
    int32_t linesign = 1;
    int32_t lineheightdelta = 0; // !!!!TBD
    int32_t lastlinestart = 0, lastwhitespacein = 0, lastwhitespaceout = 0; // so we can reset positions for wrappable text.
    float lastwhitespaceleft = 0;
    uint32_t realcharindex;
    float extrawrapmargin = 0;
    AFont::afontGlyphMap::iterator glyphit, truncit;
    AFont *currFont;

    if (m_cached) // then no work to do here, move along.
        return;
    if (!m_fontNum)
        return;
    if (!bitfont)
        return;

    // start with normal style
    currFont = bitfont->m_afont;

    // recalc size in terms of the screen res...
    //j = m_fontNum-1; // fontnum is 1-based, other stuff is 0-based
    vsize = m_fontSize;// *(high/((dispAspect<1)?640.0f:480.0f)); // need the texel-factor for the texture at the end...
    hsizepertex = vsize / bitfont->m_canonPtSize;

    // calc extra margin for wraps...
    if (m_hasBox && m_truncChar)
    {
        // calculate the approx truncChar size needed.  Note we don't have
        // style info at this point, so this could be off by a bunch.  !!!!TBD FIXME
        extrawrapmargin = 0;
        truncit = currFont->m_glyphs.find(m_truncChar);
        if (currFont->m_glyphs.end() != truncit) // found it.
            extrawrapmargin = truncit->second.m_xAdvance;
        extrawrapmargin *= 3; // for ...
    }
   
    // loop over the string, building up the output chars...
    left = 0;
    float maxWidth = 0;
    t = currFont->m_charCommon.m_baseline * hsizepertex;
    b = t + (linesign * vsize);
    vp = m_data;
    color = m_charColor; // default to set color;
    m_stringCharsOut = 0;
    
    lastvp = vp;
    lastlinestart = 0;
    lastwhitespacein = 0;
    lastwhitespaceout = 0;
    n=0;
    m_numLines = 1;
    while (n<m_stringChars)
    {
        // !!!!TBD THIS ISN'T UNICODE-READY!!!!
        realcharindex = (uint32_t)(m_string[n]); // need to cast unsigned for accented chars...        
        if (realcharindex==0) // null.  done.
            break;

        if ((realcharindex=='\n') //==0x0A == linefeed.
        ||  (realcharindex=='\r')) //==0x0D == return.
        {
            if (m_hasBox && (m_boxLines > 0) &&
                    ((m_numLines + 1) > m_boxLines)) 
                break; // exceeded line cap, break from cache-chars loop.
            n++;
            TrackOutputLines(left);
            lastlinestart = n; // where we broke and restarted.
            lastwhitespacein = n; // so we can rollback input position..
            lastwhitespaceout = m_stringCharsOut; // so we can reset output position.
            lastvp = vp; // so we can reset output position.
            m_numLines++; // count lines!

            t = b + lineheightdelta; // move to next line.
            b = t + (linesign * vsize); // proper calc for bottom.
            left = 0; // reset to left edge.

            lastwhitespaceleft = 0; // on return, reset

            continue; // loop again.
        }

        // !!!!!TBD handling of unicode/multibyte at some point.
        if (realcharindex < 0x20) // embedded commands under 0x20, color table code under 0x10...
        {
            // first check any chars we want excluded!
            if (realcharindex=='\t')
            {
                // do nothing here.  fall through, forward into processing.
            }
            else
            {
                if (realcharindex < 0x10) // color table index
                { // colorcodes are 1-based, table indices are 0-based
                    if (NvBF_COLORCODE_MAX == realcharindex-1)
                        color = m_charColor; // default to set color;
                    else
                        color = s_charColorTable[realcharindex-1];
                }
                else // escape codes
                {
                    if (realcharindex < NvBftStyle::MAX)
                        bfs = (NvBftStyle::Enum)realcharindex;
                    if (bfs > NvBftStyle::NORMAL && bitfont->m_afontBold)
                        currFont = bitfont->m_afontBold;
                    else
                        currFont = bitfont->m_afont;
                }
                n++; // now proceed to next char.
                continue; // loop again.
            }
        }

        // precalc the full glyph spacing, to optimize some of this processing.
        fullglyphwidth = 0;
        glyphit = currFont->m_glyphs.find(realcharindex);
        if (currFont->m_glyphs.end() != glyphit) // found it.
            fullglyphwidth = glyphit->second.m_xAdvance; // !!!!TBD TODO is this right???

        if (realcharindex==' ' || realcharindex=='\t') // hmmm, optimization to skip space/tab characters, since we encode the 'space' into the position.
        {
            // then, update the offset based on the total ABC width (which for a monospaced font should == fontCell size...)
            lastwhitespaceleft = left;
            left += fullglyphwidth;
            n++; // now proceed to next char.
            if (lastwhitespacein!=n-1) // then cache state
            {
                lastwhitespacein = n; // so we can rollback input position..
                lastwhitespaceout = m_stringCharsOut; // so we can reset output position.
                lastvp = vp; // so we can reset output position.
            }
            // one more check+update
            if (lastwhitespacein==lastlinestart+1) { // was first char of our new line, reset linestart num
                lastlinestart = n;
            }
            continue; // loop again.
        }
        
        // !!!!!TBD linewrap... should actually look ahead for space/lf, OR
        // needs to do a 'roll back' when it hits edge... but for extra long strings,
        // we need to realize we don't have a whitespace character we can pop back to,
        // and need to just immed character wrap.
        
        // check to see if we'd go off the 'right' edge (with spacing...)
        if ( m_hasBox && ((left + fullglyphwidth) > (m_boxWidth - extrawrapmargin)) )
        {
            if (!m_doWrap) // then character truncate!
            {
            }
            else // word wrapping, jump back IF it's sane to.
            if (lastwhitespacein!=lastlinestart)
            {
                n = lastwhitespacein; // go back some chars.
                m_stringCharsOut = lastwhitespaceout; // undo output buffering.
                vp = lastvp; // undo output buffering.
                left = lastwhitespaceleft; // undo word positioning.
            }
            else
            {
                // !!!!TBD some update to outputs here.
                // not resetting n, sco, or vp... keep going forward from HERE.
            }
            
            TrackOutputLines(left);
            lastlinestart = n; // where we broke and restarted.
            lastwhitespacein = n; // so we can rollback input position..
            lastwhitespaceout = m_stringCharsOut; // so we can reset output position.
            lastvp = vp; // so we can reset output position.
            m_numLines++; // count lines!

            // !!!!TBD truncChar handling...
            // !!!!TBD how to insert truncChar's into output stream.  need sub-function to add char.
            
            if ((m_boxLines > 0) && (m_numLines > m_boxLines)) // FIXME lines+1????
            {
                if (m_truncChar)
                {
                    int32_t i;
                    if (m_doWrap) // if wrapping, shift to ... position.
                        left = lastwhitespaceleft;
                    if (currFont->m_glyphs.end() != truncit) // found it.
                        for (i=0; i<3; i++) // for ellipses style
                        {
                            if (m_shadowDir)
                            {
                                float soff = ((float)m_shadowDir) * s_bfShadowMultiplier;
                                float tmpleft = left+soff; // so we don't really change position.
                                AddOutputGlyph( truncit->second, currFont, m_outline, &vp, &tmpleft, t+soff, b+soff, hsizepertex, m_shadowColor );
                                m_stringCharsOut++; // update number of output chars.
                            }        
                            AddOutputGlyph( truncit->second, currFont, m_outline, &vp, &left, t, b, hsizepertex, color );
                            m_stringCharsOut++; // update number of output chars.
                        }
                    
                    // update char count and line width since we're going to break out.
                    TrackOutputLines(left);
                }
                break; // out of the output loop, we're done.
            }
           
            // if doing another line, reset variables.
            t = b + lineheightdelta; // move to next line.
            b = t + (linesign * vsize); // proper calc for bottom.
            if (maxWidth < left)
                maxWidth = left;
            left = 0; // reset to left edge.
            lastwhitespaceleft = 0; // on return, reset
            
            continue; // restart this based on new value of n!
        }

        if (currFont->m_glyphs.end() != glyphit) // found the char above
        {
            if (m_shadowDir)
            {
                float soff = ((float)m_shadowDir) * s_bfShadowMultiplier;
                float tmpleft = left+soff; // so we don't really change position.
                AddOutputGlyph( glyphit->second, currFont, m_outline, &vp, &tmpleft, t+soff, b+soff, hsizepertex, m_shadowColor );
                m_stringCharsOut++; // update number of output chars.
            }        
            AddOutputGlyph( glyphit->second, currFont, m_outline, &vp, &left, t, b, hsizepertex, color );
            m_stringCharsOut++; // update number of output chars.
        }

        // now proceed to next char.
        n++; 
    }

    TrackOutputLines(left);

    for (int32_t i=0; i<m_numLines; i++)
        if (maxWidth < m_calcLineWidth[i])
            maxWidth = m_calcLineWidth[i];

    // if alignment is not left, we shift each line based on linewidth.
    AdjustGlyphsForAlignment();
    
    //DEBUG_LOG(">> output glyph count = %d, stringMax = %d.", m_stringCharsOut, m_stringMax);
    m_render->UpdateText(m_stringCharsOut, m_data, internalCall);

    m_pixelsWide = maxWidth; // cache the total width in output pixels, for justification and such.
    m_pixelsHigh = vsize * m_numLines;
    m_cached = 1; // flag that we cached this.
    m_posCached = 0; // flag that position needs recache.  FIXME could optimize...
}


//========================================================================
float NvBFText::GetWidth()
{
    return(m_pixelsWide);
}


//========================================================================
float NvBFText::GetHeight()
{
    return(m_pixelsHigh);
}

//========================================================================
void NvBFText::RenderPrep()
{
    if (m_render)
        m_render->RenderPrep();
}

//========================================================================
void NvBFText::RenderDone()
{
    if (m_render)
        m_render->RenderDone();
}


//========================================================================
// 0==top/left, 1==bottom/right, 2==center/center
//========================================================================
void NvBFText::SetCursorAlign(NvBftAlign::Enum h, NvBftAlign::Enum v)
{
    if (m_vMode!=v || m_hMode!=h) // reset
    {
        m_vMode = v;
        m_hMode = h;
        m_posCached = false; // FIXME for now, force recalc.
    }
}

//========================================================================
// in default mode, top/left offset.
//========================================================================
void NvBFText::SetCursorPos(float h, float v)
{
    if (m_hPos!=h || m_vPos!=v)
    {
        m_hPos = h;
        m_vPos = v;    
        m_posCached = false; // we need to adjust
//    m_hModeVBO = -1;
    }
}


//========================================================================
//========================================================================
void NvBFText::SetMatrix(const float *mtx)
{
    m_matrixOverride = mtx;
}


//========================================================================
//========================================================================
void NvBFText::UpdateTextPosition()
{
    if (m_posCached) return;
    
    m_textLeft = m_hPos;
    m_textTop = m_vPos;
    
    // horizontal adjustments
    if (m_hasBox)
    {
        if (m_hMode==NvBftAlign::CENTER)
            m_textLeft += (m_boxWidth*0.5f); // align box to point, center content within.
        else if (m_hMode==NvBftAlign::RIGHT)
            m_textLeft += m_boxWidth; // align to right of positioned box.
    }

    // vertical adjustments
    if (m_vMode==NvBftAlign::CENTER)
    {
        if (m_hasBox)
            m_textTop += (m_boxHeight*0.5f); // shift down half box size to center
        m_textTop -= (m_pixelsHigh*0.5f); // shift up half text height to center
    }
    else if (m_vMode==NvBftAlign::BOTTOM)
    {
        if (m_hasBox)
            m_textTop += m_boxHeight; // shift down by box height
        m_textTop -= m_pixelsHigh; // shift up by text height
    }
    
    m_posCached = 1;
}


//========================================================================
//========================================================================
void NvBFText::Render()
{
    void *p = 0;
    int32_t count = m_drawnChars;

    if (!m_visible || !m_fontNum || !m_font) // nothing we should output...
        return;

    if (count<0)
        count = m_stringChars; // !!!TBD maybe negative means something else.
    else
        if (count>m_stringChars)
            count = m_stringChars;
    if (count == 0)
        return; // done...
    if (m_shadowDir)
        count *= 2; // so we draw char+shadow equally...

    // since buffer state is now set, can rebuild cache now without extra calls.
    if (!m_cached) // need to recache BEFORE we do anything using textwidth, etc.
        RebuildCache(1);
    if (count > m_stringCharsOut) // recheck count against CharsOut after rebuilding cache
        count = m_stringCharsOut;
    if (!m_posCached) // AFTER we may have rebuilt the cache, we check if we recalc pos.
        UpdateTextPosition();

    // set the model matrix offset for rendering this text based on position & alignment
    // first, do any pre-render position updates

    // we apply any global screen orientation/rotation so long as
    // caller hasn't specified their own transform matrix.

    const float* matrix = m_matrixOverride;
    if (!matrix) {
        const float wNorm = s_pixelScaleFactorX;
        const float hNorm = s_pixelScaleFactorY;
        if (dispRotation == 0)
        { // special case no rotation to be as fast as possible...
            s_pixelToClipMatrix[0][0] = wNorm;
            s_pixelToClipMatrix[1][0] = 0;
            s_pixelToClipMatrix[0][1] = 0;
            s_pixelToClipMatrix[1][1] = -hNorm;

            s_pixelToClipMatrix[3][0] = (wNorm * m_textLeft) - 1;
            s_pixelToClipMatrix[3][1] = 1 - (hNorm * m_textTop);
        }
        else
        {
            float rad = (float)(3.14159f / 180.0f);  // deg->rad
            float cosfv;
            float sinfv;

            rad = (dispRotation * rad); // [-1,2]=>[-90,180] in radians...
            cosfv = (float)cos(rad);
            sinfv = (float)sin(rad);

            s_pixelToClipMatrix[0][0] = wNorm * cosfv;
            s_pixelToClipMatrix[1][0] = hNorm * sinfv;
            s_pixelToClipMatrix[0][1] = wNorm * sinfv;
            s_pixelToClipMatrix[1][1] = hNorm * -cosfv;

            s_pixelToClipMatrix[3][0] = (s_pixelToClipMatrix[0][0] * m_textLeft)
                - cosfv - sinfv
                + (s_pixelToClipMatrix[1][0] * m_textTop);
            s_pixelToClipMatrix[3][1] = (s_pixelToClipMatrix[0][1] * m_textLeft)
                - sinfv + cosfv
                + (s_pixelToClipMatrix[1][1] * m_textTop);
        }

        matrix = &(s_pixelToClipMatrix[0][0]);
    }

    m_render->Render(matrix, m_outlineColor, m_font, m_outline, count);
}
