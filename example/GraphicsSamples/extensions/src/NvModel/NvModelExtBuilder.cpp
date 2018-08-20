//----------------------------------------------------------------------------------
// File:        NvModel/NvModelExtBuilder.cpp
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
#include "NvModel/NvModelExtBuilder.h"
#include "NvModel/NvSkeleton.h"

namespace Nv
{
    NvModelExtBuilder::NvModelExtBuilder()
	{
	}

    NvModelExtBuilder::~NvModelExtBuilder()
	{
        std::vector<Material*>::iterator matIt = m_materials.begin();
        std::vector<Material*>::iterator matEnd = m_materials.end();
        for (; matIt != matEnd; ++matIt)
        {
            delete *matIt;
        }

        std::vector<SubMeshBuilder*>::iterator meshIt = m_subMeshes.begin();
        std::vector<SubMeshBuilder*>::iterator meshEnd = m_subMeshes.end();
        for (; meshIt != meshEnd; ++meshIt)
        {
            delete *meshIt;
        }
	}

    NvModelExtBuilder* NvModelExtBuilder::Create() 
    {
        NvModelExtBuilder* pModel = new NvModelExtBuilder();

		return pModel;
	}

    int32_t NvModelExtBuilder::GetTextureId(const std::string& name, bool bAdd)
    {
        uint32_t textureId;
        for (textureId = 0; textureId < m_textures.size(); ++textureId)
        {
            if (name == m_textures[textureId])
            {
                return textureId;
            }
        }
        if (bAdd)
        {
            m_textures.push_back(name);
            return textureId;
        }
        return -1;
    }

    void NvModelExtBuilder::UpdateBoundingBox()
    {
        if (m_subMeshes.empty())
        {
            m_boundingBoxMin = m_boundingBoxMax = m_boundingBoxCenter = nv::vec3f(0.0f, 0.0f, 0.0f);
            return;
        }

        m_boundingBoxMin = nv::vec3f( FLT_MAX, FLT_MAX, FLT_MAX);
        m_boundingBoxMax = nv::vec3f(-FLT_MAX,-FLT_MAX,-FLT_MAX);

        std::vector<SubMeshBuilder*>::iterator meshIt = m_subMeshes.begin();
        std::vector<SubMeshBuilder*>::iterator meshEnd = m_subMeshes.end();
        for (; meshIt != meshEnd; ++meshIt)
        {
            const SubMesh* pSubMesh = *meshIt;
            bool bTransformVerts = false;
            nv::matrix4f meshXfm;
            if ((pSubMesh->m_parentBone != -1) && (nullptr != m_pSkeleton))
            {
                const nv::matrix4f* offsetXfm = m_pSkeleton->GetTransform(pSubMesh->m_parentBone);
                meshXfm = *offsetXfm;
                bTransformVerts = true;
            }
            const float* pVerts = pSubMesh->getVertices();
            int32_t vertCount = pSubMesh->getVertexCount();
            int32_t vertSize = pSubMesh->getVertexSize();
            for (; vertCount > 0;--vertCount, pVerts += vertSize)
            {
                nv::vec4f v(pVerts[0], pVerts[1], pVerts[2], 1.0f);
                if (bTransformVerts)
                {
                    v = meshXfm * v;
                }

                for (int32_t comp = 0; comp < 3; ++comp)
                {
                    if (v[comp] < m_boundingBoxMin[comp])
                    {
                        m_boundingBoxMin[comp] = v[comp];
                    }
                    if (v[comp] > m_boundingBoxMax[comp])
                    {
                        m_boundingBoxMax[comp] = v[comp];
                    }
                }
            }
        }
        m_boundingBoxCenter = (m_boundingBoxMax + m_boundingBoxMin) * 0.5f;
    }
}
