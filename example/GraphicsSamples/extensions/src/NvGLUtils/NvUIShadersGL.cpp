//----------------------------------------------------------------------------------
// File:        NvGLUtils/NvUIShadersGL.cpp
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
#include "NV/NvPlatformGL.h"
#include "NvUIShadersGL.h"
#include "NV/NvLogs.h"
#include "NvGLUtils/NvGLSLProgram.h"

const static char s_graphicVertShader[] =
"#version 100\n"
"// this is set from higher level.  think of it as the upper model matrix\n"
"uniform mat4 pixelToClipMat;\n"
"attribute vec2 position;\n"
"attribute vec2 tex;\n"
"varying vec2 tex_coord;\n"
"void main()\n"
"{\n"
"    gl_Position = pixelToClipMat * vec4(position, 0, 1);\n"
"    tex_coord = tex;\n"
"}\n";

const static char s_graphicFragShader[] =
"#version 100\n"
"precision mediump float;\n"
"varying vec2 tex_coord;\n"
"uniform sampler2D sampler;\n"
"uniform float alpha;\n"
"uniform vec4 color;\n"
"void main()\n"
"{\n"
"    gl_FragColor = texture2D(sampler, tex_coord) * vec4(color.r,color.g,color.b,alpha);\n"
"}\n";


//======================================================================
//======================================================================
void NvGraphicShaderGL::Load()
{
    Load(s_graphicVertShader, s_graphicFragShader);
}

//======================================================================
//======================================================================
void NvGraphicShaderGL::Load(const char* vert, const char* frag)
{
    NvGLSLProgram *prog = NvGLSLProgram::createFromStrings(vert, frag);
    CHECK_GL_ERROR();

    if (prog==NULL)
    {
        // !!!!TBD TODO
        return;
    }

    m_program = prog;
    prog->enable();

    m_positionIndex = prog->getAttribLocation("position");
    m_uvIndex = prog->getAttribLocation("tex");

    prog->setUniform1i(prog->getUniformLocation("sampler"), 0); // texunit index zero.

    m_matrixIndex = prog->getUniformLocation("pixelToClipMat");
    m_alphaIndex = prog->getUniformLocation("alpha");
    m_colorIndex = prog->getUniformLocation("color");

    prog->disable();

    CHECK_GL_ERROR();
}

//======================================================================
// ----- NvUIGraphicFrame -----
//======================================================================

const static char s_frameVertShader[] =
"#version 100\n"
"// this is set from higher level.  think of it as the upper model matrix\n"
"uniform mat4 pixelToClipMat;\n"
"uniform vec2 thickness;\n"
"uniform vec2 texBorder;\n"
"attribute vec2 border;\n"
"attribute vec2 position;\n"
"attribute vec2 tex;\n"
"varying vec2 tex_coord;\n"
"void main()\n"
"{\n"
"    vec2 invBorder = vec2(1,1) - border;\n"
"    vec2 shiftedPosition = (position-thickness*invBorder*position);\n"
"    // we need to convert from -1,1 coords into 0,1 coords before xform.\n"
"    shiftedPosition *= 0.5;\n"
"    shiftedPosition += 0.5;\n"
"    // then we multiply like uigraphic normally would\n"
"    gl_Position = pixelToClipMat * vec4(shiftedPosition, 0, 1);\n"
"    tex_coord = tex + invBorder * -position * texBorder;\n"
"}\n";

// note this is same as uigraphic's frag shader, minus colorization removed.
const static char s_frameFragShader[] =
"#version 100\n"
"precision mediump float;\n"
"varying vec2 tex_coord;\n"
"uniform sampler2D sampler;\n"
"uniform float alpha;\n"
"uniform vec4 color;\n"
"void main()\n"
"{\n"
"    gl_FragColor = texture2D(sampler, tex_coord) * vec4(color.r,color.g,color.b,alpha);\n"
"}\n";


//======================================================================
//======================================================================
void NvGraphicFrameShaderGL::Load()
{
    INHERITED::Load((const char *)s_frameVertShader, s_frameFragShader);

    // inherited Load doesn't keep program enabled for 'safety', so we
    // re-enable here so we can reference ourselves...
    m_program->enable();

    m_borderIndex = m_program->getAttribLocation("border");

    glUniform1i(m_program->getUniformLocation("sampler"), 0); // texunit index zero.

    m_thicknessIndex = m_program->getUniformLocation("thickness");
    m_texBorderIndex = m_program->getUniformLocation("texBorder");

    m_program->disable();
}

