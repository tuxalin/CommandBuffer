//----------------------------------------------------------------------------------
// File:        NvGLUtils/NvShapesGL.cpp
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
#include "NvGLUtils/NvShapesGL.h"

void NvDrawQuadGL(GLuint posIndex)
{
    const float position[] = {
        -1.0f, -1.0f,
        1.0f, -1.0f,
        -1.0f, 1.0f,
        1.0f, 1.0f,
    };
    
    glVertexAttribPointer(posIndex, 2, GL_FLOAT, false, 2*sizeof(float), position);
    glEnableVertexAttribArray(posIndex);

    glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);

    glDisableVertexAttribArray(posIndex);
    
    //checkGlError("drawQuad");
}

void NvDrawQuadGL(GLint positionAttrib, GLint texcoordAttrib)
{
 // vertex positions in NDC tex-coords
 static const float fullScreenQuadData[] = 
 {
    -1.0f, -1.0f, -1.0f, 1.0f,  0.0f, 0.0f,
     1.0f, -1.0f, -1.0f, 1.0f,  1.0f, 0.0f,
    -1.0f,  1.0f, -1.0f, 1.0f, 0.0f, 1.0f,
    1.0f,  1.0f, -1.0f, 1.0f, 1.0f, 1.0f
 };

 glVertexAttribPointer(positionAttrib, 4, GL_FLOAT, GL_FALSE, 6*sizeof(float), fullScreenQuadData);
 glVertexAttribPointer(texcoordAttrib, 2, GL_FLOAT, GL_FALSE, 6*sizeof(float), &fullScreenQuadData[4]);

 glEnableVertexAttribArray(positionAttrib);
 glEnableVertexAttribArray(texcoordAttrib);

 glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);

 glDisableVertexAttribArray(positionAttrib);
 glDisableVertexAttribArray(texcoordAttrib);
}



void NvDrawCubeGL(GLuint posIndex)
{
    const float positions[] = {
        -1.0f, -1.0f, 1.0f,
        1.0f, -1.0f, 1.0f,
        -1.0f, 1.0f, 1.0f,
        1.0f, 1.0f, 1.0f,
        -1.0f, -1.0f, -1.0f,
        1.0f, -1.0f, -1.0f,
        -1.0f, 1.0f, -1.0f,
        1.0f, 1.0f, -1.0f,
    };
    
    const GLushort indices[] = {
        0, 1, 2, 2, 1, 3,   // f
        4, 6, 5, 5, 6, 7,   // b
        4, 0, 6, 6, 0, 2,   // l
        1, 5, 3, 3, 5, 7,   // r
        2, 3, 6, 6, 3, 7,   // t
        4, 5, 0, 0, 5, 1,   // b
    };

    glVertexAttribPointer(posIndex, 3, GL_FLOAT, false, 3*sizeof(float), positions);
    glEnableVertexAttribArray(posIndex);

    glDrawElements(GL_TRIANGLES, sizeof(indices)/sizeof(GLushort), GL_UNSIGNED_SHORT, indices);

    glDisableVertexAttribArray(posIndex);
    
    //checkGlError("drawCube");
}

void NvDrawWireCubeGL(GLuint posIndex)
{
    const float positions[] = {
        -1.0f, -1.0f, 1.0f,
        1.0f, -1.0f, 1.0f,
        1.0f, 1.0f, 1.0f,
        -1.0f, 1.0f, 1.0f,
        -1.0f, -1.0f, -1.0f,
        1.0f, -1.0f, -1.0f,
        1.0f, 1.0f, -1.0f,
        -1.0f, 1.0f, -1.0f,
    };
    
    const GLushort indices[] = {
        0, 1, 1, 2, 2, 3, 3, 0,    // f
        4, 5, 5, 6, 6, 7, 7, 4, // b
        0, 4, 1, 5, 2, 6, 3, 7
    };

    glVertexAttribPointer(posIndex, 3, GL_FLOAT, false, 3*sizeof(float), positions);
    glEnableVertexAttribArray(posIndex);

    glDrawElements(GL_LINES, sizeof(indices)/sizeof(GLushort), GL_UNSIGNED_SHORT, indices);

    glDisableVertexAttribArray(posIndex);
    
    //checkGlError("drawWireCube");
}

void NvDrawPointGL(nv::vec3f &pos, GLuint posIndex)
{
    glVertexAttribPointer(posIndex, 3, GL_FLOAT, false, 3*sizeof(float), pos.get_value());
    glEnableVertexAttribArray(posIndex);

    glDrawArrays(GL_POINTS, 0, 1);

    glDisableVertexAttribArray(posIndex);
}