//----------------------------------------------------------------------------------
// File:        NvGLUtils/NvBitFontGL.cpp
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
#include "../../src/NvUI/NvBitFontInternal.h"

#include <NvGLUtils/NvGLSLProgram.h>
#include "NvGLUtils/NvImageGL.h"
#include "NV/NvPlatformGL.h"

static int32_t TestPrintGLError(const char* str)
{
    int32_t err;
    err = glGetError();
    if (err)
        VERBOSE_LOG(str, err);
    return err;
}

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

class NvBitFontRenderGL: public NvBitFontRender {
public:
    virtual ~NvBitFontRenderGL() {
        // delete font texture
        glDeleteTextures(1, &(m_tex));
    }

    GLuint m_tex;
};

class NvBFTextRenderGL : public NvBFTextRender {
public:
    NvBFTextRenderGL();
    virtual ~NvBFTextRenderGL();
    virtual void RenderPrep();
    virtual void Render(const float* matrix, const NvPackedColor& color, NvBitFont* font, bool outline, int count);
    virtual void UpdateText(int count, const BFVert* data, bool midrender);
    virtual void RenderDone();

    GLuint m_vbo;

    static int32_t UpdateMasterIndexBuffer(int32_t stringMax, bool midrender);

};

NvBFTextRender* NvBFTextRender::Create() {
    return new NvBFTextRenderGL;
}

class NvBFProgram : public NvGLSLProgram
{
public:
    GLint fontProgLocMat;
    GLint fontProgLocTex;
    GLint fontProgAttribPos, fontProgAttribCol, fontProgAttribTex;
    // I can subclass, but for now just 'share'.
    GLint fontProgLocScale; // inv w+h vec2
    GLint fontProgLocOutlineColor;

    void cacheLocations(void)
    {
        enable();
    
        // grab new uniform locations.
        fontProgLocMat = getUniformLocation("pixelToClipMat");
        fontProgLocTex = getUniformLocation("fontTex");
        fontProgLocScale = getUniformLocation("fontScale");
        fontProgLocOutlineColor = getUniformLocation("outlineColor");

        fontProgAttribPos = getAttribLocation("pos_attr");
        fontProgAttribCol = getAttribLocation("col_attr");
        fontProgAttribTex = getAttribLocation("tex_attr");

        // and bind the uniform for the sampler
        // as it never changes.
        glUniform1i(fontProgLocTex, 0);

        disable();
    };

    static NvBFProgram* createFromStrings(const char* vertSrc, const char* fragSrc, bool strict = false)
    {
        NvBFProgram* prog = new NvBFProgram;
        if (prog->setSourceFromStrings(vertSrc, fragSrc, strict)) {
            prog->cacheLocations();
            return prog;
        } else {
            delete prog;
            return NULL;
        }
    }
};

//========================================================================
// static vars
//========================================================================

// track shader programs...
static NvBFProgram *fontProg = 0;
static NvBFProgram *fontOutlineProg = 0;
static int32_t maxIndexChars = 0;
static int16_t *masterTextIndexList = NULL;

//========================================================================
//========================================================================
static NvBFProgram *lastFontProgram = NULL;

static GLuint masterTextIndexVBO = 0;

const static char s_fontVertShader[] =
"#version 100\n"
"// this is set from higher level.  think of it as the upper model matrix\n"
"uniform mat4 pixelToClipMat;\n"
"attribute vec2 pos_attr;\n"
"attribute vec2 tex_attr;\n"
"attribute vec4 col_attr;\n"
"varying vec4 col_var;\n"
"varying vec2 tex_var;\n"
"void main() {\n"
"    // account for translation and rotation of the primitive into [-1,1] spatial default.\n"
"    gl_Position = pixelToClipMat * vec4(pos_attr.x, pos_attr.y, 0, 1);\n"
"    col_var = col_attr;"
"    tex_var = tex_attr;\n"
"}\n";

const static char s_fontFragShader[] =
"#version 100\n"
"precision mediump float;\n"
"uniform sampler2D fontTex;\n"
"varying vec4 col_var;\n"
"varying vec2 tex_var;\n"
"void main() {\n"
"    float alpha = texture2D(fontTex, tex_var).a;\n"
"    gl_FragColor = col_var * vec4(1.0, 1.0, 1.0, alpha);\n"
"}\n";

// this seems the most efficient way to outline an existing glyph.
// one key thing is it samples 'x' and not '+', helping sharp corners.
const static char s_fontOutlineFragShader[] =
"#version 100\n"
"precision mediump float;\n"
"uniform sampler2D fontTex;\n"
"uniform vec2 texelScale;\n"
"uniform vec4 outlineColor;\n"
"varying vec4 col_var;\n"
"varying vec2 tex_var;\n"
"void main() {\n"
"    float alpha = texture2D(fontTex, tex_var).a;\n"
"    vec4 baseCol = col_var;\n"
"    baseCol.a = alpha;\n"
"    float left = tex_var.x - texelScale.x;\n"
"    float right = tex_var.x + texelScale.x;\n"
"    float top = tex_var.y - texelScale.y;\n"
"    float bottom = tex_var.y + texelScale.y;\n"
"    float a1 = texture2D(fontTex, vec2(left, top)).a;\n"
"    float a2 = texture2D(fontTex, vec2(right, top)).a;\n"
"    float a3 = texture2D(fontTex, vec2(left, bottom)).a;\n"
"    float a4 = texture2D(fontTex, vec2(right, bottom)).a;\n"
"    vec4 lineCol = outlineColor;\n"
"    lineCol.a = clamp(a1+a2+a3+a4+alpha, 0.0, 1.0);\n"
"    gl_FragColor = mix(lineCol, baseCol, alpha);\n"
"}\n";



//========================================================================
// !!!!TBD needs a lot more error handling with finding the files...
// should also allow a method for being handed off the data from the app,
// rather than opening the files here.  split this into two funcs!!!!!TBD
//========================================================================
int32_t NvBitFontRenderInit()
{
    if (TestPrintGLError("> Caught GL error 0x%x @ top of NvBFInitialize...\n"))
    {
        //return(1);
    }

    if (fontProg == 0)
    { // then not one set already, load one...
        fontProg = NvBFProgram::createFromStrings(s_fontVertShader, s_fontFragShader);
        if (0 == fontProg)
        {
            ERROR_LOG("!!> NvBFInitialize failure: couldn't load shader program...\n");
            return(1);
        }

        fontOutlineProg = NvBFProgram::createFromStrings(s_fontVertShader, s_fontOutlineFragShader);
        if (0 == fontOutlineProg)
        {
            ERROR_LOG("!!> NvBFInitialize failure: couldn't load outlining shader program...\n");
            //return(1);
        }
    }

    // since this is our 'system initialization' function, allocate the master index VBO here.
    if (masterTextIndexVBO == 0)
    {
        glGenBuffers(1, &masterTextIndexVBO);
        if (TestPrintGLError("Error 0x%x NvBFInitialize master index vbo...\n"))
            return(1);
    }

    return 0;
}

void NvBitFontRenderShutdown()
{
    // free the shader
    delete fontProg;
    fontProg = 0;
    delete fontOutlineProg;
    fontOutlineProg = 0;
    // NvFree the master index vbo
    if (masterTextIndexVBO)
    {
        glDeleteBuffers(1, &masterTextIndexVBO);
        masterTextIndexVBO = 0;
    }

    free(masterTextIndexList);
    masterTextIndexList = NULL;
    maxIndexChars = 0;
}

NvBitFontRender* NvBitFontRender::Create(NvImage *image)
{
    NvBitFontRenderGL* render = new NvBitFontRenderGL;

    TestPrintGLError("Error 0x%x NvBFInitialize before texture gen...\n");
    // GL initialization...
    render->m_tex = NvImageGL::UploadTexture(image);
    TestPrintGLError("Error 0x%x NvBFInitialize after texture load...\n");

    // set up any tweaks to texture state here...
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, render->m_tex);
    if (image->getMipLevels() > 1)
        glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
    else
        glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    // then turn the bind back off!
    glBindTexture(GL_TEXTURE_2D, 0);

    return render;
}



//========================================================================
NvBFTextRenderGL::NvBFTextRenderGL() : m_vbo(0) {
    if (!m_vbo)
        glGenBuffers(1, &(m_vbo)); // !!!!TBD TODO error handling.
}

//========================================================================
NvBFTextRenderGL::~NvBFTextRenderGL()
{
    if (m_vbo)
        glDeleteBuffers(1, &(m_vbo));
    m_vbo = 0;
}


//========================================================================
//========================================================================
//========================================================================
void NvBFTextRenderGL::RenderPrep()
{
    //lastFontProgram = NULL;

    // set up master rendering state
    glDisable(GL_CULL_FACE);
    glDisable(GL_DEPTH_TEST);
    glDisable(GL_STENCIL_TEST);
    glDepthMask(GL_FALSE);

    // blending...
    glEnable(GL_BLEND);

    // texture to base.
    glActiveTexture(GL_TEXTURE0);
    // do we need to loop over TUs and disable any we're not using!?!?! !!!!TBD

    // any master buffer...
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, masterTextIndexVBO);

    TestPrintGLError("Error 0x%x in NvBFText::RenderPrep...\n");
}


void NvBFTextRenderGL::Render(const float* matrix, const NvPackedColor& color, NvBitFont* font, bool outline, int count)
{

    // we're past all early-exits here.
    // let's make sure we have the right program.
    NvBFProgram *prog = fontProg;
    if (fontOutlineProg != NULL && outline) prog = fontOutlineProg;
    if (lastFontProgram != prog)
    {
        if (lastFontProgram)
            lastFontProgram->disable();
        prog->enable();
        lastFontProgram = prog;
    }

    // set up master rendering state
    {
        uint8_t *offset = NULL;
        glBindBuffer(GL_ARRAY_BUFFER, m_vbo);

        glVertexAttribPointer(prog->fontProgAttribPos, 2, GL_FLOAT, 0, sizeof(BFVert), (void *)offset);
        glEnableVertexAttribArray(prog->fontProgAttribPos);
        offset += sizeof(float) * 2; // jump ahead the two floats

        glVertexAttribPointer(prog->fontProgAttribTex, 2, GL_FLOAT, 0, sizeof(BFVert), (void *)offset); // !!!!TBD update this to use a var if we do 2 or 3 pos verts...
        glEnableVertexAttribArray(prog->fontProgAttribTex);
        offset += sizeof(float) * 2; // jump ahead the two floats.

        glVertexAttribPointer(prog->fontProgAttribCol, 4, GL_UNSIGNED_BYTE, 1, sizeof(BFVert), (void *)offset); // !!!!TBD update this to use a var if we do 2 or 3 pos verts...
        glEnableVertexAttribArray(prog->fontProgAttribCol);
        offset += sizeof(GLuint);
    }

    glUniformMatrix4fv(prog->fontProgLocMat, 1, GL_FALSE, matrix);

    // bind texture... now with simplistic state caching
    //    if (lastFontTexture != m_font->m_tex)
    //    {
    glBindTexture(GL_TEXTURE_2D, ((NvBitFontRenderGL*)(font->m_render))->m_tex);

    if (prog->fontProgLocScale >= 0)
        glUniform2f(prog->fontProgLocScale,
        1.0f / font->m_afont->m_charCommon.m_pageWidth,
        1.0f / font->m_afont->m_charCommon.m_pageHeight);
    if (prog->fontProgLocOutlineColor >= 0)
        glUniform4f(prog->fontProgLocOutlineColor,
        (NV_PC_RED_FLOAT(color)),
        (NV_PC_GREEN_FLOAT(color)),
        (NV_PC_BLUE_FLOAT(color)),
        (NV_PC_ALPHA_FLOAT(color)));
    //    }

    // now, switch blend mode to work for our luma-based text texture.
    if (font->m_alpha)
    {
        // We need to have the alpha make the destination alpha
        // so that text doesn't "cut through" existing opaque
        // destination alpha
        glBlendFuncSeparate(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA,
            GL_ONE, GL_ONE);
        // glBlendAmout(0.5??) !!!!!TBD
    }

    // draw it already!
    //DEBUG_LOG("printing %d chars...", count);
    glDrawElements(GL_TRIANGLES, IND_PER_QUAD * count, GL_UNSIGNED_SHORT, 0);

    TestPrintGLError("Error 0x%x NvBFText::Render drawels...\n");
}

void NvBFTextRenderGL::UpdateText(int count, const BFVert* data, bool midrender)
{
    if (!count)
        return;
    // first, check that our master index buffer is big enough.
    if (UpdateMasterIndexBuffer(count, midrender))
        return; // TODO FIXME error output/handling.

    glBindBuffer(GL_ARRAY_BUFFER, m_vbo);
    glBufferData(GL_ARRAY_BUFFER, count*sizeof(BFVert)*VERT_PER_QUAD, data, GL_DYNAMIC_DRAW);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
}

void NvBFTextRenderGL::RenderDone()
{
    glBindTexture(GL_TEXTURE_2D, 0);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
    glBindBuffer(GL_ARRAY_BUFFER, 0);

    // !!!!TBD TODO do we need to get this working again??
    //nv_flush_tracked_attribs(); // clear any attrib binds.
    if (lastFontProgram)
    {
        glDisableVertexAttribArray(lastFontProgram->fontProgAttribPos);
        glDisableVertexAttribArray(lastFontProgram->fontProgAttribTex);
        glDisableVertexAttribArray(lastFontProgram->fontProgAttribCol);

        lastFontProgram->disable();
        lastFontProgram = NULL;
    }
}

int32_t NvBFTextRenderGL::UpdateMasterIndexBuffer(int32_t stringMax, bool midrender)
{
    if (stringMax>maxIndexChars) // reallocate...
    {
        if (masterTextIndexList) // delete first..
            free(masterTextIndexList);
        masterTextIndexList = NULL;

        maxIndexChars = stringMax; // easy solution, keep these aligned!
        int32_t n = sizeof(int16_t) * IND_PER_QUAD * maxIndexChars;
        masterTextIndexList = (int16_t*)malloc(n);
        if (masterTextIndexList == NULL)
            return -1;

        // re-init the index buffer.
        for (int32_t c = 0; c<maxIndexChars; c++) // triangle list indices... three per triangle, six per quad.
        {
            masterTextIndexList[c * 6 + 0] = (int16_t)(c * 4 + 0);
            masterTextIndexList[c * 6 + 1] = (int16_t)(c * 4 + 2);
            masterTextIndexList[c * 6 + 2] = (int16_t)(c * 4 + 1);
            masterTextIndexList[c * 6 + 3] = (int16_t)(c * 4 + 0);
            masterTextIndexList[c * 6 + 4] = (int16_t)(c * 4 + 3);
            masterTextIndexList[c * 6 + 5] = (int16_t)(c * 4 + 2);
        }

        if (!midrender)
            glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, masterTextIndexVBO);
        glBufferData(GL_ELEMENT_ARRAY_BUFFER, n, masterTextIndexList, GL_STATIC_DRAW);
        if (!midrender)
            glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
    }

    return 0;
}

void NvBitfontUseGL() {
    NvBitFontRenderFactory::GlobalInit = &NvBitFontRenderInit;
    NvBitFontRenderFactory::FontRenderCreate = &NvBitFontRender::Create;
    NvBitFontRenderFactory::TextRenderCreate = &NvBFTextRenderGL::Create;
    NvBitFontRenderFactory::GlobalShutdown = &NvBitFontRenderShutdown;
}
