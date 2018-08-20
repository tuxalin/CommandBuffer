//----------------------------------------------------------------------------------
// File:        NvGLUtils/NvModelGL.cpp
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

#include "NV/NvLogs.h"
#include "NvGLUtils/NvModelGL.h"
#include "NvModel/NvModel.h"
#include "../NvModel/NvModelObj.h"

#define OFFSET(n) ((char *)NULL + (n))

NvModelGL::NvModelGL()
:model_vboID(0), model_iboID(0)
{
    glGenBuffers(1, &model_vboID);
    glGenBuffers(1, &model_iboID);
    model = NULL;
}

NvModelGL::~NvModelGL()
{
    glDeleteBuffers(1, &model_vboID);
    glDeleteBuffers(1, &model_iboID);
    delete model;
}

NvModelGL* NvModelGL::CreateFromObj(uint8_t* data, float scale, bool computeNormals, bool computeTangents) {
	NvModel* model = NvModelObj::CreateFromObj(data, scale, computeNormals, computeTangents);
	if (!model)
		return NULL;

	NvModelGL* modelGL = new NvModelGL;
	modelGL->model = model;
	modelGL->computeCenter();
	modelGL->initBuffers();

	return modelGL;
}

NvModelGL* NvModelGL::CreateFromPreprocessed(uint8_t* data) {
	NvModel* model = NvModel::CreateFromPreprocessed(data);
	if (!model)
		return NULL;

	NvModelGL* modelGL = new NvModelGL;
	modelGL->model = model;
	modelGL->computeCenter();
	modelGL->initBuffers();

	return modelGL;
}

NvModelGL* NvModelGL::CreateFromModel(NvModel* model) {
	if (!model)
		return NULL;

	NvModelGL* modelGL = new NvModelGL;
	modelGL->model = model;
	modelGL->computeCenter();
	modelGL->initBuffers();

	return modelGL;
}

void NvModelGL::computeCenter()
{
    model->getBoundingBox(m_minExtent, m_maxExtent);

    m_radius = (m_maxExtent - m_minExtent) / 2.0f;
    m_center = m_minExtent + m_radius;
}

void NvModelGL::initBuffers()
{
    //print the number of vertices...
    //LOGI("Model Loaded - %d vertices\n", model->getCompiledVertexCount());
	NvModelPrimType::Enum prim;
	mIndexCount = model->getCompiledIndexCount(prim);

    glBindBuffer(GL_ARRAY_BUFFER, model_vboID);
    glBufferData(GL_ARRAY_BUFFER, model->getCompiledVertexCount() * model->getCompiledVertexSize() * sizeof(float), model->getCompiledVertices(), GL_STATIC_DRAW);
    glBindBuffer(GL_ARRAY_BUFFER, 0);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, model_iboID);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, mIndexCount * sizeof(uint32_t), model->getCompiledIndices(prim), GL_STATIC_DRAW);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
}


void NvModelGL::bindBuffers()
{
    glBindBuffer(GL_ARRAY_BUFFER, model_vboID);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, model_iboID);
}

void NvModelGL::unbindBuffers()
{
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
}

void NvModelGL::drawElements(GLint positionHandle)
{
    bindBuffers();
    glVertexAttribPointer(positionHandle, model->getPositionSize(), GL_FLOAT, GL_FALSE, model->getCompiledVertexSize() * sizeof(float), 0);
    glEnableVertexAttribArray(positionHandle);
	glDrawElements(GL_TRIANGLES, mIndexCount, GL_UNSIGNED_INT, 0);
    glDisableVertexAttribArray(positionHandle);
    unbindBuffers();
}

void NvModelGL::drawElements(GLint positionHandle, GLint normalHandle)
{
    bindBuffers();
    glVertexAttribPointer(positionHandle, model->getPositionSize(), GL_FLOAT, GL_FALSE, model->getCompiledVertexSize() * sizeof(float), 0);
    glEnableVertexAttribArray(positionHandle);
    
    if (normalHandle >= 0) {
        glVertexAttribPointer(normalHandle, model->getNormalSize(), GL_FLOAT, GL_FALSE, model->getCompiledVertexSize() * sizeof(float), OFFSET(model->getCompiledNormalOffset()*sizeof(float)));
        glEnableVertexAttribArray(normalHandle);
    }
    
	glDrawElements(GL_TRIANGLES, mIndexCount, GL_UNSIGNED_INT, 0);

    glDisableVertexAttribArray(positionHandle);
    if (normalHandle >= 0)
        glDisableVertexAttribArray(normalHandle);
    unbindBuffers();
}

void NvModelGL::drawElements(GLint positionHandle, GLint normalHandle, GLint texcoordHandle)
{
    bindBuffers();
    glVertexAttribPointer(positionHandle, model->getPositionSize(), GL_FLOAT, GL_FALSE, model->getCompiledVertexSize() * sizeof(float), 0);
    glEnableVertexAttribArray(positionHandle);

    if (normalHandle >= 0) {
        glVertexAttribPointer(normalHandle, model->getNormalSize(), GL_FLOAT, GL_FALSE, model->getCompiledVertexSize() * sizeof(float), OFFSET(model->getCompiledNormalOffset()*sizeof(float)));
        glEnableVertexAttribArray(normalHandle);
    }

    if (texcoordHandle >= 0) {
        glVertexAttribPointer(texcoordHandle, model->getTexCoordSize(), GL_FLOAT, GL_FALSE, model->getCompiledVertexSize() * sizeof(float), OFFSET(model->getCompiledTexCoordOffset()*sizeof(float)));
        glEnableVertexAttribArray(texcoordHandle);
    }
    
	glDrawElements(GL_TRIANGLES, mIndexCount, GL_UNSIGNED_INT, 0);
    
    glDisableVertexAttribArray(positionHandle);
    if (normalHandle >= 0)
        glDisableVertexAttribArray(normalHandle);
    if (texcoordHandle >= 0)
        glDisableVertexAttribArray(texcoordHandle);
    unbindBuffers();
}

void NvModelGL::drawElements(GLint positionHandle, GLint normalHandle, GLint texcoordHandle, GLint tangentHandle)
{
    bindBuffers();
    bindBuffers();
    glVertexAttribPointer(positionHandle, model->getPositionSize(), GL_FLOAT, GL_FALSE, model->getCompiledVertexSize() * sizeof(float), 0);
    glEnableVertexAttribArray(positionHandle);

    if (normalHandle >= 0) {
        glVertexAttribPointer(normalHandle, model->getNormalSize(), GL_FLOAT, GL_FALSE, model->getCompiledVertexSize() * sizeof(float), OFFSET(model->getCompiledNormalOffset()*sizeof(float)));
        glEnableVertexAttribArray(normalHandle);
    }

    if (texcoordHandle >= 0) {
        glVertexAttribPointer(texcoordHandle, model->getTexCoordSize(), GL_FLOAT, GL_FALSE, model->getCompiledVertexSize() * sizeof(float), OFFSET(model->getCompiledTexCoordOffset()*sizeof(float)));
        glEnableVertexAttribArray(texcoordHandle);
    }

    if (tangentHandle >= 0) {
        glVertexAttribPointer(tangentHandle,  model->getTangentSize(), GL_FLOAT, GL_FALSE, model->getCompiledVertexSize() * sizeof(float), OFFSET(model->getCompiledTangentOffset()*sizeof(float)));
        glEnableVertexAttribArray(tangentHandle);
    }
	glDrawElements(GL_TRIANGLES, mIndexCount, GL_UNSIGNED_INT, 0);

    glDisableVertexAttribArray(positionHandle);
    if (normalHandle >= 0)
        glDisableVertexAttribArray(normalHandle);
    if (texcoordHandle >= 0)
        glDisableVertexAttribArray(texcoordHandle);
    if (tangentHandle >= 0)
        glDisableVertexAttribArray(tangentHandle);
    unbindBuffers();
}

NvModel * NvModelGL::getModel()
{
    return model;
}
