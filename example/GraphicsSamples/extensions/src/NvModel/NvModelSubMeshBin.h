//----------------------------------------------------------------------------------
// File:        NvModel/NvModelSubMeshBin.h
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
#ifndef _NVMODELSUBMESHBIN_H_
#define _NVMODELSUBMESHBIN_H_
#include <NvSimpleTypes.h>
#include <NvModel/NvModelSubMesh.h>

namespace Nv
{
    // Used to define a mesh that uses a single material and an array of faces
    class SubMeshBin : public SubMesh
    {
    public:
        SubMeshBin() { 
        }

        /// Checks to see if the submesh's vertices contain normals
        /// \return True if the submesh's vertices contain normals, 
        ///         false if no normals exist.
		virtual bool HasNormals() {
			return getNormalOffset() > 0;
		}

        /// Checks to see if the submesh's vertices contain texture coordinates
        /// \return True if the submesh's vertices contain texture coordinates, 
        ///         false if no texture coordinates exist.
		virtual bool HasTexCoords() {
			return getTexCoordOffset() > 0;
		}

        /// Checks to see if the submesh's vertices contain tangents
        /// \return True if the submesh's vertices contain tangents, 
        ///         false if no tangents exist.
		virtual bool HasTangents() {
			return getTangentOffset() > 0;
		}

        /// Checks to see if the submesh's vertices contain colors
        /// \return True if the submesh's vertices contain colors, 
        ///         false if no colors exist.
        virtual bool HasColors() {
            return getColorOffset() > 0;
        }
        
        /// Checks to see if the submesh's vertices contain bone weights
        /// \return True if the submesh's vertices contain bone weights, 
        ///         false if no bone weights exist.
        virtual bool HasBoneWeights() {
            return getBoneIndexOffset() > 0;
        }
	};
}
#endif