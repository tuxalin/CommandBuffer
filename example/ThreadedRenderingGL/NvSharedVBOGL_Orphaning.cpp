//----------------------------------------------------------------------------------
// File:        es3aep-kepler\ThreadedRenderingGL/NvSharedVBOGL_Orphaning.cpp
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
#include "NvSharedVBOGL_Orphaning.h"
#include <NV/NvLogs.h>

namespace Nv
{
    NvSharedVBOGL_Orphaning::NvSharedVBOGL_Orphaning()
        : m_vbo(0)
        , m_vboData(nullptr)
    {}

    bool NvSharedVBOGL_Orphaning::Initialize(uint32_t dataSize, uint32_t numBuffers, bool bPersistent)
    {
        numBuffers; // Unused
        bPersistent; // Unused.  Can't persist buffers if we orphan every frame

        // Create the vertex buffer
        glGenBuffers(1, &m_vbo);
        m_dataSize = dataSize;
        return true;
    }

    void NvSharedVBOGL_Orphaning::Finish()
    {
        if (0 != m_vbo)
        {
            if (nullptr != m_vboData)
            {
                glBindBuffer(GL_ARRAY_BUFFER, m_vbo);
                glUnmapBuffer(GL_ARRAY_BUFFER);
                glBindBuffer(GL_ARRAY_BUFFER, 0);
                m_vboData = nullptr;
            }
            glDeleteBuffers(1, &m_vbo);
            m_vbo = 0;
        }
    }

    bool NvSharedVBOGL_Orphaning::BeginUpdate()
    {
        if (0 == m_vbo)
        {
            return false;
        }
        glBindBuffer(GL_ARRAY_BUFFER, m_vbo);
        glBufferData(GL_ARRAY_BUFFER, m_dataSize, nullptr, GL_STREAM_DRAW);
        // Even though we're mapping the whole buffer, we use glMapBufferRange rather than glMapBuffer
        // since GLES doesn't support glMapBuffer
        GLvoid* pBuff = glMapBufferRange(GL_ARRAY_BUFFER, 0, m_dataSize, GL_MAP_WRITE_BIT | GL_MAP_INVALIDATE_BUFFER_BIT | GL_MAP_UNSYNCHRONIZED_BIT);
        m_vboData = static_cast<uint8_t*>(pBuff);
        glBindBuffer(GL_ARRAY_BUFFER, 0);
        if (nullptr == m_vboData)
        {
            return false;
        }
        return true;
    }

    void NvSharedVBOGL_Orphaning::EndUpdate()
    {
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

    void NvSharedVBOGL_Orphaning::DoneRendering()
    {
    }
}
