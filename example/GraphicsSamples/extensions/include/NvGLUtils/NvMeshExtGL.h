//----------------------------------------------------------------------------------
// File:        NvGLUtils/NvMeshExtGL.h
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
#ifndef NVGLMESHEXT_H_
#define NVGLMESHEXT_H_
#pragma once

#include "NV/NvPlatformGL.h"
#include "KHR/khrplatform.h"

#include "NV/NvMath.h"
#include <vector>

namespace Nv
{
    class NvModelExt;
	class NvSkeleton;
	class SubMesh;

    // Temporary hack to allow for instanced mesh rendering in GLUtils library
    class NvGLInstancingSupport {
    public:
        typedef void (KHRONOS_APIENTRY *PFNDrawElementsInstanced)(GLenum mode, GLsizei count, GLenum type, const void* indices, GLsizei primcount);
        typedef void (KHRONOS_APIENTRY *PFNVertexAttribDivisor)(GLuint index, GLuint divisor);

        static PFNDrawElementsInstanced glDrawElementsInstancedInternal;
        static PFNVertexAttribDivisor glVertexAttribDivisorInternal;
    };

    /// \file
    /// OpenGL geometric mesh data handling and rendering
	class NvMeshExtGL
    {
    public:
		NvMeshExtGL();
		NvMeshExtGL(const NvMeshExtGL& other);
		~NvMeshExtGL();

        /// Returns the ID of the material used by this mesh.  The apps can map this to
		/// actual material info with #NvModelExtGL::GetMaterial
		/// \return index of the current mesh's material in the overall model's material list
		uint32_t GetMaterialID() const { return m_materialID; }

		/// \privatesection
		// Initialize mesh data from the given sub-mesh in the model
		bool InitFromSubmesh(NvModelExt* pModel, uint32_t subMeshID);// , NvMutex* mutex = nullptr);

        const SubMesh* GetSubMesh() const { return m_pSrcMesh; }

		void DrawElements(uint32_t instanceCount, GLint positionHandle, GLint normalHandle = -1, GLint texcoordHandle = -1, GLint tangentHandle = -1);

		/// Returns the model-relative transform of the mesh
		const nv::matrix4f& GetMeshOffset() const { return m_offsetMatrix; }

        /// Copy the current transforms from bones in the given skeleton
		/// to the internal bone array in preparation for rendering
		/// \param pSrcSkel Pointer to the skeleton containing the current
		///                 transforms for all bones used by the mesh
		/// \return True if the mesh's transforms could be updated from 
		///         the given skeleton, false if they could not.
        bool UpdateBoneTransforms(Nv::NvSkeleton* pSrcSkel);

        // Draw the mesh, using the given handles for vertex elements

        // Returns a bool indicating whether or not the mesh's vertices contain normals
        bool HasNormals() const { return (m_normalOffset >= 0); }

        // Returns a bool indicating whether or not the mesh's vertices contain texture coordinates
        bool HasTexCoords() const { return (m_texCoordOffset >= 0); }

        // Returns a bool indicating whether or not the mesh's vertices contain tangent vectors
        bool HasTangents() const { return (m_tangentOffset >= 0); }

        // Mesh Format Inspection Accessors (for advanced rendering)
        
        /// Get the index count
        /// \return the index count
        uint32_t GetIndexCount() const { return m_indexCount; }    
        
        /// Get the vertex count
        /// \return the vertex count
        uint32_t GetVertexCount() const { return m_vertexCount; }

        /// Get the size of each vertex, in number of floats
        /// \return The size of the vertex in number of floats
        uint32_t GetVertexSize() const { return m_vertexSize; }

        /// Get the size of the Position component of the vertices
        /// \return The size of the Position component in number of floats.
        ///         A value of indicates that the vertices do not contain Position data
        int32_t GetPositionSize() const { return m_positionSize; }

        /// Get the offset, in bytes, of the Position component within the
        /// vertex format.
        /// \return The offset, in bytes, of the Position component within
        ///         the vertex format.
        int32_t GetPositionOffset() const { return m_positionOffset; }

        /// Get the size of the Normal component of the vertices
        /// \return The size of the Normal component in number of floats.
        ///         A value of indicates that the vertices do not contain Normal data
        int32_t GetNormalSize() const { return m_normalSize; }

        /// Get the offset, in bytes, of the Normal component within the
        /// vertex format.
        /// \return The offset, in bytes, of the Normal component within
        ///         the vertex format.
        int32_t GetNormalOffset() const { return m_normalOffset; }

        /// Get the size of the texture coordinate component of the vertices
        /// \return The size of the texture coordinate component in number of floats.
        ///         A value of indicates that the vertices do not contain texture coordinate data
        int32_t GetTexCoordSize() const { return m_texCoordSize; }

        /// Get the offset, in bytes, of the texture coordinate component within the
        /// vertex format.
        /// \return The offset, in bytes, of the texture coordinate component within
        ///         the vertex format.
        int32_t GetTexCoordOffset() const { return m_texCoordOffset; }

        /// Get the size of the Tangent component of the vertices
        /// \return The size of the Tangent component in number of floats.
        ///         A value of indicates that the vertices do not contain Tangent data
        int32_t GetTangentSize() const { return m_tangentSize; }

        /// Get the offset, in bytes, of the Tangent component within the
        /// vertex format.
        /// \return The offset, in bytes, of the Tangent component within
        ///         the vertex format.
        int32_t GetTangentOffset() const { return m_tangentOffset; }

        /// Get the size of the Color component of the vertices
        /// \return The size of the Color component in number of floats.
        ///         A value of indicates that the vertices do not contain Color data
        int32_t GetColorSize() const { return m_colorSize; }

        /// Get the offset, in bytes, of the Color component within the
        /// vertex format.
        /// \return The offset, in bytes, of the Color component within
        ///         the vertex format.
        int32_t GetColorOffset() const { return m_colorOffset; }

    private:
		/// \privatesection
		NvMeshExtGL &operator =(const NvMeshExtGL &) { return *this; }

		void Clear();

		SubMesh* m_pSrcMesh;

        // Index of the material used by this mesh
        uint32_t m_materialID;

        // GL IDs of the buffer objects
        GLuint m_vboID;
        GLuint m_iboID;
        GLsizei m_indexCount;

        uint32_t m_vertexCount;
        int32_t m_vertexSize;       // in floats

        int32_t m_positionSize;     // in floats
        intptr_t m_positionOffset;   // in bytes

        int32_t m_normalSize;       // in floats
		intptr_t m_normalOffset;     // in bytes

        int32_t m_texCoordSize;     // in floats
		intptr_t m_texCoordOffset;   // in bytes

        int32_t m_tangentSize;      // in floats
		intptr_t m_tangentOffset;    // in bytes

		int32_t m_colorSize;        // in floats
		intptr_t m_colorOffset;      // in bytes

        int32_t m_parentNode;
		nv::matrix4f m_offsetMatrix;
		uint32_t m_vertexAttribCount;
	};
}
#endif
