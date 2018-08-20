//----------------------------------------------------------------------------------
// File:        NvModel/NvModelSubMesh.h
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
#ifndef _NVMODELSUBMESH_H_
#define _NVMODELSUBMESH_H_
#include <NvSimpleTypes.h>
#include <vector>
#include "NV/NvMath.h"

namespace Nv
{
    // Used to define a mesh that uses a single material and an array of faces
    class SubMesh
    {
    public:
        SubMesh() 
            : m_materialId(0)
            , m_parentBone(-1)
            , m_vertices(NULL)
            , m_indices(NULL)
            , m_vertexCount(0)
            , m_indexCount(0)
            , m_normalOffset(-1)
            , m_texCoordOffset(-1)
            , m_texCoordCount(0)
            , m_tangentOffset(-1)
            , m_colorCount(0)
            , m_colorOffset(-1)
            , m_bonesPerVertex(0)
            , m_boneIndexOffset(-1)
            , m_boneWeightOffset(-1)
            , m_vertSize(0)
        {
        }

        /// Checks to see if the submesh's vertices contain normals
        /// \return True if the submesh's vertices contain normals, 
        ///         false if no normals exist.
		virtual bool HasNormals() = 0;

        /// Checks to see if the submesh's vertices contain texture coordinates
        /// \return True if the submesh's vertices contain texture coordinates, 
        ///         false if no texture coordinates exist.
		virtual bool HasTexCoords() = 0;

        /// Checks to see if the submesh's vertices contain tangents
        /// \return True if the submesh's vertices contain tangents, 
        ///         false if no tangents exist.
		virtual bool HasTangents() = 0;

        /// Checks to see if the submesh's vertices contain colors
        /// \return True if the submesh's vertices contain colors, 
        ///         false if no colors exist.
        virtual bool HasColors() = 0;
        
        /// Checks to see if the submesh's vertices contain bone weights
        /// \return True if the submesh's vertices contain bone weights, 
        ///         false if no bone weights exist.
        virtual bool HasBoneWeights() = 0;

        /// Get the array of compiled vertices.
		/// The array of the optimized, compiled vertices for rendering
		/// \return the pointer to the start of the first vertex
		virtual const float* getVertices() const { return m_vertices; }

		/// Get the array of compiled indices
		/// \return pointer to the array of indices
		virtual uint32_t* getIndices() const { return m_indices; }

		///@{
		/// Get the offset within the vertex of each attrib.
		/// \return the offset (in number of floats) of each attrib from the base of the vertex
		int32_t getPositionOffset() const { return 0; }
		int32_t getNormalOffset() const { return m_normalOffset; }
        int32_t getTexCoordOffset() const { return m_texCoordOffset; }
		int32_t getTangentOffset() const { return m_tangentOffset; }
        int32_t getColorOffset() const { return m_colorOffset; }
        int32_t getBoneIndexOffset() const { return m_boneIndexOffset; }
        int32_t getBoneWeightOffset() const { return m_boneWeightOffset; }
		///@}
        int32_t getTexCoordCount() const { return m_texCoordCount; }
        int32_t getColorCount() const { return m_colorCount; }
        int32_t getBonesPerVertex() const { return m_bonesPerVertex; }
        int32_t getNumBonesInMesh() const { return m_boneMap.size(); }

		/// Get the size of a compiled vertex.
		/// \return the size of the merged vertex (in number of floats)
		virtual int32_t getVertexSize() const { return m_vertSize; }

		/// Get the count of vertices in the compiled array.
		/// \return the vertex count in the compiled (renderable) array
		virtual int32_t getVertexCount() const { return m_vertexCount; }

		/// The rendering index count.
		/// \return the number of indices in the given array
		virtual int32_t getIndexCount() const { return m_indexCount; }


        // Material Id used by the sub mesh
        uint32_t m_materialId;

        // Mapping of bones used by the mesh to the full set of bones contained in the model
        // Local indices are contiguous and the array value at that index contains the index
        // of the full set that contains the actual bone node
        std::vector<int32_t> m_boneMap;

        // Index of the node that is the parent of this sub-mesh
        int32_t m_parentBone;

        // Matching array of transforms that map from mesh space to bone space
        std::vector<nv::matrix4f> m_meshToBoneTransforms;

		float* m_vertices;
		uint32_t* m_indices;

		uint32_t m_vertexCount;
		uint32_t m_indexCount;

		int32_t m_normalOffset; // in floats (zero == no component)
		int32_t m_texCoordOffset; // in floats (zero == no component)
        int32_t m_texCoordCount;
		int32_t m_tangentOffset; // in floats (zero == no component)
        int32_t m_colorCount;
        int32_t m_colorOffset; // in floats (zero == no component)
        int32_t m_bonesPerVertex;
        int32_t m_boneIndexOffset;   // in floats (zero == no component)
        int32_t m_boneWeightOffset; //  in floats (zero == no component)
		int32_t m_vertSize; // in floats
	};
}
#endif