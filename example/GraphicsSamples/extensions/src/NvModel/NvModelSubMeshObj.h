//----------------------------------------------------------------------------------
// File:        NvModel/NvModelSubMeshObj.h
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
#ifndef _NVMODELSUBMESHOBJ_H_
#define _NVMODELSUBMESHOBJ_H_
#pragma once

#include "NvModel/NvModelSubMesh.h"
#include "NvModelMeshFace.h"
#include "NvModelMeshVertex.h"
#include <vector>
#include <map>

namespace Nv
{
    // Used to define a mesh that uses a single material and an array of faces
    class SubMeshObj : public SubMesh
    {
    public:
        typedef std::vector<MeshVertex> MeshVertexArray;
        typedef std::pair<MeshVertex, int32_t> MeshVertexMapPair;
        typedef std::map<MeshVertex, int32_t, MeshVertexCompare> MeshVertexMap;

        SubMeshObj()
        { 
            m_rawFaces.reserve(100); 
            m_srcVertices.reserve(100); 
        }

        /// Checks to see if the submesh's vertices contain normals
        /// \return True if the submesh's vertices contain normals, 
        ///         false if no normals exist.
        virtual bool HasNormals()
        {
            if (m_rawFaces.empty())
                return false;
			return m_srcVertices[0].m_normal != -1;
        }

        /// Checks to see if the submesh's vertices contain texture coordinates
        /// \return True if the submesh's vertices contain texture coordinates, 
        ///         false if no texture coordinates exist.
		virtual bool HasTexCoords()
        {
            if (m_rawFaces.empty())
                return false;
			return m_srcVertices[0].m_texcoord != -1;
        }

        /// Checks to see if the submesh's vertices contain tangents
        /// \return True if the submesh's vertices contain tangents, 
        ///         false if no tangents exist.
		virtual bool HasTangents()
        {
            if (m_rawFaces.empty())
                return false;
			return m_srcVertices[0].m_tangent != -1;
        }

        /// Checks to see if the submesh's vertices contain colors
        /// \return False, as we don't support vertex colors in obj files
        virtual bool HasColors() { return false; }

        /// Checks to see if the submesh's vertices contain bone weights
        /// \return False, as we don't support bones in obj files
        virtual bool HasBoneWeights() { return false; }
        
        /// Retrieves an index of a vertex matching the given one, adding a new one
        /// in the case of no existing match.
        /// \param[in] v Vertex to retrieve an index for
        /// \return Index into the submesh's array of vertices that contains a vertex
        ///         matching the given one, -1 if one did not exist and could not be added.
        int32_t FindOrAddVertex(MeshVertex& v);

        /// Sets the normal to be used by the given vertex.  If that normal
        /// conflicts with one that is already stored in the vertex, this will 
        /// split the vertex.  In this case, it will return the index of the
        /// newly created vertex to be used instead of the original index.
        /// \param[in] vertexId Index of the vertex to set the normal for
        /// \param[in] normalId Index of the normal to associate with the vertex
        /// \return The index of the vertex that has been associated with the 
        ///         given normal.  This will be the original vertex index if
        ///         no existing normal was present.  If setting the normal 
        ///         caused the vertex to split, then this will be the index of
        ///         the new vertex that was created.
        int32_t SetNormal(int32_t vertexId, int32_t normalId);

        /// Sets the tangent vector to be used by the given vertex.  If that 
        /// tangent vector conflicts with one that is already set, this will
        /// split the vertex.  In this case, it will return the index of the
        /// newly created vertex to be used instead of the original index.
        /// \param[in] vertexId Index of the vertex to set the tangent vector for
        /// \param[in] tangentId Index of the tangent vector to associate with 
        ///             the vertex
        /// \return The index of the vertex that has been associated with the 
        ///         given tangent vector.  This will be the original vertex
        ///         index if no existing normal was present.  If setting the 
        ///         tangent vector caused the vertex to split, then this will
        ///         be the index of the new vertex that was created.
        int32_t SetTangent(int32_t vertexId, int32_t tangentId);

        // Array of face definitions that define the submesh
        std::vector<MeshFace> m_rawFaces;

    public:
        // Compacted array of vertices used to define the submesh
        MeshVertexArray m_srcVertices;

        // Map from each vertex to its index in the flat array of vertices
        MeshVertexMap m_vertexMap;
};
}
#endif