//----------------------------------------------------------------------------------
// File:        es3aep-kepler\ThreadedRenderingGL/NvSharedVBOGL_Pooled.h
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
#ifndef NVSHAREDVBO_GL_POOLED_H_
#define NVSHAREDVBO_GL_POOLED_H_

#include "NvSharedVBOGL.h"

namespace Nv
{
	/// \file
	/// SharedVBO that implements a single, large VBO that is mapped at the 
    /// beginning of each frame so that sub-ranges within the VBO may be written to.
	class NvSharedVBOGL_Pooled : public NvSharedVBOGLPool
	{
	public:
        NvSharedVBOGL_Pooled();
        virtual ~NvSharedVBOGL_Pooled(){}

        /// Initializes the large, shared VBO wrapper and sets its size
        /// \param dataSize Size, in bytes, of each sub-range that will be
        ///                 contained withing the buffer object
        /// \param numSubRanges Number of sub-ranges that will be contained 
        ///                     within the shared VBO
        /// \param numFrames Number of frames worth of data contained in the VBO, which will
        ///                  act as a ring buffer, using the next set of sub-ranges for each frame
        /// \param bPersistent Flag to indicate whether the VBO should be persistently mapped
        /// \return True if the buffer was initialized, false if an error occurred
        virtual bool Initialize(uint32_t dataSize, uint32_t numSubRanges, uint32_t numFrames, bool bPersistent = false);

        /// Sets the current number of sub-ranges to contain in the shared VBO
        /// \param numSubRanges Number of sub-ranges that will be held within the VBO
        /// \note Must not be called between calls to BeginUpdate()/EndUpdate().
        virtual void SetNumSubRanges(uint32_t numSubRanges);

        /// Sets the size of each sub-range contained in the shared VBO
        /// \param subRangeSize Size, in bytes, of each sub-range that will be held within the VBO
        /// \note Must not be called between calls to BeginUpdate()/EndUpdate().
        virtual void SetSubRangeSize(uint32_t subRangeSize);

        /// Sets the number of frames worth of data contained in the shared VBO
        /// \param numFrames number of frames worth of data that will be held within the VBO
        /// \note Must not be called between calls to BeginUpdate()/EndUpdate().
        virtual void SetNumFrames(uint32_t numFrames);

		/// Waits for the queue that the shared VBO is bound to to finish
        virtual void Finish();

        /// Updates our internal state to allow write access to the VBO
        /// \return True if the update could begin
        virtual bool BeginUpdate();

        /// Unmaps our VBO and clears the data pointer if necessary
        virtual void EndUpdate();

        /// Notifies the VBO that we are done rendering for this frame
        virtual void DoneRendering();

		/// Returns the pointer to the currently active buffer in the shared VBO
		/// \return Pointer to the current writable range of the vertex buffer
        virtual uint8_t* GetData();

        /// Returns the pointer to the area within the active buffer in 
        /// the shared VBO that corresponds to the given sub-range
        /// \param subRangeIndex Index of the sub-range for which to return a pointer
        /// \return Pointer to the current writeable range of the vertex buffer
        virtual uint8_t* GetData(uint32_t subRangeIndex);

		/// Returns the size of the vertex buffer in the shared VBO
		/// \return Number of bytes in the shared VBO
        virtual uint32_t GetDataSize() const { return m_dataSize; }

        /// Returns the offset to the currently active section of the VBO
        /// \return Offset, in bytes, from the beginning of the VBO to the currently
        ///         active frame's section
        virtual uint32_t GetDynamicOffset() { return (m_index * m_dataSize); }

        /// Returns the offset into the shared VBO of the sub-range with the 
        /// provided index
        /// \param subRangeIndex Index of the sub-range for which to retrieve the offset
        /// \return Offset, in bytes, from the start of the vertex buffer at which the
        ///         sub-range with the given index begins.
        virtual uint32_t GetDynamicOffset(uint32_t subRangeIndex) { return GetDynamicOffset() + (subRangeIndex * m_subRangeSize); }

        /// Returns the GL "Name" of the currently active VBO
        virtual GLuint GetBuffer() { return m_vbo; }

	private:
        bool InitBuffers();

		GLuint		m_vbo;
		uint8_t*    m_vboData;
        uint32_t    m_numSubRanges;
        uint32_t    m_subRangeSize;
        uint32_t    m_index;
        uint32_t    m_numFrames;
        uint32_t    m_bufferSize;

        bool        m_bResizeBuffer;
        bool        m_bPersistent;
	};

    /// \file
    /// SharedVBO that implements a VBO interface to address a portion of an
    /// NvSharedVBOGL_Pooled object as if it were the full VBO.
    class NvSharedVBOGL_PooledSubRange : public NvSharedVBOGL
    {
    public:
        NvSharedVBOGL_PooledSubRange() 
            : NvSharedVBOGL() 
            , m_pVBOPool(nullptr)
            , m_vboData(nullptr)
            , m_subRangeIndex(0)
        {}
        virtual ~NvSharedVBOGL_PooledSubRange(){}

        /// Initializes the VBO wrapper and sets its size
        /// \param dataSize Size, in bytes, of the buffer sub-range
        /// \param numBuffers Unused for this implementation, as the buffer
        ///                   management actually happens in the Pool.
        /// \param bPersistent Flag to indicate whether the VBO should be persistently mapped
        /// \return True if the buffer was initialized, false if an error occurred
        virtual bool Initialize(uint32_t dataSize, uint32_t numBuffers, bool bPersistent)
        {
            m_dataSize = dataSize;
            numBuffers;
            bPersistent; // Unused as we effectively inherit the behavior of our pool VBO
            return true;
        }
        virtual bool SetPool(NvSharedVBOGLPool* pVBOPool, uint32_t subRangeIndex)
        {
            m_pVBOPool = pVBOPool;
            m_subRangeIndex = subRangeIndex;
            return true;
        }

        virtual void Finish() {}

        /// Updates our data pointer and ensures that the available data
        /// pointer can be modified without affecting any in-flight rendering
        /// \return True if the update could begin
        virtual bool BeginUpdate()
        {
            // Get a pointer to the portion of the currently mapped buffer
            // that is to be used for this sub-range
            m_vboData = m_pVBOPool->GetData(m_subRangeIndex);
            return (m_vboData != nullptr);
        }

        /// No unmapping is needed for a pooled sub-range.  We just clear the
        /// pointer to our mapped range.
        virtual void EndUpdate() { m_vboData = nullptr; }

        /// No fencing or other synchronization is required for the sub-range.
        virtual void DoneRendering() {}

        /// Returns the pointer to the currently active buffer in the shared VBO
        /// \return Pointer to the current writeable range of the vertex buffer
        virtual uint8_t* GetData() { return m_vboData; }

        /// Returns the size of the vertex buffer sub-range
        /// \return Number of bytes contained in the sub-range of the vertex buffer
        virtual uint32_t GetDataSize() const { return m_dataSize; }

        /// Returns the offset, in bytes, of the sub-range from the beginning of the
        /// shared buffer.
        /// \return Number of bytes from the beginning of the VBO at which the
        ///         sub-range begins
        virtual uint32_t GetDynamicOffset() 
        { 
            if (nullptr == m_pVBOPool)
            {
                return 0;
            }
            return m_pVBOPool->GetDynamicOffset(m_subRangeIndex);
        }

        /// Returns the GL "Name" of the currently active VBO
        virtual GLuint GetBuffer() 
        {
            if (nullptr == m_pVBOPool)
            {
                return 0;
            }
            return m_pVBOPool->GetBuffer();
        }

    private:
        NvSharedVBOGLPool* m_pVBOPool; // Pointer to the VBO that the sub-range is part of
        uint8_t*           m_vboData;
        uint32_t           m_subRangeIndex;
    };

}
#endif // NVSHAREDVBO_GL_POOLED_H_
