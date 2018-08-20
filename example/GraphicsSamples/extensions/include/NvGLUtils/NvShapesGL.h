//----------------------------------------------------------------------------------
// File:        NvGLUtils/NvShapesGL.h
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

#ifndef NVSHAPES_H_
#define NVSHAPES_H_

#include <NvSimpleTypes.h>


#include "NV/NvPlatformGL.h"
#include "NV/NvMath.h"

/// \file
/// Basic shape rendering.
/// Functions to draw basic shapes, allowing the application to provide its own shader.

/// Draw a 2D quad with a single vertex attribute.
/// Renders a normalized quad ([-1.0, 1.0] in X and Y) using the currently-bound shader
/// Binds the vertex position array to the given attribute array index and draws the object
/// using the bound shader
/// \param[in] posIndex the vertex attribute array index that represents position in the current shader
void NvDrawQuadGL(GLuint posIndex);

/// Draw a 2D quad with positions and UVs.
/// Renders a normalized quad ([-1.0, 1.0] in X and Y) with normalized UVs [0.0, 1.0]
/// using the currently-bound shader
/// Binds the vertex position and UVs arrays to the given attribute array indices and draws the object
/// using the bound shader
/// \param[in] positionAttrib the vertex attribute array index that represents position in the current shader
/// \param[in] texcoordAttrib the vertex attribute array index that represents 2D UVs in the current shader
void NvDrawQuadGL(GLint positionAttrib, GLint texcoordAttrib);

/// Draw a cube with positions.
/// Renders a normalized cube ([-1.0, 1.0] in X, Y, and Z) using the currently-bound shader
/// Binds the vertex position array to the given attribute array index and draws the object
/// using the bound shader
/// \param[in] posIndex the vertex attribute array index that represents position in the current shader
void NvDrawCubeGL(GLuint posIndex);

/// Draw a wireframe cube with positions.
/// Renders a normalized cube ([-1.0, 1.0] in X, Y, and Z) as lines using the currently-bound shader
/// Binds the vertex position array to the given attribute array index and draws the object
/// using the bound shader
/// \param[in] posIndex the vertex attribute array index that represents position in the current shader
void NvDrawWireCubeGL(GLuint posIndex);


/// Draw a single point with given position.
/// Renders a single point (GL_POINTS) using the currently-bound shader
/// Binds the vertex position array to the given attribute array index and draws one point
/// using the bound shader
/// \param[in] pos the point to be drawn
/// \param[in] posIndex the vertex attribute array index that represents position in the current shader
void NvDrawPointGL(nv::vec3f &pos, GLuint posIndex);

#endif