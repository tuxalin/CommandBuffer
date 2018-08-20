//----------------------------------------------------------------------------------
// File:        NvGLUtils/NvUIShadersGL.h
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
#ifndef NV_UI_SHADERS_GL_H
#define NV_UI_SHADERS_GL_H

#include "NvUI/NvUI.h"
#include "NV/NvMath.h"

/** This is a helper structure for rendering sets of 2D textured vertices. */
typedef struct
{
    nv::vec2<float> position; /**< 2d vertex coord */
    nv::vec2<float> uv; /**< vertex texturing position */
} NvTexturedVertex;

//=============================================================================
//=============================================================================
class NvGLSLProgram;
/** A helper object for managing a shader program for NvUIGraphic and subclasses. */
class NvGraphicShaderGL
{
public:
    NvGLSLProgram *m_program; /**< A pointer to an allocated NvGLSLProgram object.  We're not a subclass as we could easily have some other method for loading/storing what shader program we are using (previously, this was just a GLint for the compiled program). */
    //int32_t m_shader; // now stored inside program
    int32_t m_positionIndex; /**< Index for position attribute */
    int32_t m_uvIndex;  /**< Index for uv attribute */
    int32_t m_matrixIndex; /**< Index for matrix uniform */
    int32_t m_alphaIndex; /**< Index for alpha uniform */
    int32_t m_colorIndex; /**< Index for color uniform */

    /** Helper for compiling the given shader strings and then retrieving indicies. */
    virtual void Load();
    void Load(const char* vert, const char* frag);
};



//=============================================================================
//=============================================================================
/** This adds NvUIGraphicFrame features to the NvGraphicShader 'template'. */
class NvGraphicFrameShaderGL : public NvGraphicShaderGL
{
private:
    INHERIT_FROM(NvGraphicShaderGL);

public:
    uint32_t m_borderIndex;
    uint32_t m_thicknessIndex;
    uint32_t m_texBorderIndex;

    /** Helper for compiling the given shader strings and then retrieving indicies.
        Overrides the version in NvGraphicShader, calls the inherited version first,
        then retrieves our additional indices. */
    virtual void Load();
};


#endif
