//----------------------------------------------------------------------------------
// File:        NvModel/NvModelMeshVertex.h
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
#ifndef _NVMODELMESHVERTEX_H_
#define _NVMODELMESHVERTEX_H_
#pragma once
#include "NV/NvMath.h"

namespace Nv
{
    // Defines a vertex within a mesh via indices into
    // the large, shared arrays of individual elements
    class MeshVertex
    {
    public:
        MeshVertex() : m_pos(-1), m_normal(-1), m_texcoord(-1), m_tangent(-1) {}

        int32_t m_pos;      // Index into the shared positions array
        int32_t m_normal;   // Index into the shared normals array
        int32_t m_texcoord; // Index into the shared texture coordinates array
        int32_t m_tangent;  // Index into the shared tangent array
    };

    struct MeshVertexCompare
    {
        bool operator()(const MeshVertex& lhs, const MeshVertex& rhs) const
        {
            if (lhs.m_pos < rhs.m_pos)
            {
                return true;
            }
            if (lhs.m_pos > rhs.m_pos)
            {
                return false;
            }
            if (lhs.m_normal < rhs.m_normal)
            {
                return true;
            }
            if (lhs.m_normal > rhs.m_normal)
            {
                return false;
            }
            if (lhs.m_texcoord < rhs.m_texcoord)
            {
                return true;
            }
            if (lhs.m_texcoord > rhs.m_texcoord)
            {
                return false;
            }
            return (lhs.m_tangent < rhs.m_tangent);
        };
    };
}
#endif
