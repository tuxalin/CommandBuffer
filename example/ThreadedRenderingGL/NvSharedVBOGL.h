//----------------------------------------------------------------------------------
// File:        es3aep-kepler\ThreadedRenderingGL/NvSharedVBOGL.h
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
#ifndef NVSHAREDVBO_GL_H_
#define NVSHAREDVBO_GL_H_

#include <NvSimpleTypes.h>
#include <NV/NvPlatformGL.h>

//#include "NvAppBase/NvThread.h"

namespace Nv
{
    enum VBOPolicy
    {
        VBO_SUBRANGE,
        VBO_SUBRANGE_PERSISTENT,
        VBO_ORPHANED,
        VBO_POOLED,
        VBO_POOLED_PERSISTENT,
        VBO_INVALID
    };

	/// \file
	/// GL-specific abstraction of a Vertex Buffer object intended to be written
	/// each frame by code running on the CPU. 
	class NvSharedVBOGL
	{
	public:
        NvSharedVBOGL() : m_dataSize(0) {}
        virtual ~NvSharedVBOGL() {}

        /// Initializes the VBO wrapper and sets its size
        /// \param dataSize Size, in bytes, of the buffer object
        /// \param numBuffers Number of copies of the buffer to create.  Exact
        ///                   usage of the number of buffers is implementation
        ///                   dependent.
        /// \param bPersistent Flag to indicate whether the VBO should be persistently mapped
        /// \return True if the buffer was initialized, false if an error occurred
        virtual bool Initialize(uint32_t dataSize, uint32_t numBuffers, bool bPersistent) = 0;

		/// Waits for all resources to finish being used and the clean them up
        virtual void Finalize() { Finish(); }

		/// Flushes any outstanding operations, if necessary
        virtual void Finish() = 0;

		/// Updates our data pointer and ensures that the available data
		/// pointer can be modified without affecting any in-flight rendering
		/// \return True if the update could begin
        virtual bool BeginUpdate() = 0;

        /// Signals the VBO wrapper that the caller is done updating the data and
        /// the VBO is ready for rendering
        virtual void EndUpdate() = 0;

        /// Signals the VBO wrapper that the caller is done rendering with the VBO and may
        /// re-use the VBO using whatever method the concrete implementation requires.
        virtual void DoneRendering() = 0;

		/// Returns the pointer to the currently active buffer data in the shared VBO
		/// \return Pointer to the current writeable (possibly a sub-range of the) vertex buffer
        virtual uint8_t* GetData() = 0;

		/// Returns the size of each of the vertex buffers in the shared VBO
		/// \return Number of bytes per vertex buffer in the shared VBO
        virtual uint32_t GetDataSize() const { return m_dataSize; }

        /// Returns the current offset into the VBO to start rendering from
        virtual uint32_t GetDynamicOffset() = 0;

        /// Returns the GL "Name" of the currently active VBO
        virtual GLuint GetBuffer() = 0;

	protected:
		// Size, in bytes, of the data stored in the VBO buffers
		uint32_t    m_dataSize;
	};

    /// Interface to add to a SharedVBO that implements a single, large VBO that is 
    /// subdivided into smaller, virtual VBO objects
    class NvSharedVBOGLPool : public NvSharedVBOGL
    {
    public:
        NvSharedVBOGLPool() : NvSharedVBOGL() {}
        virtual ~NvSharedVBOGLPool(){}

        virtual bool Initialize(uint32_t dataSize, uint32_t numBuffers, bool bPersistent)
        {
            return Initialize(dataSize, numBuffers, 1, bPersistent);
        }

        /// Initializes the large, shared VBO wrapper and sets its size
        /// \param dataSize Size, in bytes, of each sub-range that will be
        ///                 contained withing the buffer object
        /// \param numSubRanges Number of sub-ranges that will be contained 
        ///                     within the shared VBO
        /// \param numFrames Number of frames worth of data contained in the VBO, which will
        ///                  act as a ring buffer, using the next set of sub-ranges for each frame
        /// \param bPersistent Flag to indicate whether the VBO should be persistently mapped
        /// \return True if the buffer was initialized, false if an error occurred
        virtual bool Initialize(uint32_t dataSize, uint32_t numSubRanges, uint32_t numFrames, bool bPersistent) = 0;

        /// Sets the current number of sub-ranges to contain in the shared VBO
        /// \param numSubRanges Number of sub-ranges that will be held within the VBO
        /// \note Must not be called between calls to BeginUpdate()/EndUpdate().
        virtual void SetNumSubRanges(uint32_t numSubRanges) = 0;

        /// Sets the size of each sub-range contained in the shared VBO
        /// \param subRangeSize Size, in bytes, of each sub-range that will be held within the VBO
        /// \note Must not be called between calls to BeginUpdate()/EndUpdate().
        virtual void SetSubRangeSize(uint32_t subRangeSize) = 0;

        /// Returns the pointer to the area within the active buffer in 
        /// the shared VBO that corresponds to the given sub-range
        /// \param subRangeIndex Index of the sub-range for which to return a pointer
        /// \return Pointer to the current writeable range of the vertex buffer
        virtual uint8_t* GetData(uint32_t subRangeIndex) = 0;

        /// Returns the offset into the shared VBO of the sub-range with the 
        /// provided index
        /// \param subRangeIndex Index of the sub-range for which to retrieve the offset
        /// \return Offset, in bytes, from the start of the vertex buffer at which the
        ///         sub-range with the given index begins.
        virtual uint32_t GetDynamicOffset(uint32_t subRangeIndex) = 0;
    };
}


#endif // NVSHAREDVBO_GL_H_
