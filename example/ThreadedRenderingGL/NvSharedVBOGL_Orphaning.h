//----------------------------------------------------------------------------------
// File:        es3aep-kepler\ThreadedRenderingGL/NvSharedVBOGL_Orphaning.h
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
#ifndef NVSHAREDVBO_GL_ORPHANING_H_
#define NVSHAREDVBO_GL_ORPHANING_H_

#include "NvSharedVBOGL.h"

namespace Nv
{
	/// \file
	/// SharedVBO that implements a single VBO which is orphaned each
    /// time a new frame begins
	class NvSharedVBOGL_Orphaning : public NvSharedVBOGL
	{
	public:
        NvSharedVBOGL_Orphaning();
        virtual ~NvSharedVBOGL_Orphaning(){}

        /// Initializes the VBO wrapper and sets its size
        /// \param dataSize Size, in bytes, of the buffer object
        /// \param numBuffers Unused for this implementation, as the buffer
        ///                   is orphaned each frame and a new one allocated.
        /// \param bPersistent Flag to indicate whether the VBO should be persistently mapped
        /// \return True if the buffer was initialized, false if an error occurred
        virtual bool Initialize(uint32_t dataSize, uint32_t numBuffers, bool bPersistent);

		/// Waits for the queue that the shared VBO is bound to to finish
        virtual void Finish();

        /// Orphans our previous VBO and requests a new one, then updates our 
        /// data pointer by mapping the new VBO
        /// \return True if the update could begin
        virtual bool BeginUpdate();

        /// Unmaps our VBO and clears the data pointer
        virtual void EndUpdate();

        /// Does nothing in this implementation
        virtual void DoneRendering();

		/// Returns the pointer to the currently active buffer
		/// \return Pointer to the current writeable memory of the vertex buffer
        virtual uint8_t* GetData() { return m_vboData; }

        /// We always use the whole VBO in this implementation, so the
        // offset is always 0
        virtual uint32_t GetDynamicOffset() { return 0; }

        /// Returns the GL "Name" of the currently active VBO
        virtual GLuint GetBuffer() { return m_vbo; }

	private:
		GLuint		m_vbo;      // GL Name of the Vertex Buffer Object
		uint8_t*    m_vboData;  // Pointer to the memory representing the buffer, 
                                // if it is currently mapped (between calls 
                                // to BeginUpdate()/EndUpdate())
	};
}
#endif // NVSHAREDVBO_GL_ORPHANING_H_
