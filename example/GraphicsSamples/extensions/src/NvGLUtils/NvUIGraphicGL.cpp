//----------------------------------------------------------------------------------
// File:        NvGLUtils/NvUIGraphicGL.cpp
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

//======================================================================
//======================================================================
NvUIGraphicRender* NvUIGraphicRenderGL::Create(NvUIGraphic* graphic) {
    return new NvUIGraphicRenderGL(graphic);
}

//======================================================================
//======================================================================
NvGraphicShaderGL NvUIGraphicRenderGL::ms_shader;

GLuint NvUIGraphicRenderGL::ms_vbo = 0;
GLuint NvUIGraphicRenderGL::ms_ibo = 0;
int32_t NvUIGraphicRenderGL::ms_initCount;


//======================================================================
// ----- NvUIGraphicRenderGL -----
//======================================================================
NvUIGraphicRenderGL::NvUIGraphicRenderGL(NvUIGraphic* graphic)
{
    m_graphic = graphic;
    StaticInit();
}

NvUIGraphicRenderGL::~NvUIGraphicRenderGL()
{
    StaticCleanup();
}


//======================================================================
//======================================================================
bool NvUIGraphicRenderGL::StaticInit()
{
    if (ms_initCount++ == 0)
    {
        NvTexturedVertex    vert[4];
        uint16_t            indices[6];

        ms_shader.Load();

        uint16_t pos = 0;
        int32_t ipos = 0;

        indices[ipos+0] = pos;
        indices[ipos+1] = pos+1;
        indices[ipos+2] = pos+3;
        indices[ipos+3] = pos+3;
        indices[ipos+4] = pos+1;
        indices[ipos+5] = pos+2;

        vert[pos].position.x = 0; 
        vert[pos].position.y = 1;
        vert[pos].uv.x = 0; 
        vert[pos].uv.y = 1;
        pos++;

        vert[pos].position.x = 0; 
        vert[pos].position.y = 0;
        vert[pos].uv.x = 0; 
        vert[pos].uv.y = 0;
        pos++;

        vert[pos].position.x = 1; 
        vert[pos].position.y = 0;
        vert[pos].uv.x = 1; 
        vert[pos].uv.y = 0;
        pos++;

        vert[pos].position.x = 1; 
        vert[pos].position.y = 1;
        vert[pos].uv.x = 1; 
        vert[pos].uv.y = 1;
        pos++;

        glGenBuffers(1, &ms_ibo);
        glGenBuffers(1, &ms_vbo);

        glBindBuffer(GL_ARRAY_BUFFER, ms_vbo);
        glBufferData(GL_ARRAY_BUFFER, 4 * sizeof(NvTexturedVertex), vert, GL_STATIC_DRAW);

        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ms_ibo);
        glBufferData(GL_ELEMENT_ARRAY_BUFFER, 6 * sizeof(uint16_t), indices, GL_STATIC_DRAW);

        CHECK_GL_ERROR();

        glBindBuffer(GL_ARRAY_BUFFER, 0);
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
    }

    CHECK_GL_ERROR();
    
    return true;
}


//======================================================================
//======================================================================
bool NvUIGraphicRenderGL::StaticCleanup()
{
    if (--ms_initCount == 0)
    {
        glBindBuffer(GL_ARRAY_BUFFER, 0);
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
        glUseProgram(0);

        delete ms_shader.m_program;
        ms_shader.m_program = 0;

        glDeleteBuffers(1, &ms_vbo);
        glDeleteBuffers(1, &ms_ibo);
        ms_vbo = 0;
        ms_ibo = 0;
    }

    return true;
}

//======================================================================
//======================================================================
void NvUIGraphicRenderGL::Draw(float alpha, const NvPackedColor& color, const float pixelToClipMatrix[4][4])
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
    
    glUniformMatrix4fv(ms_shader.m_matrixIndex, 1, GL_FALSE, &(pixelToClipMatrix[0][0]));

    // set up texturing.
    bool ae = false;
    if (m_graphic->GetTex()->GetHasAlpha() || (alpha<1.0f))
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

    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, ((NvUITextureRenderGL*)(m_graphic->GetTex()->GetRender()))->m_glID);
    glBindBuffer(GL_ARRAY_BUFFER, ms_vbo);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ms_ibo);

    glVertexAttribPointer(ms_shader.m_positionIndex, 2, GL_FLOAT, 0, sizeof(NvTexturedVertex), 0);
    glEnableVertexAttribArray(ms_shader.m_positionIndex);
    glVertexAttribPointer(ms_shader.m_uvIndex, 2, GL_FLOAT, 0, sizeof(NvTexturedVertex), (void*)sizeof(nv::vec2<float>));
    glEnableVertexAttribArray(ms_shader.m_uvIndex);

    // draw it already!
    glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_SHORT, 0);

    //nv_flush_tracked_attribs();
    glDisableVertexAttribArray(ms_shader.m_positionIndex);
    glDisableVertexAttribArray(ms_shader.m_uvIndex);

    if (ae)
        glDisable(GL_BLEND);
}
