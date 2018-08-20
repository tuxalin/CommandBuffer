//----------------------------------------------------------------------------------
// File:        es3aep-kepler\ThreadedRenderingGL/VertexFormatBinder.cpp
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
#include "VertexFormatBinder.h"

namespace Nv
{
    bool VertexFormatBinder::AddInstanceAttrib(GLuint attribIndex,
        GLint size,
        GLenum type,
        GLenum usage,
        GLboolean normalized,
        GLuint divisor,
        GLuint offset)
    {
        // Verify that the usage is valid.  We only support these three values
        // to determine how the data should be used.
        if ((usage != GL_FLOAT) && (usage != GL_INT) && (usage != GL_DOUBLE))
        {
            return false;
        }

        // Create the attribute description block for this attribute and add 
        // it to the map of attributes.
        InstanceAttribDesc desc;
        desc.m_attribIndex = attribIndex;
        desc.m_size = size;
        desc.m_type = type;
        desc.m_usage = usage;
        desc.m_normalized = normalized;
        desc.m_divisor = divisor;
        desc.m_offset = offset;

        m_instanceAttributes[attribIndex] = desc;
        return true;
	}

    void VertexFormatBinder::RemoveInstanceAttrib(GLuint attribIndex)
    {
        m_instanceAttributes.erase(attribIndex);
    }


    void VertexFormatBinder::Activate(NvSharedVBOGL* pInstanceDataStream)
    {
        // Activate the instancing data by binding the instance data stream and setting
        // up all of the offsets into each of the attributes
		intptr_t pBufferStart = pInstanceDataStream->GetDynamicOffset();
        glBindBuffer(GL_ARRAY_BUFFER, pInstanceDataStream->GetBuffer());
        AttribSet::iterator attribIt = m_instanceAttributes.begin();
        AttribSet::const_iterator attribEnd = m_instanceAttributes.end();
        for (; attribIt != attribEnd; ++attribIt)
        {
            const InstanceAttribDesc& desc = attribIt->second;
            glEnableVertexAttribArray(desc.m_attribIndex);

            // We have to use an appropriate version of the glVertexAttribPointer family of methods
            // depending on the type of data and how it will be used in the shader
            switch (desc.m_usage)
            {
            case GL_FLOAT:
                glVertexAttribPointer(desc.m_attribIndex, desc.m_size, desc.m_type, desc.m_normalized, m_stride, (GLvoid*)(pBufferStart + desc.m_offset));
                break;
            case GL_INT:
                glVertexAttribIPointer(desc.m_attribIndex, desc.m_size, desc.m_type, m_stride, (GLvoid*)(pBufferStart + desc.m_offset));
                break;
            case GL_DOUBLE:
                glVertexAttribLPointer(desc.m_attribIndex, desc.m_size, desc.m_type, m_stride, (GLvoid*)(pBufferStart + desc.m_offset));
                break;
            }
            glVertexAttribDivisor(desc.m_attribIndex, desc.m_divisor);
        }
    }

    void VertexFormatBinder::UpdatePointers(NvSharedVBOGL* pInstanceDataStream, uint32_t batchOffset)
    {
        // Activate the instancing data by binding the instance data stream and setting
        // up all of the offsets into each of the attributes
		intptr_t pBufferStart = pInstanceDataStream->GetDynamicOffset();
        glBindBuffer(GL_ARRAY_BUFFER, pInstanceDataStream->GetBuffer());
        AttribSet::iterator attribIt = m_instanceAttributes.begin();
        AttribSet::const_iterator attribEnd = m_instanceAttributes.end();
        for (; attribIt != attribEnd; ++attribIt)
        {
            const InstanceAttribDesc& desc = attribIt->second;
            // We have to use an appropriate version of the glVertexAttribPointer family of methods
            // depending on the type of data and how it will be used in the shader
            switch (desc.m_usage)
            {
            case GL_FLOAT:
                glVertexAttribPointer(desc.m_attribIndex, desc.m_size, desc.m_type, desc.m_normalized, m_stride, (GLvoid*)(pBufferStart + batchOffset + desc.m_offset));
                break;
            case GL_INT:
                glVertexAttribIPointer(desc.m_attribIndex, desc.m_size, desc.m_type, m_stride, (GLvoid*)(pBufferStart + batchOffset + desc.m_offset));
                break;
            case GL_DOUBLE:
                glVertexAttribLPointer(desc.m_attribIndex, desc.m_size, desc.m_type, m_stride, (GLvoid*)(pBufferStart + batchOffset + desc.m_offset));
                break;
            }
        }
    }

    void VertexFormatBinder::Deactivate()
    {
        // Clear out the vertex attribute bindings
        AttribSet::iterator attribIt = m_instanceAttributes.begin();
        AttribSet::const_iterator attribEnd = m_instanceAttributes.end();
        for (; attribIt != attribEnd; ++attribIt)
        {
            const InstanceAttribDesc& desc = attribIt->second;
            glDisableVertexAttribArray(desc.m_attribIndex);
            glVertexAttribDivisor(desc.m_attribIndex, 0);
        }
    }
}
