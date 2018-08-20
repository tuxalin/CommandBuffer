//----------------------------------------------------------------------------------
// File:        es3aep-kepler\ThreadedRenderingGL/NvSharedVBOGL_MappedSubRanges.cpp
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
#include "NvSharedVBOGL_MappedSubRanges.h"
#include <NV/NvLogs.h>

namespace Nv
{
    NvSharedVBOGL_MappedSubRanges::NvSharedVBOGL_MappedSubRanges()
        : m_index(0)
        , m_numBuffers(0)
        , m_bufferSize(0)
        , m_vboData(nullptr)
        , m_bPersistent(false)
    {}

    bool NvSharedVBOGL_MappedSubRanges::Initialize(uint32_t dataSize, uint32_t numBuffers, bool bPersistent)
    {
        // Initialize all of the basic bookkeeping
        m_numBuffers = numBuffers;
        m_index = 0;

        // Round the data/buffer size up to multiples of 4 bytes
        m_dataSize = (dataSize + 3) & 0xFFFFFFFC;

        m_bufferSize = m_dataSize * m_numBuffers;
        m_bPersistent = bPersistent;

        // Create the vertex buffer
        glGenBuffers(1, &m_vbo);
        glBindBuffer(GL_ARRAY_BUFFER, m_vbo);

        // Create the VBO buffer and map it if we're using persistent mapping
        if (m_bPersistent)
        {
            // We have to use glBufferStorage to use the persistent/coherent bits 
            GLbitfield flags = GL_MAP_WRITE_BIT | GL_MAP_PERSISTENT_BIT | GL_MAP_COHERENT_BIT;
            glBufferStorage(GL_ARRAY_BUFFER, m_bufferSize, 0, flags);
            m_vboData = static_cast<uint8_t*>(glMapBufferRange(GL_ARRAY_BUFFER, 0, m_bufferSize, flags));
            glBindBuffer(GL_ARRAY_BUFFER, 0);
            if (nullptr == m_vboData)
            {
                return false;
            }
        }
        else
        {
            glBufferData(GL_ARRAY_BUFFER, m_bufferSize, 0, GL_STREAM_DRAW);
            glBindBuffer(GL_ARRAY_BUFFER, 0);
        }
        return true;
    }

    void NvSharedVBOGL_MappedSubRanges::Finish()
    {
        if (0 != m_vbo)
        {
            if (nullptr != m_vboData)
            {
                // Bind our VBO so that we can unmap it
                glBindBuffer(GL_ARRAY_BUFFER, m_vbo);
                glUnmapBuffer(GL_ARRAY_BUFFER);
                glBindBuffer(GL_ARRAY_BUFFER, 0);
                m_vboData = nullptr;
            }            
            glDeleteBuffers(1, &m_vbo);
            m_vbo = 0;
        }
    }

    bool NvSharedVBOGL_MappedSubRanges::BeginUpdate()
    {
        // Next buffer in the cycle
        uint32_t  nextIndex = (m_index + 1) % m_numBuffers;
        if (!m_bPersistent)
        {
            if (0 == m_vbo)
            {
                return false;
            }
            // Map our buffer, discarding the previous contents of the range that we're
            // mapping and marking it as unsynchronized, since we'll be managing 
            // that via fences in the main app
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

    void NvSharedVBOGL_MappedSubRanges::EndUpdate()
    {
        // If we are persistently mapped, then we don't need
        // to do anything in here as we don't unmap it each frame.
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

    void NvSharedVBOGL_MappedSubRanges::DoneRendering()
    {
    }

    uint8_t* NvSharedVBOGL_MappedSubRanges::GetData()
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

}
