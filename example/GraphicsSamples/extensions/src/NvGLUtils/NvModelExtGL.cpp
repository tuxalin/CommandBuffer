//----------------------------------------------------------------------------------
// File:        NvGLUtils/NvModelExtGL.cpp
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

#include "NvGLUtils/NvModelExtGL.h"
#include "NvModel/NvModelExt.h"
#include "NvGLUtils/NvImageGL.h"

bool Nv::NvModelExtGL::ms_loadTextures = true;

namespace Nv
{

	NvModelExtGL* NvModelExtGL::Create(NvModelExt* pSourceModel)
    {
        if (NULL == pSourceModel)
        {
            return NULL;
        }

		NvModelExtGL* model = new NvModelExtGL(pSourceModel);
		model->PrepareForRendering(pSourceModel);
		return model;
    }

	NvModelExtGL::NvModelExtGL(NvModelExt* pSourceModel) :
        m_pSourceModel(pSourceModel),
        m_diffuseTextureLocation(-1)
    {
        NV_ASSERT(NULL != pSourceModel);
    }

	NvModelExtGL::~NvModelExtGL()
    {
        if (m_pSourceModel)
        {
            delete m_pSourceModel;
        }
    };


    bool NvModelExtGL::UpdateBoneTransforms()
    {
        std::vector<NvMeshExtGL*>::iterator meshIt = m_meshes.begin();
        std::vector<NvMeshExtGL*>::iterator meshEnd = m_meshes.end();
        bool result = true;

        for (; meshIt != meshEnd; ++meshIt)
        {
            result &= (*meshIt)->UpdateBoneTransforms(GetModel()->GetSkeleton());
        }
        return result;
    }

	void NvModelExtGL::PrepareForRendering(NvModelExt* pModel)
    {
	    // Create GL textures of the referenced source files and keep a
		// mapping from texture index to the returned texture GL "name"
        uint32_t textureCount = m_pSourceModel->GetTextureCount();
        m_textures.resize(textureCount);
        if (ms_loadTextures) {
            for (uint32_t textureIndex = 0; textureIndex < textureCount; ++textureIndex)
            {
                m_textures[textureIndex] = NvImageGL::UploadTextureFromFile(m_pSourceModel->GetTextureName(textureIndex).c_str());
                glActiveTexture(GL_TEXTURE0);
                glBindTexture(GL_TEXTURE_2D, m_textures[textureIndex]);
                glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
                glBindTexture(GL_TEXTURE_2D, 0);
            }
        }
        else {
            for (uint32_t textureIndex = 0; textureIndex < textureCount; ++textureIndex)
                m_textures[textureIndex] = 0;
        }
 
		// Get GL usable versions of all the materials in the model
		uint32_t materialCount = pModel->GetMaterialCount();
		m_materials.resize(materialCount);
		if (materialCount > 0)
		{
			for (uint32_t materialIndex = 0; materialIndex < materialCount; ++materialIndex)
			{
				m_materials[materialIndex].InitFromMaterial(m_pSourceModel, materialIndex);
			}
		}

		// Get GL renderable versions of all meshes in the model
		uint32_t meshCount = pModel->GetMeshCount();
		m_meshes.resize(meshCount);
		if (meshCount > 0)
		{
			for (uint32_t meshIndex = 0; meshIndex < meshCount; ++meshIndex)
			{
				m_meshes[meshIndex] = new NvMeshExtGL;
				m_meshes[meshIndex]->InitFromSubmesh(pModel, meshIndex);
			}
		}
	}

	void NvModelExtGL::DrawElements(uint32_t instanceCount, GLint positionHandle, GLint normalHandle, GLint texcoordHandle, GLint tangentHandle)
    {
		uint32_t numMeshes = GetMeshCount();
		for (uint32_t meshIndex = 0; meshIndex < numMeshes; ++meshIndex)
		{
			Nv::NvMeshExtGL* pMesh = GetMesh(meshIndex);
			uint32_t matId = pMesh->GetMaterialID();
            ActivateMaterial(matId);
            pMesh->DrawElements(instanceCount, positionHandle, normalHandle, texcoordHandle, tangentHandle);
		}
    }

	// Currently we only support setting a single diffuse texture.  This needs
	// to be replaced by a general materials system for binding shader
	// parameters
    void NvModelExtGL::ActivateMaterial(uint32_t materialId)
    {
        Nv::NvMaterialGL* pMat = GetMaterial(materialId);
        if (nullptr == pMat) { return; }

        if (m_diffuseTextureLocation >= 0)
        {
            uint32_t diffuseTextureIndex = pMat->m_diffuseTexture;
            if (diffuseTextureIndex >= 0)
            {
			    // We have been given a diffuse texture location in the shader and we have 
				// a diffuse texture to use, so bind it to the given location
                GLuint textureId = GetTexture(diffuseTextureIndex);
                glActiveTexture(GL_TEXTURE0 + (GLuint)m_diffuseTextureLocation);
                glBindTexture(GL_TEXTURE_2D, textureId);
            }
        }
    }
}
