//----------------------------------------------------------------------------------
// File:        es3aep-kepler\ThreadedRenderingGL/NvSharedVBOGL_Pooled.cpp
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
#include "NvSharedVBOGL_Pooled.h"
#include <NV/NvLogs.h>

namespace Nv
{
    NvSharedVBOGL_Pooled::NvSharedVBOGL_Pooled()
        : NvSharedVBOGLPool()
        , m_vbo(0)
        , m_vboData(nullptr)
        , m_numSubRanges(0)
        , m_subRangeSize(0)
        , m_index(0)
        , m_numFrames(0)
        , m_bufferSize(0)
        , m_bResizeBuffer(false)
        , m_bPersistent(false)
    {}

    bool NvSharedVBOGL_Pooled::Initialize(uint32_t dataSize, uint32_t numSubRanges, uint32_t numFrames, bool bPersistent)
    {
        // Use dataSize as the size of a single sub-range...
        m_subRangeSize = dataSize;

        // Account for the number of sub-ranges...
        m_numSubRanges = numSubRanges;

        // Round the buffer size up to multiples of 4 bytes
        m_dataSize = ((m_numSubRanges * m_subRangeSize) + 3) & 0xFFFFFFFC;

        // Account for the number of frames in the ring-buffer...
        m_numFrames = numFrames;
        m_bufferSize = m_dataSize * m_numFrames;

        m_bPersistent = bPersistent;
        return InitBuffers();
    }

    /// Sets the current number of sub-ranges to contain in the shared VBO
    /// \param numSubRanges Number of sub-ranges that will be held within the VBO
    /// \note Must not be called between calls to BeginUpdate()/EndUpdate().
    void NvSharedVBOGL_Pooled::SetNumSubRanges(uint32_t numSubRanges)
    {
        // After changing these values, we'll have to create a new buffer, so
        // ensure we're not in the middle of using any portion of the VBO first
        Finish();

        m_numSubRanges = numSubRanges;
        // Round the buffer size up to multiples of 4 bytes
        m_dataSize = ((m_numSubRanges * m_subRangeSize) + 3) & 0xFFFFFFFC;
        m_bufferSize = m_dataSize * m_numFrames;

        // Recreate the VBO with the new settings
        InitBuffers();
    }

    /// Sets the size of each sub-range contained in the shared VBO
    /// \param subRangeSize Size, in bytes, of each sub-range that will be held within the VBO
    /// \note Must not be called between calls to BeginUpdate()/EndUpdate().
    void NvSharedVBOGL_Pooled::SetSubRangeSize(uint32_t subRangeSize)
    {
        // After changing these values, we'll have to create a new buffer, so
        // ensure we're not in the middle of using any portion of the VBO first
        Finish();

        m_subRangeSize = subRangeSize;
        // Round the buffer size up to multiples of 4 bytes
        m_dataSize = ((m_numSubRanges * m_subRangeSize) + 3) & 0xFFFFFFFC;
        m_bufferSize = m_dataSize * m_numFrames;

        // Recreate the VBO with the new settings
        InitBuffers();
    }

    /// Sets the number of frames worth of data contained in the shared VBO
    /// \param numFrames number of frames worth of data that will be held within the VBO
    /// \note Must not be called between calls to BeginUpdate()/EndUpdate().
    void NvSharedVBOGL_Pooled::SetNumFrames(uint32_t numFrames)
    {
        // After changing these values, we'll have to create a new buffer, so
        // ensure we're not in the middle of using any portion of the VBO first
        Finish();

        m_numFrames = numFrames;
        m_bufferSize = m_dataSize * m_numFrames;

        // Recreate the VBO with the new settings
        InitBuffers();
    }

    void NvSharedVBOGL_Pooled::Finish()
    {
        if (0 != m_vbo)
        {
            if (nullptr != m_vboData)
            {
                glBindBuffer(GL_ARRAY_BUFFER, m_vbo);
                glUnmapBuffer(GL_ARRAY_BUFFER);
                glBindBuffer(GL_ARRAY_BUFFER, 0);
                m_vboData = 0;
            }
            glDeleteBuffers(1, &m_vbo);
            m_vbo = 0;
        }
    }

    bool NvSharedVBOGL_Pooled::BeginUpdate()
    {
        if (0 == m_vbo)
        {
            return false;
        }
        // Next buffer in the cycle
        uint32_t  nextIndex = (m_index + 1) % m_numFrames;

        // If we are persistently mapped, then we don't need to map anything each frame
        if (!m_bPersistent)
        {
            glBindBuffer(GL_ARRAY_BUFFER, m_vbo);
            GLbitfield flags = GL_MAP_WRITE_BIT |
                GL_MAP_INVALIDATE_RANGE_BIT |
                GL_MAP_UNSYNCHRONIZED_BIT;
            GLvoid* pBuff = glMapBufferRange(GL_ARRAY_BUFFER, m_dataSize * nextIndex, m_dataSize, flags);
            m_vboData = static_cast<uint8_t*>(pBuff);
            glBindBuffer(GL_ARRAY_BUFFER, 0);
            if (nullptr == m_vboData)
            {
                return false;
            }
        }

        m_index = nextIndex;
        return true;
    }

    void NvSharedVBOGL_Pooled::EndUpdate()
    {
        if (m_bPersistent)
        {
            return;
        }

        if (nullptr != m_vboData)
        {
            if (0 != m_vbo)
            {
                glBindBuffer(GL_ARRAY_BUFFER, m_vbo);
                glUnmapBuffer(GL_ARRAY_BUFFER);
                glBindBuffer(GL_ARRAY_BUFFER, 0);
            }
            m_vboData = nullptr;
        }
    }

    void NvSharedVBOGL_Pooled::DoneRendering()
    {
    }

    uint8_t* NvSharedVBOGL_Pooled::GetData()
    {
        if (nullptr == m_vboData)
        {
            return nullptr;
        }
        if (m_bPersistent)
        {
            // m_vboData always points to the beginning of the buffer, so we must
            // account for the offset of the currently used section
            return m_vboData + (m_index * m_dataSize);
        }
        else
        {
            // m_vboData always points to the beginning of the currently mapped
            // range of the buffer, so no additional offset is used
            return m_vboData;
        }
    }

    uint8_t* NvSharedVBOGL_Pooled::GetData(uint32_t subRangeIndex)
    {
        if (nullptr == m_vboData)
        {
            return nullptr;
        }
        if (m_bPersistent)
        {
            // m_vboData always points to the beginning of the buffer, so we must
            // account for the offset of the currently used section
            return m_vboData + (m_index * m_dataSize) + (subRangeIndex * m_subRangeSize);
        }
        else
        {
            // m_vboData always points to the beginning of the currently mapped
            // range of the buffer, so no additional per-frame offset is used
            return m_vboData + (subRangeIndex * m_subRangeSize);
        }
    }

    bool NvSharedVBOGL_Pooled::InitBuffers()
    {
        if (m_bufferSize == 0)
        {
            return false;
        }

        // If we already have a buffer created, then we need to unmap it,
        // if it is currently mapped, and also delete the old vbo, if we
        // are persistently mapped
        if (0 != m_vbo)
        {
            if (nullptr != m_vboData)
            {
                glBindBuffer(GL_ARRAY_BUFFER, m_vbo);
                glUnmapBuffer(GL_ARRAY_BUFFER);
                glBindBuffer(GL_ARRAY_BUFFER, 0);
                m_vboData = 0;
            }
            if (m_bPersistent)
            {
                glDeleteBuffers(1, &m_vbo);
                m_vbo = 0;
            }
        }

        // Create the vertex buffer
        if (0 == m_vbo)
        {
            glGenBuffers(1, &m_vbo);
        }

        glBindBuffer(GL_ARRAY_BUFFER, m_vbo);

        // Create the VBO buffer and map it if we're using persistent mapping
        if (m_bPersistent)
        {
            // For persistent mapping, we must use glBufferStorage
            GLbitfield flags = GL_MAP_WRITE_BIT | GL_MAP_PERSISTENT_BIT | GL_MAP_COHERENT_BIT;
            glBufferStorage(GL_ARRAY_BUFFER, m_bufferSize, 0, flags);
            m_vboData = static_cast<uint8_t*>(glMapBufferRange(GL_ARRAY_BUFFER, 0, m_bufferSize, flags));
            if (nullptr == m_vboData)
            {
                glBindBuffer(GL_ARRAY_BUFFER, 0);
                return false;
            }
        }
        else
        {
            glBufferData(GL_ARRAY_BUFFER, m_bufferSize, 0, GL_STREAM_DRAW);
        }
        glBindBuffer(GL_ARRAY_BUFFER, 0);
        return true;
    }

}
