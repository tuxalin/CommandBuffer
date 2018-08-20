//----------------------------------------------------------------------------------
// File:        es3aep-kepler\ThreadedRenderingGL/NvInstancedModelExtGL.cpp
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
#include "NvInstancedModelExtGL.h"
#include "VertexFormatBinder.h"

namespace Nv
{
	bool NvInstancedModelExtGL::EnableInstancing(VertexFormatBinder* pInstancingVertexBinder, NvSharedVBOGL* pInstanceDataStream) 
    {
        m_pInstancingVertexBinder = pInstancingVertexBinder;
        m_pInstanceDataStream = pInstanceDataStream;
		return true;
	}

    uint32_t NvInstancedModelExtGL::Render(GLint positionHandle, GLint normalHandle, GLint texcoordHandle, GLint tangentHandle)
    {
        // If we don't have a model to render, we can't render
        if (nullptr == m_pSourceModel)
        {
            return 0;
        }

        // Choose the correct method of rendering based on number of instances and maximum number per draw call
        if ((nullptr == m_pInstanceDataStream) || (nullptr == m_pInstancingVertexBinder))
        {
            return RenderNonInstanced(positionHandle, normalHandle, texcoordHandle, tangentHandle);
        }
        else if (m_batchSize >= m_instanceCount)
        {
            return RenderInstanced(positionHandle, normalHandle, texcoordHandle, tangentHandle);
        }
        else
        {
            return RenderBatched(positionHandle, normalHandle, texcoordHandle, tangentHandle);
        }
    }

	NvInstancedModelExtGL::NvInstancedModelExtGL(uint32_t instanceCount,
		NvModelExtGL* pSourceModel) :
		m_instanceCount(instanceCount),
		m_pSourceModel(nullptr)
	{
		SetSourceModel(pSourceModel);
	}

    uint32_t NvInstancedModelExtGL::RenderNonInstanced(GLint positionHandle, GLint normalHandle, GLint texcoordHandle, GLint tangentHandle)
    {
        // No instance data.  Render as single mesh.
        m_pSourceModel->DrawElements(1, positionHandle, normalHandle, texcoordHandle, tangentHandle);
        return 1;
    }

    uint32_t NvInstancedModelExtGL::RenderInstanced(GLint positionHandle, GLint normalHandle, GLint texcoordHandle, GLint tangentHandle)
    {
        NV_ASSERT(m_pInstancingVertexBinder != nullptr);
        // Activate the instancing data by binding the instance data stream and setting
        // up all of the offsets into each of the attributes
        m_pInstancingVertexBinder->Activate(m_pInstanceDataStream);
        m_pSourceModel->DrawElements(m_instanceCount, positionHandle, normalHandle, texcoordHandle, tangentHandle);
        m_pInstancingVertexBinder->Deactivate();
        return 1;
    }

    uint32_t NvInstancedModelExtGL::RenderBatched(GLint positionHandle, GLint normalHandle, GLint texcoordHandle, GLint tangentHandle)
    {
        bool bFirstBatch = true;
        uint32_t batchOffset = 0;
        uint32_t batchInstanceCount = m_batchSize;
        uint32_t numDraws = 0;

        // Invoke the number of draws required to render all of our instances, while limiting
        // each draw call to a number of instances equal to or less than our batch size
        for (uint32_t remainingInstances = m_instanceCount; remainingInstances > 0; remainingInstances -= batchInstanceCount)
        {
            if (remainingInstances < m_batchSize)
            {
                batchInstanceCount = remainingInstances;
            }

            if (bFirstBatch)
            {
                m_pInstancingVertexBinder->Activate(m_pInstanceDataStream);
            }
            else
            {
                m_pInstancingVertexBinder->UpdatePointers(m_pInstanceDataStream, batchOffset);
            }

            m_pSourceModel->DrawElements(batchInstanceCount, positionHandle, normalHandle, texcoordHandle, tangentHandle);

            ++numDraws;
            bFirstBatch = false;
            batchOffset += m_pInstancingVertexBinder->GetStride() * batchInstanceCount;
        }

        m_pInstancingVertexBinder->Deactivate();
        return numDraws;
    }
}
