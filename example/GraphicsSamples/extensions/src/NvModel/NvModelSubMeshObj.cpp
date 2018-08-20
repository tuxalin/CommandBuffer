//----------------------------------------------------------------------------------
// File:        NvModel/NvModelSubMeshObj.cpp
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
#include "NvModelSubMeshObj.h"
#include "NV/NvMath.h"

namespace Nv
{
    int32_t SubMeshObj::FindOrAddVertex(MeshVertex& v)
    {
        // See if a vertex already exists with the same components
        MeshVertexMap::iterator vertIt = m_vertexMap.find(v);

        if (vertIt != m_vertexMap.end())
        {
            // Found an existing one.
            return vertIt->second;
        }
        // We need to add the vertex to our collection of vertices and
        // add it to the appropriate spot in the map
        int32_t vertexId = int32_t(m_srcVertices.size());
        m_srcVertices.push_back(v);

        // Add an entry to the map so that we can find this one if 
        // another matching vertex should be added
        m_vertexMap.insert(MeshVertexMapPair(v, vertexId));
        return vertexId;
    }

	int32_t SubMeshObj::SetNormal(int32_t vertexId, int32_t normalId)
    {
        if ((vertexId < 0) || (vertexId >= (int32_t)(m_srcVertices.size())))
        {
            // Invalid Vertex Id
            return -1;
        }

        MeshVertex vert = m_srcVertices[vertexId];
        if (normalId == vert.m_normal)
        {
            // The vertex's normal has already been set to use the given normal
            return vertexId;
        }

        if (-1 == vert.m_normal)
        {
            // Vertex's normal hasn't been set yet.  Remove the old
            // version from the map so that we can re-add it with the 
            // correct normal
            m_vertexMap.erase(vert);
            vert.m_normal = normalId;
            m_srcVertices[vertexId] = vert;
            m_vertexMap.insert(MeshVertexMapPair(vert, vertexId));
            return vertexId;
        }

        vert.m_normal = normalId;

        // See if a vertex already exists with the new normal setting
        MeshVertexMap::iterator vertIt = m_vertexMap.find(vert);

        if (vertIt != m_vertexMap.end())
        {
            // Found an existing one.
            return vertIt->second;
        }

        // Vertex's normal has already been set to a different normal.  We need to
        // split this vertex by creating a new vertex with the different normal
        // and add it to both the set of vertices and the map to them.
        vertexId = int32_t(m_srcVertices.size());
        m_srcVertices.push_back(vert);
        m_vertexMap.insert(MeshVertexMapPair(vert, vertexId));
        return vertexId;
    }

	int32_t SubMeshObj::SetTangent(int32_t vertexId, int32_t tangentId)
    {
        if ((vertexId < 0) || (vertexId >= (int32_t)(m_srcVertices.size())))
        {
            // Invalid Vertex Id
            return -1;
        }

        MeshVertex vert = m_srcVertices[vertexId];
        if (tangentId == vert.m_tangent)
        {
            // The vertex's tangent has already been set to use the given tangent
            return vertexId;
        }

        if (-1 == vert.m_tangent)
        {
            // Vertex's tangent hasn't been set yet.  Remove the old
            // version from the map so that we can re-add it with the 
            // correct tangent
            m_vertexMap.erase(vert);
            vert.m_tangent = tangentId;
            m_srcVertices[vertexId] = vert;
            m_vertexMap.insert(MeshVertexMapPair(vert, vertexId));
            return vertexId;
        }
        vert.m_tangent = tangentId;

        // See if a vertex already exists with the new tangent setting
        MeshVertexMap::iterator vertIt = m_vertexMap.find(vert);

        if (vertIt != m_vertexMap.end())
        {
            // Found an existing one.
            return vertIt->second;
        }

        // Vertex's tangent has already been set to a different tangent.  We need to
        // split this vertex by creating a new vertex with the different tangent
        // and add it to both the set of vertices and the map to them.
        vertexId = int32_t(m_srcVertices.size());
        m_srcVertices.push_back(vert);
        m_vertexMap.insert(MeshVertexMapPair(vert, vertexId));
        return vertexId;
    }
}