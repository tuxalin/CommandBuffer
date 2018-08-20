//----------------------------------------------------------------------------------
// File:        es3aep-kepler\ThreadedRenderingGL/NvSharedVBOGL_MappedSubRanges.h
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
#ifndef NVSHAREDVBO_GL_MAPPEDSUBRANGES_H_
#define NVSHAREDVBO_GL_MAPPEDSUBRANGES_H_

#include "NvSharedVBOGL.h"

namespace Nv
{
	/// \file
	/// SharedVBO that implements a ring-buffer of VBOs to be synchronized with
    /// GPU frames so that a data pointer may be retrieved each frame and used
    /// to fill in the vertex buffer for that frame's rendering. The shared VBO 
    /// may have n buffers, allowing up to n frames worth of updates before 
    /// attempting to update the buffer will need to wait until the GPU has 
    /// finished consuming one of the buffers.
	class NvSharedVBOGL_MappedSubRanges : public NvSharedVBOGL
	{
	public:
        NvSharedVBOGL_MappedSubRanges();
        virtual ~NvSharedVBOGL_MappedSubRanges(){}

        /// Initializes the VBO wrapper and sets its size
        /// \param dataSize Size, in bytes, of the buffer object
        /// \param numBuffers Number of copies of the buffer to create.  This
        ///                   is the size of the ring buffer, and the number of
        ///                   frames that can be rendered before the caller may 
        ///                   need to wait for a frame to complete before
        ///                   calling BeginUpdate().
        /// \param bPersistent Flag to indicate whether the VBO should be persistently mapped
        /// \return True if the buffer was initialized, false if an error occurred
        virtual bool Initialize(uint32_t dataSize, uint32_t numBuffers, bool bPersistent);

		/// Releases all resources associated with the VBO
        virtual void Finish();

		/// Maps our data pointer and assures us that the available data
		/// pointer can be modified without affecting any in-flight rendering
		/// \return True if the update could begin
        virtual bool BeginUpdate();

        /// Signals the VBO wrapper that the caller is done updating the data and
        /// the VBO is ready for rendering
        virtual void EndUpdate();

        /// Signals the VBO wrapper that the caller is done rendering with the VBO.
        virtual void DoneRendering();

		/// Returns the pointer to the currently active buffer in the shared VBO
		/// \return Pointer to the current writeable range of the vertex buffer
        virtual uint8_t* GetData();

		/// Returns the size of each of the vertex buffers in the shared VBO
		/// \return Number of bytes per vertex buffer in the shared VBO
        virtual uint32_t GetDataSize() const { return m_dataSize; }

        /// Returns the offset to the currently active section of the VBO
        /// \return Offset, in bytes, from the beginning of the VBO to the currently
        ///         active frame's section
        virtual uint32_t GetDynamicOffset() { return m_index * m_dataSize; }

        /// Returns the GL "Name" of the currently active VBO
        virtual GLuint GetBuffer() { return m_vbo; }

	private:
        // Index of the currently active buffer section within the larger UBO
		uint32_t    m_index;
        
        // Number of buffer sections within the larger UBO
		uint32_t    m_numBuffers;
		
        // Size, in bytes, rounded up to a multiple of 4, of the VBO buffers
		uint32_t    m_bufferSize;
        
        // GL "Name" of the VBO 
		GLuint		m_vbo;

        // Currently mapped pointer to the active buffer section's data
		uint8_t*    m_vboData;

        // Flag indicating whether the buffer section should be mapped/unmapped
        // each frame or mapped once when created and kept mapped until destroyed
        bool        m_bPersistent;
    };
}
#endif // NVSHAREDVBO_GL_MAPPEDSUBRANGES_H_
