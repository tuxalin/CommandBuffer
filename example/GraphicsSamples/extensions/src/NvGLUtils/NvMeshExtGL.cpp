//----------------------------------------------------------------------------------
// File:        NvGLUtils/NvMeshExtGL.cpp
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

#include "NvGLUtils/NvMeshExtGL.h"
#include "NvModel/NvModelExt.h"
#include "NvModel/NvModelSubMesh.h"
#include "../../src/NvModel/NvModelExtObj.h"
#include "NvModel/NvSkeleton.h"

namespace Nv
{
    // Static function pointers for rendering instanced geometry
    NvGLInstancingSupport::PFNDrawElementsInstanced NvGLInstancingSupport::glDrawElementsInstancedInternal = nullptr;
    NvGLInstancingSupport::PFNVertexAttribDivisor NvGLInstancingSupport::glVertexAttribDivisorInternal = nullptr;

	NvMeshExtGL::NvMeshExtGL() :
		m_pSrcMesh(NULL),
        m_materialID(-1),
        m_vboID(0),
        m_iboID(0),
        m_indexCount(0),
        m_vertexCount(0),
        m_vertexSize(0),
        m_positionSize(3),
        m_positionOffset(0),
        m_normalSize(3),
        m_normalOffset(-1),
        m_texCoordSize(2),
        m_texCoordOffset(-1),
        m_tangentSize(3),
        m_tangentOffset(-1),
		m_colorSize(0),
		m_colorOffset(-1),
        m_parentNode(-1)
    {
    }

	NvMeshExtGL::NvMeshExtGL(const NvMeshExtGL& other) :
		m_pSrcMesh(other.m_pSrcMesh),
		m_materialID(other.m_materialID),
		m_vboID(other.m_vboID),
		m_iboID(other.m_iboID),
		m_indexCount(other.m_indexCount),
		m_vertexCount(other.m_vertexCount),
		m_vertexSize(other.m_vertexSize),
		m_positionSize(other.m_positionSize),
		m_positionOffset(other.m_positionOffset),
		m_normalSize(other.m_normalSize),
		m_normalOffset(other.m_normalOffset),
		m_texCoordSize(other.m_texCoordSize),
		m_texCoordOffset(other.m_texCoordOffset),
		m_tangentSize(other.m_tangentSize),
		m_tangentOffset(other.m_tangentOffset),
		m_colorSize(other.m_colorSize),
		m_colorOffset(other.m_colorOffset),
        m_parentNode(other.m_parentNode),
        m_offsetMatrix(other.m_offsetMatrix)
    {
	}


	NvMeshExtGL::~NvMeshExtGL()
    {
        Clear();
    }


	bool NvMeshExtGL::InitFromSubmesh(NvModelExt* pModel, uint32_t subMeshID)
    {
        if (NULL == pModel)
        {
            return false;
        }

        SubMesh* pSubMesh = pModel->GetSubMesh(subMeshID);
        if (NULL == pSubMesh)
        {
            return false;
        }

		m_pSrcMesh = pSubMesh;

		m_materialID = m_pSrcMesh->m_materialId;

		m_vertexSize = m_pSrcMesh->m_vertSize; // In Floats

		// We will always have positions in our vertex, and we will put 
		// them first in the vertex layout
		m_positionOffset = 0;

		// Account for normals, if there are any
		m_normalOffset = m_pSrcMesh->m_normalOffset * sizeof(float);

		// Account for texture coordinates, if there are any
		m_texCoordOffset = m_pSrcMesh->m_texCoordOffset * sizeof(float);

		// Account for tangents, if there are any
		m_tangentOffset = m_pSrcMesh->m_tangentOffset * sizeof(float);

		// Account for colors, if there are any
		m_colorOffset = m_pSrcMesh->m_colorOffset * sizeof(float);

		uint32_t vertBytes = sizeof(float) * m_vertexSize;

		// Allocate a large enough vertex buffer to hold all vertices in the mesh
		m_vertexCount = m_pSrcMesh->m_vertexCount;

        m_parentNode = m_pSrcMesh->m_parentBone;

		// Create a vertex buffer and fill it with data
		uint32_t vboBytes = vertBytes * m_vertexCount;

        // Set up the GL data structures necessary for the vertex buffer object
        glGenBuffers(1, &m_vboID);
        glBindBuffer(GL_ARRAY_BUFFER, m_vboID);
		glBufferData(GL_ARRAY_BUFFER, vboBytes, m_pSrcMesh->m_vertices, GL_STATIC_DRAW);
        glBindBuffer(GL_ARRAY_BUFFER, 0);

        // Allocate a large enough index buffer to hold the indices for all primitives in the mesh
		m_indexCount = m_pSrcMesh->m_indexCount;
        
        // Set up the GL data structures necessary for the index buffer object
        glGenBuffers(1, &m_iboID);
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_iboID);
		glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(uint32_t) * m_indexCount, m_pSrcMesh->m_indices, GL_STATIC_DRAW);
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
        return true;
    }

    bool NvMeshExtGL::UpdateBoneTransforms(Nv::NvSkeleton* pSrcSkel)
    {
        if ((NULL == m_pSrcMesh) || (NULL == pSrcSkel))
        {
            return false;
        }

        // Update the mesh offset from its parent node
        nv::matrix4f* pSrcTransforms = pSrcSkel->GetTransforms();
        if ((m_parentNode >= 0) && (m_parentNode < pSrcSkel->GetNumNodes()))
        {
            m_offsetMatrix = pSrcTransforms[m_parentNode];
        }
        return true;
    }

	void NvMeshExtGL::DrawElements(uint32_t instanceCount, GLint positionHandle, GLint normalHandle, GLint texcoordHandle, GLint tangentHandle)
    {
        int32_t vertexSizeInBytes = m_vertexSize * sizeof(float);

        // Bind our vertex/index buffers
        glBindBuffer(GL_ARRAY_BUFFER, m_vboID);
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_iboID);

        glVertexAttribPointer(positionHandle, m_positionSize, GL_FLOAT, GL_FALSE, vertexSizeInBytes, (GLvoid*)m_positionOffset);
        glEnableVertexAttribArray(positionHandle);

        if (normalHandle >= 0) {
            glVertexAttribPointer(normalHandle, m_normalSize, GL_FLOAT, GL_FALSE, vertexSizeInBytes, (GLvoid*)m_normalOffset);
            glEnableVertexAttribArray(normalHandle);
        }

        if (texcoordHandle >= 0) {
            glVertexAttribPointer(texcoordHandle, m_texCoordSize, GL_FLOAT, GL_FALSE, vertexSizeInBytes, (GLvoid*)m_texCoordOffset);
            glEnableVertexAttribArray(texcoordHandle);
        }

        if (tangentHandle >= 0) {
            glVertexAttribPointer(tangentHandle, m_tangentSize, GL_FLOAT, GL_FALSE, vertexSizeInBytes, (GLvoid*)m_tangentOffset);
            glEnableVertexAttribArray(tangentHandle);
        }

		if (instanceCount >= 1 && (NvGLInstancingSupport::glDrawElementsInstancedInternal != nullptr))
        {
            NvGLInstancingSupport::glDrawElementsInstancedInternal(GL_TRIANGLES, m_indexCount, GL_UNSIGNED_INT, nullptr, instanceCount);
        }
        else
        {
            glDrawElements(GL_TRIANGLES, m_indexCount, GL_UNSIGNED_INT, 0);
        }

        // Unbind our vertex/index buffers
        glDisableVertexAttribArray(positionHandle);
        if (normalHandle >= 0)
            glDisableVertexAttribArray(normalHandle);
        if (texcoordHandle >= 0)
            glDisableVertexAttribArray(texcoordHandle);
        if (tangentHandle >= 0)
            glDisableVertexAttribArray(tangentHandle);

        // Unbind our vertex/index buffers
        glBindBuffer(GL_ARRAY_BUFFER, 0);
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
	}

	void NvMeshExtGL::Clear()
    {
        if (m_vboID != 0)
        {
            glDeleteBuffers(1, &m_vboID);
            m_vboID = 0;
        }

        if (m_iboID != 0)
        {
            glDeleteBuffers(1, &m_iboID);
            m_iboID = 0;
        }

        m_indexCount = 0;
        m_vertexCount = 0;
        m_vertexSize = 0;
        m_positionSize = 0;
        m_positionOffset = 0;
        m_normalSize = 0;
        m_normalOffset = -1;
        m_texCoordSize = 0;
        m_texCoordOffset = -1;
        m_tangentSize = 0;
        m_tangentOffset = -1;
    }
}