//----------------------------------------------------------------------------------
// File:        NvUI/NvBitFont.h
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

#ifndef _nvbitfont_h_
#define _nvbitfont_h_

#include <NvSimpleTypes.h>

#include <NvUI/NvPackedColor.h>


/** @file NvBitFont.h
    @brief An OpenGL bitmap-font text rendering library
    Library for rendering text strings under Open GL and GL ES, using bitmap fonts alongside
    xml files with glyph data annotations.  Fonts can be made using fairly-standard 'angelcode'
    output-format bitmap font rasterizers.

    BitFont is able to:
    <ul>
        <li> Load multiple bitmap fonts simultaneously, from compressed/DDS files, with or without mipmaps </li>
        <li> Handle extended ASCII character codes </li>
        <li> Manage multi-style fonts, thus able to combine normal and "bold" styling in a single string. </li>
        <li> Render text aligned to screen edges or any sub-rect 'box' on the screen </li>
        <li> Support multi-line output, including auto-wrapping </li>
        <li> Apply a specific font, size, base color, and multiplied color to each string </li>
        <li> Optionally apply drop-shadowing to a text run </li>
        <li> Optionally embed special escape codes for on-the-fly color or style changes </li>
        <li> Given screen size (and rotation), can automatically re-scale and rotate text output </li>
        <li> Allow overriding the default matrix calc to apply custom transformations to text </li>
    </ul>
 */


/** Alignment modes for text output.

    BitFont supports standard 'cell' alignment, either to the screen bounds
    or to a specified 'box'.  Note that the center-alignment enumerant is used
    for both horizontal and vertical alignments.

    Note: for coexisting with NvUI, these values duplicate those in NvUITextAlign.
 */
struct NvBftAlign {
    enum Enum {
        LEFT=0, /**< [Default] Left aligned */
        RIGHT=1, /**< Right aligned */
        CENTER=2, /**< Center aligned */

        TOP=0, /**< [Default] Top aligned */
        BOTTOM=1 /**< Bottom aligned */
    };
};

/** BitFont supports 'styles' via embedded character codes and string literals.

    You can directly use the following as character values for switching
    between the top and bottom 'halves' of a 'split' font files.  This is
    currently used as 'normal' and 'bold' styles, but the two halves could
    actually be set up as different typeface families.  There are also matching
    string literals for equivalent use directly in C quoted string composition.
    @see NVBF_STYLESTR_NORMAL
    @see NVBF_STYLESTR_BOLD
 */
struct NvBftStyle {
    enum Enum {
        NORMAL = 0x10, /**< Sets further text to normal style. */
        BOLD, /**< Sets further text to bold style, for fonts loaded with bold support. */
//      ITALIC, // !!!!TBD NYI
//      BOLDITALIC, // !!!!TBD NYI
        MAX  /**< Used for programmatic range checking of embedded codes. */
    };
};

/** @name Text Style String Literals
*/
/* @{ */
/** BitFont string literal for style reset to 'normal'. */
#define NVBF_STYLESTR_NORMAL     "\020"
/** BitFont string literal to style further characters 'bold'. */
#define NVBF_STYLESTR_BOLD       "\021"
//#define NvBFSTYLEITALIC     "\022"
//#define NvBFSTYLEBOLDITALIC "\023"
/** @see bftStyle */
/* @} */


/** BitFont supports specific color constants embedded in a string.

    You can directly use the following as character values for switching
    color 'runs' in the text.  Note that these embedded color changes will
    completely override the base color specified for a given string.

    There are also string literals for use directly in C quoted string composition.
    @see NvBF_COLORSTR_WHITE
    @see NvBF_COLORSTR_BLACK
 */
typedef enum
{
    NvBF_COLORCODE_WHITE = 0, /**< Sets further text to be white. */
    NvBF_COLORCODE_GRAY, /**< Sets further text to be gray. */
    NvBF_COLORCODE_BLACK, /**< Sets further text to be black. */
    NvBF_COLORCODE_RED, /**< Sets further text to be red. */
    NvBF_COLORCODE_GREEN, /**< Sets further text to be green. */
    NvBF_COLORCODE_BLUE, /**< Sets further text to be blue. */
    NvBF_COLORCODE_MAX /**< Used for programmatic range checking of embedded codes. */
} NvBftColorCode;

/** @name Text Color String Literals
*/
/* @{ */
/** Embedded string literal to change text coloring to white. */
#define NvBF_COLORSTR_WHITE     "\001"
/** Embedded string literal to change text coloring to gray. */
#define NvBF_COLORSTR_GRAY      "\002"
/** Embedded string literal to change text coloring to black. */
#define NvBF_COLORSTR_BLACK     "\003"
/** Embedded string literal to change text coloring to red. */
#define NvBF_COLORSTR_RED       "\004"
/** Embedded string literal to change text coloring to green. */
#define NvBF_COLORSTR_GREEN     "\005"
/** Embedded string literal to change text coloring to blue. */
#define NvBF_COLORSTR_BLUE      "\006"
/** Embedded string literal to restore text coloring to 'normal'. should be 'max' value. */
#define NvBF_COLORSTR_NORMAL    "\007"
/* @} */

// forward delcare private class so we can hold pointer to it.
class NvBitFont;
// forward declare private struct so we can hold pointer to data block
struct BFVert;

/** @name BitFont System Creation & Global Property Accessors

    The high level functions for initializing and preparing the global
    state of the BitFont system on a per-app basis.
*/
/* @{ */

/** Base initialization of the font system, once per application.

    Initialize the BitFont system with one or more on-disk fonts.

    @param count total fonts to load
    @param filename array of two char* .fnt font descriptor files.  In case bold style is supported, second is the bold .fnt variant -- note that the bold.fnt file MUST refer to the same texture/bitmap files as the normal/base did (we only support when bold is embedded in same texture).
    @return zero if initialized fine, one if failed anywhere during init process.
 */
int32_t NvBFInitialize(uint8_t count, const char* filename[][2]);
/** Clean up all BitFont resources. */
void NvBFCleanup(void);

/** Get the font ID for a named font file.

    Look up the matching font ID for a given font filename previously loaded.
    This is then used as argument to SetFont.  It gives us the ability
    to abstract the order of loaded fonts from the ID/index used to reference
    them later -- most useful when multiple, distinct systems in an app are
    talking to BitFont, and separately loading some same, some different fonts.

    @param filename one of the filenames used in previous NvBFInitialize call.
    @return byte (bfuchar) ID/index that will refer to that font until app exit.
 */
uint8_t NvBFGetFontID(const char *filename);

/** Set the destination size/scale to factor coords into. */
void NvBFSetScreenRes(float width, float height);
/** Get current destination size/scale for safe save/restore. */
void NvBFGetScreenRes(float *width, float *height);
/** Set the rotation/orientation of text output (in 90-degree increments, preferably). */
void NvBFSetScreenRot(float degrees);

/** Set automatic save/restore of GL state when rendering text.  Defaults false as it's heavyweight.*/
void NvBFSetSaveRestoreState(bool enable);

/* @} */

class NvBFTextRender;

class NvBFText
{
public:
    
    /** @name Creation & Property Accessors

        BFText 'objects' are an abstraction of a 'text run' or drawable string, and
        contain all information and buffers necessary to quickly draw the text
        using GLES primitives.
    */
    /* @{ */

    NvBFText();
    virtual ~NvBFText();

    /** Set whether a given bftext is visible or invisible. */
    void SetVisibility(bool vis);

    /** Set the RGBA color (using an @ref NvPackedColor) for a line of text.
        Since colors are per-character (stored in vertex data), changing the global text color
        requires a rebuild of the string buffers.
        NOTE: Embedded color literals/escapes inside the string data itself will take precedence
        over this string-global color value.
    */
    void SetColor(NvPackedColor color);

    /** Set the drop-shadow for a bftext.

        Activates a drop-shadow effect on given bftext output.

        @param offset the +/- offset of the shadow from the base, in 'texels' (not pixels)
        @param color the shadow color as an @ref NvPackedColor
     */
    void SetShadow(char offset, NvPackedColor color); // !!!!TBD would be more useful in pixel space

    /** Set the outline mode for a bftext.

        Enables/disable an outline effect on given bftext output.

        @param outline true to enable, false to disable.
        @param color the outline color as an @ref NvPackedColor
     */
    void SetOutline(bool outline, NvPackedColor color);

    /** Draw less than the full string.

        Switches bftext to draw only first @p num characters in a bftext string.
        Most useful for helping do simple 'type on' effects.
        Note that positioning on the screen is where the chars would be if
        entire text was rendered -- thus left and top alignment are best.
     */
    void SetDrawnChars(int32_t num);

    /** Use a subrect of the screen for this bftext's raster operations.

        Sets a subrect in the given screen size, in which processes such as alignment,
        multi-line wrapping, will occur.

        @param width left/right alignment 'edges' (and wrap/clip boundary).
        @param height top/bottom alignment 'edges'.
        @param lines if greater than zero, specifies width is used to wrap text, and how many lines to wrap.
        @param dots if greater than zero, specifies a character to repeat three times when needs wrap but out of lines.
     */
    void SetBox(float width, float height, int32_t lines, uint32_t dots);

    /** Helper to quickly update width and height of previously-set text box. */
    void UpdateBox(float width, float height);

    /** Select font 'face' to use for this bftext (font ID from NvBFGetFontID). */
    void SetFont(uint8_t font);

    /** Set the output size of text in destination-space pixels, dependent on input font size/scale. */
    void SetSize(float size);

    /** Set the text string for a given bftext. */
    void SetString(const char* str);

    /** Get the last calculated output width of the bftext string. */
    float GetWidth();

    /** Get the last calculated output height of the bftext. */
    float GetHeight();

    //============================================================================
    // positioning the text output
    //============================================================================

    /** Set the bftext horizontal and vertical alignment.

        Sets the alignment 'edges' for render offset calculation and direction.
        @see NvBftAlign
     */
    void SetCursorAlign(NvBftAlign::Enum horizontal, NvBftAlign::Enum vertical);

    /** Set the bftext starting render position.

        Sets an inset/offset from the horizontal and vertical 'edges', as selected via
        @ref CursorAlign, in possible combination with @ref SetBox.
        That is, the borders to offset from are "screen edges", unless @ref SetBox
        was called in which case that box/rect is used to determine the 'edges' instead
        to inset from.
     */
    void SetCursorPos(float horiz, float vert);

    /* @} */


    //============================================================================
    // bitfont rendering functions
    //============================================================================
    /** @name Rasterization Functions
    */
    /* @{ */

    //============================================================================
    /** Prepare to render some bftext objects.

        This sets up necessary bits of the BitFont raster system.
        From this until calling @ref NvBFRenderDone, do no other GL operations.
        It can/should be called once before rendering one or more bftexts, for instance
        looping over an array of text elements on screen.
     */
    void RenderPrep(void);

    /** Conclude rendering of bftexts, making it safe to do other GLES calls. */
    void RenderDone(void);

    /** Set a specific transformation matrix to be used for rendering all text strings,
        until the next call to @ref RenderDone. */
    void SetMatrix(const float *mtx);

    //============================================================================
    /** The main call to actually render a bftext using stored properties. */
    void Render();

    //============================================================================
    /** Force a rebuilding of the cached vertex data for this bftext.

        This function recalculates all cached data related to a given bftext object,
        including vertex data for optimized rendering.  It does NOT normally need
        to be called by the programmer, but is useful in the specific case where you
        have a number of completely static strings, in which case calling this during
        the initialization process can allow the system to preallocate necessary
        structures and not need to recalc/allocate further at runtime.
    
        Many BFText state-changing functions will flag that a given string requires
        its vertex data be rebuilt -- but the system waits until rendering is requested
        so that it only recomputes the cached data once, regardless of the number of
        states changed for that text since the previous render or rebuild call.

        @param internal should be set to false, unless being called by an internal
        BitFont function which has already established the VBOs for this bftext.
     */
    void RebuildCache(bool internal);

    /* @} */

private:
    void AdjustGlyphsForAlignment();
    void TrackOutputLines(float lineWidth);
    void UpdateTextPosition();

protected:
    int32_t m_stringChars;
    int32_t m_stringMax;
    char *m_string;  // !! ascii for now
    // !!!!TBD not sure how much these features will be allowed in unicode... hmmm...
    int32_t m_stringCharsOut; // since string can have escape codes, need a sep count of REAL to output.
    int32_t m_drawnChars; // allowing 'clamping' the number of chars to actually draw.

    BFVert *m_data;
    
    int32_t m_numLines;
    int32_t m_calcLinesMax; // size of buffers allocated.
    int32_t *m_calcLineChars; // output char count per line
    float *m_calcLineWidth; // line width

    NvPackedColor m_charColor; // base color.  set in vertices, can override with escape codes.

    bool m_cached; // all vbo burn-in bits ready.
    bool m_visible;
    uint8_t m_fontNum;
    float m_fontSize;
    NvBitFont *m_font;
    
    NvBftAlign::Enum m_hMode;
    NvBftAlign::Enum m_vMode;
    float m_hPos; // 0-1 coordinates
    float m_vPos; // 0-1 coordinates
    float m_textLeft;
    float m_textTop; // screen coordinates

    float m_boxWidth; // max width before cap/wrap, zero if none.
    float m_boxHeight;
    int32_t m_boxLines;
    
    bool m_hasBox; // do we have a box set that we are going to adjust to?
    bool m_doWrap;
    bool m_doScissor; // !!!!TBD!!!!TBD
    bool m_posCached; // position has alignment applied.
    
    uint32_t m_truncChar; // if a ... or other char.  zero if none.
    
    char m_shadowDir; // signed so we can use it as shift amount
    NvPackedColor m_shadowColor;

    bool m_outline;
    NvPackedColor m_outlineColor;
    
    float m_pixelsWide;
    float m_pixelsHigh; // size of text.

    NvBFTextRender* m_render;
};

#endif //_nvbitfont_h_
