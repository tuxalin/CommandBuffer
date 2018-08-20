//----------------------------------------------------------------------------------
// File:        NvModel/NvModelMeshFace.cpp
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

#include "NvModel/NvModelExt.h"
#include "NvModelMeshFace.h"
#include "NvModelSubMeshObj.h"

namespace Nv
{
    void MeshFace::CalculateFaceNormal(const std::vector<nv::vec4f>& positions)
    {
        const std::vector<MeshVertex>& verts = m_pSubMesh->m_srcVertices;
        nv::vec3f edge1 = (nv::vec3f)(positions[verts[m_verts[1]].m_pos] - positions[verts[m_verts[0]].m_pos]);
        nv::vec3f edge2 = (nv::vec3f)(positions[verts[m_verts[2]].m_pos] - positions[verts[m_verts[0]].m_pos]);
        nv::vec3f faceNormal = edge1.cross(edge2);
        m_faceNormal = nv::normalize(faceNormal);
    }

    float MeshFace::GetFaceWeight_Area(const std::vector<nv::vec4f>& positions, uint32_t vertIndex) const
    {
		const std::vector<MeshVertex>& verts = m_pSubMesh->m_srcVertices;

        nv::vec3f vert = (nv::vec3f)positions[verts[m_verts[vertIndex]].m_pos];
        nv::vec3f nextVert = (nv::vec3f)positions[verts[m_verts[(vertIndex + 1) % 3]].m_pos];
        nv::vec3f prevVert = (nv::vec3f)positions[verts[m_verts[(vertIndex + 2) % 3]].m_pos];

        nv::vec3f edge1 = nextVert - vert;
        nv::vec3f edge2 = prevVert - vert;
        nv::vec3f faceNormal = edge1.cross(edge2);
        return nv::length(faceNormal);
    }

    float MeshFace::GetFaceWeight_Normalized(const std::vector<nv::vec4f>& positions, uint32_t vertIndex) const
    {
		const std::vector<MeshVertex>& verts = m_pSubMesh->m_srcVertices;

        nv::vec3f vert = (nv::vec3f)positions[verts[m_verts[vertIndex]].m_pos];
        nv::vec3f nextVert = (nv::vec3f)positions[verts[m_verts[(vertIndex + 1) % 3]].m_pos];
        nv::vec3f prevVert = (nv::vec3f)positions[verts[m_verts[(vertIndex + 2) % 3]].m_pos];

        nv::vec3f edge1 = normalize(nextVert - vert);
        nv::vec3f edge2 = normalize(prevVert - vert);
        return 1.0f - dot(edge2, edge1);
    }
}