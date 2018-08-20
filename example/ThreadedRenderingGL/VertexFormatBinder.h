//----------------------------------------------------------------------------------
// File:        es3aep-kepler\ThreadedRenderingGL/VertexFormatBinder.h
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
#ifndef VertexFormatBinder_H_
#define VertexFormatBinder_H_
#include "NV/NvPlatformGL.h"
#include "NvSharedVBOGL.h"
#include <map>

namespace Nv
{
	/// \file
	/// GL-specific set of attributes and their descriptors to encapsulate setting
    /// up a particular vertex format for rendering
	class VertexFormatBinder
	{
	public:
        VertexFormatBinder() : m_stride(0) {}

        ~VertexFormatBinder()
		{
		}

        /// Sets the size of each block of vertex data in the buffer
        /// \param stride Size, in bytes, of each block of vertex data
        void SetStride(uint32_t stride) { m_stride = stride; }

        /// Gets the size of each block of vertex data
        /// \return Size, in bytes, of each block of vertex data
        uint32_t GetStride() const { return m_stride; }

		/// Adds a single attribute description to the set of vertex data attributes
		/// that describe the data stream that will be used with this binder.
		/// \param attribIndex Location in the vertex format for this attribute
		/// \param size Size, in elements, of the attribute (e.g. 4 for a vec4)
		/// \param type Type, such as GL_FLOAT of the elements that comprise the attribute
        /// \param usage One of the GL types (GL_FLOAT, GL_INT, GL_DOUBLE) that indicates the 
        ///              way that the data will be used in the shader.  For GL_FLOAT, attributes
        ///              of a non-float Type will be converted to a float.  For GL_INT, any 
        ///              integer types (GL_INT, GL_UNSIGNED_INT, etc.) will remain integers.  For
        ///              GL_DOUBLE, the type must also be GL_DOUBLE and no conversion is done.
		/// \param normalized Boolean flag indicating whether the attribute should be normalized
		/// \param divisor Number of instances of the model that should be rendered with each
		///                value of this attribute before incrementing to the next value
		/// \param offset Starting offset, in bytes, of this attribute from the beginning of 
		///               the instance data structure
        /// \return True if the instance attribute could be added.  False if something was wrong
        ///         with the parameters and it could not be added.
        bool AddInstanceAttrib(GLuint attribIndex,
            GLint size,
            GLenum type,
            GLenum usage,
            GLboolean normalized,
            GLuint divisor,
            GLuint offset);

        /// Removes the instancing vertex attribute at the given index
		/// \param attribIndex Index of the attribute to remove from the vertex definition
        void RemoveInstanceAttrib(GLuint attribIndex);

        /// Activates and sets up the vertex attribute arrays and their pointers and divisors
        /// in preparation for using an associated buffer to render.
        /// \param pVertexDataStream Pointer to the Shared VBO object containing the vertex
        ///                          data to use
        void Activate(NvSharedVBOGL* pVertexDataStream);

        /// The same as Activate(), but assumes the attribute array
        /// is already enabled and the divisor is set so that only
        /// the pointer needs to be applied.
        /// \param pVertexDataStream Pointer to the Shared VBO object containing the vertex
        ///                          data to use
        /// \param batchOffset An offset in bytes from the beginning of the VBO stream to 
        ///                    use as the start of the active VBO
        void UpdatePointers(NvSharedVBOGL* pVertexDataStream, uint32_t batchOffset);

        /// Clear out the state set by the Binder's Activate call
        void Deactivate();

	private:
        // See the AddInstanceAttrib call for explanations of
        // the members of this structure
        struct InstanceAttribDesc
        {
            GLuint      m_attribIndex;
            GLint       m_size;
            GLenum      m_type;
            GLenum      m_usage;
            GLboolean   m_normalized;
            GLuint      m_divisor;
            GLuint      m_offset;
        };

        // Map of all attribute definitions associated with this binder, indexed 
        // by their attribIndex
        typedef std::map<uint32_t, InstanceAttribDesc> AttribSet;
        AttribSet m_instanceAttributes;

        // Total size of a single vertex
        GLsizei m_stride;
	};
}
#endif // NvInstancedModelExtGL_VK_H_
