//----------------------------------------------------------------------------------
// File:        NvGLUtils/NvUIGL.cpp
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
#include "../../src/NvUI/NvUIInternal.h"
#include "NvUIGL.h"

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
//========================================================================
#define SAVED_ATTRIBS_MAX   16 /* something for now */

typedef struct
{
    GLboolean enabled;
    GLint size;
    GLint stride;
    GLint type;
    GLint norm;
    GLvoid *ptr;
} NvUIGLAttribInfo;

typedef struct
{
    GLint               programBound;
    NvUIGLAttribInfo    attrib[SAVED_ATTRIBS_MAX];

    GLboolean           depthMaskEnabled;
    GLboolean           depthTestEnabled;
    GLboolean           cullFaceEnabled;
    GLboolean           blendEnabled;
    //gStateBlock.blendFunc // !!!!TBD
    //blendFuncSep // tbd

    GLint               vboBound;
    GLint               iboBound;
    GLint               texBound;
    GLint               texActive;

    // stencil?  viewport? // !!!!TBD

} NvUIGLStateBlock;

static NvUIGLStateBlock gStateBlock;

//========================================================================
void NvUISaveStateGL()
{
    int32_t i;
    int32_t tmpi;

    TestPrintGLError("Error 0x%x in SaveState @ start...\n");

    glGetIntegerv(GL_CURRENT_PROGRAM, &(gStateBlock.programBound));
    for (i = 0; i<SAVED_ATTRIBS_MAX; i++)
    {
        glGetVertexAttribiv(i, GL_VERTEX_ATTRIB_ARRAY_ENABLED, &tmpi);
        gStateBlock.attrib[i].enabled = (GLboolean)tmpi;
        if (tmpi)
        {
            glGetVertexAttribiv(i, GL_VERTEX_ATTRIB_ARRAY_SIZE, &(gStateBlock.attrib[i].size));
            glGetVertexAttribiv(i, GL_VERTEX_ATTRIB_ARRAY_STRIDE, &(gStateBlock.attrib[i].stride));
            glGetVertexAttribiv(i, GL_VERTEX_ATTRIB_ARRAY_TYPE, &(gStateBlock.attrib[i].type));
            glGetVertexAttribiv(i, GL_VERTEX_ATTRIB_ARRAY_NORMALIZED, &(gStateBlock.attrib[i].norm));
            glGetVertexAttribPointerv(i, GL_VERTEX_ATTRIB_ARRAY_POINTER, &(gStateBlock.attrib[i].ptr));
        }
    }

    glGetBooleanv(GL_DEPTH_WRITEMASK, &(gStateBlock.depthMaskEnabled));
    gStateBlock.depthTestEnabled = glIsEnabled(GL_DEPTH_TEST);
    gStateBlock.blendEnabled = glIsEnabled(GL_BLEND);
    gStateBlock.cullFaceEnabled = glIsEnabled(GL_CULL_FACE);
    //    glGetIntegerv(GL_CULL_FACE_MODE, &(gStateBlock.cullMode));

    glGetIntegerv(GL_ARRAY_BUFFER_BINDING, &(gStateBlock.vboBound));
    glGetIntegerv(GL_ELEMENT_ARRAY_BUFFER_BINDING, &(gStateBlock.iboBound));
    glGetIntegerv(GL_TEXTURE_BINDING_2D, &(gStateBlock.texBound));
    glGetIntegerv(GL_ACTIVE_TEXTURE, &(gStateBlock.texActive));

    TestPrintGLError("Error 0x%x in SaveState @ end...\n");
}

//========================================================================
void NvUIRestoreStateGL()
{
    int32_t i;

    // !!!!TBD TODO probably should ensure we can do this still... wasn't before though.
    //    nv_flush_tracked_attribs(); // turn ours off...

    TestPrintGLError("Error 0x%x in RestoreState @ start...\n");

    glUseProgram(gStateBlock.programBound);

    // set buffers first, in case attribs bound to them...
    glBindBuffer(GL_ARRAY_BUFFER, gStateBlock.vboBound);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, gStateBlock.iboBound);

    if (gStateBlock.programBound)
    { // restore program stuff..
        for (i = 0; i<SAVED_ATTRIBS_MAX; i++)
        {
            if (gStateBlock.attrib[i].enabled) // only restore enabled ones.. ;)
            {
                glVertexAttribPointer(i,
                    gStateBlock.attrib[i].size,
                    gStateBlock.attrib[i].type,
                    (GLboolean)(gStateBlock.attrib[i].norm),
                    gStateBlock.attrib[i].stride,
                    gStateBlock.attrib[i].ptr);
                glEnableVertexAttribArray(i);
            }
            else
                glDisableVertexAttribArray(i);
        }
    }

    if (gStateBlock.depthMaskEnabled)
        glDepthMask(GL_TRUE); // we turned off.
    if (gStateBlock.depthTestEnabled)
        glEnable(GL_DEPTH_TEST); // we turned off.
    if (!gStateBlock.blendEnabled)
        glDisable(GL_BLEND); // we turned ON.
    if (gStateBlock.cullFaceEnabled)
        glEnable(GL_CULL_FACE); // we turned off.
    //    glGetIntegerv(GL_CULL_FACE_MODE, &(gStateBlock.cullMode));

    // restore tex BEFORE switching active state...
    glBindTexture(GL_TEXTURE_2D, gStateBlock.texBound);
    if (gStateBlock.texActive != GL_TEXTURE0)
        glActiveTexture(gStateBlock.texActive); // we set to 0

    TestPrintGLError("Error 0x%x in RestoreState @ end...\n");
}

extern void NvBitfontUseGL();

static int32_t doNothing() { return 0; }
static void doNothingVoid() { /* */ }

static bool sIsUsingGL = false;

bool NvUIIsGL() {
	return sIsUsingGL;
}

void NvUIUseGL() {
    NvUIRenderFactory::GlobalInit = &doNothing;
    NvUIRenderFactory::GraphicCreate = &NvUIGraphicRenderGL::Create;
    NvUIRenderFactory::GraphicFrameCreate = &NvUIGraphicFrameRenderGL::Create;
    NvUIRenderFactory::TextureRenderCreate = &NvUITextureRenderGL::Create;
    NvUIRenderFactory::GlobalRenderPrep = &NvUISaveStateGL;
    NvUIRenderFactory::GlobalRenderDone = &NvUIRestoreStateGL;
    NvUIRenderFactory::GlobalShutdown = &doNothingVoid;
    NvBitfontUseGL();
	sIsUsingGL = true;
}
