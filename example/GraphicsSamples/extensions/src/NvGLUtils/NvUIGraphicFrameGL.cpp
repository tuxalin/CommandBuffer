//----------------------------------------------------------------------------------
// File:        NvGLUtils/NvUIGraphicFrameGL.cpp
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

#include "NvAssetLoader/NvAssetLoader.h"

#include "NvUIGL.h"

#include <math.h>



int32_t NvUIGraphicFrameRenderGL::ms_staticCount = 0;

NvGraphicFrameShaderGL NvUIGraphicFrameRenderGL::ms_shader;

GLuint NvUIGraphicFrameRenderGL::ms_gfvbo = 0;
GLuint NvUIGraphicFrameRenderGL::ms_gfibo = 0;

//======================================================================
//======================================================================
NvUIGraphicFrameRender* NvUIGraphicFrameRenderGL::Create(NvUIGraphicFrame* graphic) {
    return new NvUIGraphicFrameRenderGL(graphic);
}

NvUIGraphicFrameRenderGL::NvUIGraphicFrameRenderGL(NvUIGraphicFrame* graphic)
{
    m_graphic = graphic;
    StaticInit();
}

NvUIGraphicFrameRenderGL::~NvUIGraphicFrameRenderGL()
{
    StaticCleanup();
}

bool NvUIGraphicFrameRenderGL::StaticCleanup()
{
    if (--ms_staticCount == 0)
    {
        glBindBuffer(GL_ARRAY_BUFFER, 0);
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
        glUseProgram(0);

        delete ms_shader.m_program;
        ms_shader.m_program = 0;

        glDeleteBuffers(1, &ms_gfvbo);
        glDeleteBuffers(1, &ms_gfibo);
        ms_gfvbo = 0;
        ms_gfibo = 0;
    }

    return true;
}

//======================================================================
//======================================================================
bool NvUIGraphicFrameRenderGL::StaticInit()
{
    ms_staticCount++;
    
    if (!ms_gfvbo)
    {
        // TODO: disable drawing of the center piece?
        NvFrameVertex vert[4*4];
        NvFrameVertex temp[4] =
        {
            {-1,  1, 0, 1, 0, 0},
            { 1,  1, 1, 1, 0, 0},
            {-1, -1, 0, 0, 0, 0},
            { 1, -1, 1, 0, 0, 0}
        };

        for (int32_t y = 0; y < 4; y++)
        {
            for (int32_t x = 0; x < 4; x++)
            {
                int32_t src = ((y >> 1)*2) + (x >> 1);  //0,0,1,1,0,0,1,1,2,2,3,3,2,2,3,3
                int32_t dst = y*4+x;                    //0,1,2,3,4,5,6,7...
                memcpy(&vert[dst], &temp[src], sizeof(NvFrameVertex));
                if (y == 0 || y == 3)
                    vert[dst].by = 1;
                if (x == 0 || x == 3)
                    vert[dst].bx = 1;
            }
        }

        uint16_t indices[30+6] = {
                  0, 4, 1, 5, 2, 6, 3, 7,         7, // first row
          4,      4, 8, 5, 9,                     9, // left panel
          6,      6, 10, 7, 11, 11, // right panel
          8,      8, 12, 9, 13, 10, 14, 11, 15,   15, // bottom
          5,      5, 9, 6, 10 // center piece
        };

        ms_shader.Load();

        CHECK_GL_ERROR();

        glGenBuffers(1, &ms_gfibo);
        glGenBuffers(1, &ms_gfvbo);

        glBindBuffer(GL_ARRAY_BUFFER, ms_gfvbo);
        glBufferData(GL_ARRAY_BUFFER, 4 * 4 * sizeof(NvFrameVertex), vert, GL_STATIC_DRAW);

        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ms_gfibo);
        glBufferData(GL_ELEMENT_ARRAY_BUFFER, (30+6) * sizeof(uint16_t),
            indices, GL_STATIC_DRAW);

        glBindBuffer(GL_ARRAY_BUFFER, 0);
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
    }

    CHECK_GL_ERROR();

    return true;
}

//======================================================================
//======================================================================
void NvUIGraphicFrameRenderGL::Draw(float alpha, const NvPackedColor& color, const float pixelToClipMatrix[4][4], 
                            const nv::vec2<float>& thickness, const nv::vec2<float>& texBorder, bool drawCenter)
{
    // pick correct shader based on alpha...
    ms_shader.m_program->enable();

    // then if alpha shader, set alpha uniform...
    if (ms_shader.m_alphaIndex >= 0)
        glUniform1f(ms_shader.m_alphaIndex, alpha);

    // then if colorizing shader, set color uniform...
    if (ms_shader.m_colorIndex >= 0)
    {   // optimize it a bit...  // !!!!TBD alpha in color not just sep value?
        if (NV_PC_IS_WHITE(color))
            glUniform4f(ms_shader.m_colorIndex, 1,1,1,1);
        else
            glUniform4f(ms_shader.m_colorIndex,
                            NV_PC_RED_FLOAT(color),
                            NV_PC_GREEN_FLOAT(color),
                            NV_PC_BLUE_FLOAT(color),
                            1); // !!!!TBD
    }

    NvUITexture* tex = m_graphic->GetTex();

    glUniform2f(ms_shader.m_texBorderIndex,
                    texBorder.x / tex->GetWidth(),
                    texBorder.y / tex->GetHeight());
    glUniform2f(ms_shader.m_thicknessIndex, thickness.x, thickness.y);

    glUniformMatrix4fv(ms_shader.m_matrixIndex, 1, GL_FALSE, &(pixelToClipMatrix[0][0]));

    // set up texturing.
    bool ae = false;
    if (tex->GetHasAlpha() || (alpha<1.0f))
    {
        ae = true;
        glEnable(GL_BLEND);
        // Alpha sums in the destination channel to ensure that
        // partially-opaque items do not decrease the destination
        // alpha and thus "cut holes" in the backdrop
        glBlendFuncSeparate(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA,
            GL_ONE, GL_ONE);
    }
    else
        glDisable(GL_BLEND);

//    glUniform1i(shader.m_samplerIndex, 0); // texunit index zero.
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, ((NvUITextureRenderGL*)(tex->GetRender()))->m_glID);

    // setup data buffers/attribs.
    glBindBuffer(GL_ARRAY_BUFFER, ms_gfvbo);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ms_gfibo);

    glVertexAttribPointer(ms_shader.m_positionIndex, 2, GL_FLOAT, 0, sizeof(NvFrameVertex), 0);
    glEnableVertexAttribArray(ms_shader.m_positionIndex);
    glVertexAttribPointer(ms_shader.m_uvIndex, 2, GL_FLOAT, 0, sizeof(NvFrameVertex), (void*) (2* 4));
    glEnableVertexAttribArray(ms_shader.m_uvIndex);
    glVertexAttribPointer(ms_shader.m_borderIndex, 2, GL_FLOAT, 0, sizeof(NvFrameVertex), (void*) (2*2* 4));
    glEnableVertexAttribArray(ms_shader.m_borderIndex);

    // draw it already!
    glDrawElements(GL_TRIANGLE_STRIP, drawCenter ? (30+6) : 30, GL_UNSIGNED_SHORT, 0);

    //nv_flush_tracked_attribs();
    glDisableVertexAttribArray(ms_shader.m_positionIndex);
    glDisableVertexAttribArray(ms_shader.m_uvIndex);
    glDisableVertexAttribArray(ms_shader.m_borderIndex);

    if (ae)
        glDisable(GL_BLEND);
}
