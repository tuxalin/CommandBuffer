//----------------------------------------------------------------------------------
// File:        NvModel/NvModelMeshFace.h
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
#ifndef _NVMODELMESHFACE_H_
#define _NVMODELMESHFACE_H_
#pragma once

#include "NV/NvMath.h"
#include <vector>

namespace Nv
{
    class SubMeshObj;

    // Contains the definition of a single face in a mesh
    class MeshFace
    {
    public:
        MeshFace() : m_faceNormal(0.0f, 0.0f, 0.0f), m_smoothingGroup(0), m_material(0) {}

        // Indices into the mesh's array of vertices of those vertices the define the face
        int32_t m_verts[3];
        nv::vec3f m_faceNormal; // Calculated at load time
        uint32_t m_smoothingGroup;
        uint32_t m_material;    // Index into the materials array
        SubMeshObj* m_pSubMesh;    // Pointer back to the mesh that contains this face

        /// Calculates the face normal using the 3 vertices that make up the face
        /// \param[in] positions The array of positions used by the containing mesh's vertices
        void CalculateFaceNormal(const std::vector<nv::vec4f>& positions);

        /// Returns the weight to be used for this face when averaging normals
        /// \param[in] positions The array of positions used by the containing mesh's vertices
        /// \param[in] vertIndex Index into the face's array of positions for the vertex being 
        ///             used to generate the normal
        /// \return A value to be used to determine the face normal's contribution when averaging normals
        float GetFaceWeight(const std::vector<nv::vec4f>& positions, uint32_t vertIndex) const
        {
            return GetFaceWeight_Normalized(positions, vertIndex);
        }

        /// Returns the weight to be used for this face when averaging normals, in this case a constant
        /// so that all containing faces are equal.
        /// \param[in] positions The array of positions used by the containing mesh's vertices
        /// \param[in] vertIndex Index into the face's array of positions for the vertex being 
        ///             used to generate the normal
        /// \return A value to be used to determine the face normal's contribution when averaging normals        
        float GetFaceWeight_Constant(const std::vector<nv::vec4f>& positions, uint32_t vertIndex) const
        {
            return 1.0f;
        }

        /// Returns the weight to be used for this face when averaging normals, in this case a 
        /// value based on the total area of the triangle.
        /// \param[in] positions The array of positions used by the containing mesh's vertices
        /// \param[in] vertIndex Index into the face's array of positions for the vertex being 
        ///             used to generate the normal
        /// \return A value to be used to determine the face normal's contribution when averaging normals 
        float GetFaceWeight_Area(const std::vector<nv::vec4f>& positions, uint32_t vertIndex) const;

        /// Returns the weight to be used for this face when averaging normals, in this case a 
        /// value based upon the angle between the edges that share the vertex in question.
        /// \param[in] positions The array of positions used by the containing mesh's vertices
        /// \param[in] vertIndex Index into the face's array of positions for the vertex being 
        ///             used to generate the normal
        /// \return A value to be used to determine the face normal's contribution when averaging normals        
        float GetFaceWeight_Normalized(const std::vector<nv::vec4f>& positions, uint32_t vertIndex) const;
    };
}
#endif

