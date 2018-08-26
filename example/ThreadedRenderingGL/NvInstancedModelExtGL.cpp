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

	uint32_t NvInstancedModelExtGL::Render(GeometryCommandBuffer& geometryCommands, GLint positionHandle, GLint normalHandle, GLint texcoordHandle, GLint tangentHandle)
	{
		// If we don't have a model to render, we can't render
		if (nullptr == m_pSourceModel)
		{
			return 0;
		}

		// Choose the correct method of rendering based on number of instances and maximum number per draw call
		if ((nullptr == m_pInstanceDataStream) || (nullptr == m_pInstancingVertexBinder))
		{
			RenderNonInstanced& cmd = *geometryCommands.addCommand<RenderNonInstanced>(m_drawKey);
			cmd.normalHandle = normalHandle;
			cmd.positionHandle = positionHandle;
			cmd.texcoordHandle = texcoordHandle;
			cmd.tangentHandle = tangentHandle;
			cmd.pSourceModel = m_pSourceModel;
			CB_DEBUG_COMMAND_TAG(cmd);
			return 1;
		}
		else if (m_batchSize >= m_instanceCount)
		{
			NV_ASSERT(m_pInstancingVertexBinder != nullptr);
			RenderInstanced& cmd = *geometryCommands.addCommand<RenderInstanced>(m_drawKey);
			cmd.normalHandle = normalHandle;
			cmd.positionHandle = positionHandle;
			cmd.texcoordHandle = texcoordHandle;
			cmd.tangentHandle = tangentHandle;
			cmd.pSourceModel = m_pSourceModel;
			cmd.instanceCount = m_instanceCount;
			cmd.pInstanceDataStream = m_pInstanceDataStream;
			cmd.pInstancingVertexBinder = m_pInstancingVertexBinder;
			CB_DEBUG_COMMAND_TAG(cmd);
			return 1;
		}
		else
		{
			return RenderBatched(geometryCommands, positionHandle, normalHandle, texcoordHandle, tangentHandle);
		}
	}

	NvInstancedModelExtGL::NvInstancedModelExtGL(uint32_t instanceCount,
		NvModelExtGL* pSourceModel) :
		m_instanceCount(instanceCount),
		m_pSourceModel(nullptr),
		m_drawKey(0)
	{
		SetSourceModel(pSourceModel);
	}

	uint32_t NvInstancedModelExtGL::RenderBatched(GeometryCommandBuffer& geometryCommands, GLint positionHandle, GLint normalHandle, GLint texcoordHandle, GLint tangentHandle)
	{
		bool bFirstBatch = true;
		uint32_t batchOffset = 0;
		uint32_t batchInstanceCount = m_batchSize;
		uint32_t numDraws = 0;

		// Add first command to activate the vertex binder.
		NV_ASSERT(m_pInstancingVertexBinder != nullptr);
		UpdateVertexBinder& cmd = *geometryCommands.addCommand<UpdateVertexBinder>(m_drawKey);
		cmd.pInstanceDataStream = m_pInstanceDataStream;
		cmd.pInstancingVertexBinder = m_pInstancingVertexBinder;
		cmd.activate = true;
		CB_DEBUG_COMMAND_SET_MSG(cmd, "Activate binder");

		// Invoke the number of draws required to render all of our instances, while limiting
		// each draw call to a number of instances equal to or less than our batch size
		RenderInstancedUpdate* cmdPtr;
		for (uint32_t remainingInstances = m_instanceCount; remainingInstances > 0; remainingInstances -= batchInstanceCount)
		{
			if (remainingInstances < m_batchSize)
			{
				batchInstanceCount = remainingInstances;
			}

			// Chain draw commands
			cmdPtr = bFirstBatch ? geometryCommands.appendCommand<RenderInstancedUpdate>(&cmd) :
				geometryCommands.appendCommand<RenderInstancedUpdate>(cmdPtr);

			RenderInstancedUpdate& renderCmd = *cmdPtr;
			renderCmd.normalHandle = normalHandle;
			renderCmd.positionHandle = positionHandle;
			renderCmd.texcoordHandle = texcoordHandle;
			renderCmd.tangentHandle = tangentHandle;
			renderCmd.pSourceModel = m_pSourceModel;
			renderCmd.instanceCount = batchInstanceCount;
			renderCmd.offset = batchOffset;
			renderCmd.pInstanceDataStream = m_pInstanceDataStream;
			renderCmd.pInstancingVertexBinder = m_pInstancingVertexBinder;
			CB_DEBUG_COMMAND_TAG(renderCmd);

			++numDraws;
			bFirstBatch = false;
			batchOffset += m_pInstancingVertexBinder->GetStride() * batchInstanceCount;
		}

		auto& lastCmd = *geometryCommands.appendCommand<UpdateVertexBinder>(cmdPtr);
		lastCmd = cmd;
		lastCmd.activate = false;
		CB_DEBUG_COMMAND_TAG(lastCmd);

		return numDraws;
	}

	void NvInstancedModelExtGL::RenderInstancedUpdate::execute() const
	{
		pInstancingVertexBinder->UpdatePointers(pInstanceDataStream, offset);
		pSourceModel->DrawElements(instanceCount, positionHandle, normalHandle, texcoordHandle, tangentHandle);
	}

	void NvInstancedModelExtGL::RenderInstanced::execute() const
	{
		// Activate the instancing data by binding the instance data stream and setting
		// up all of the offsets into each of the attributes
		pInstancingVertexBinder->Activate(pInstanceDataStream);
		pSourceModel->DrawElements(instanceCount, positionHandle, normalHandle, texcoordHandle, tangentHandle);
		pInstancingVertexBinder->Deactivate();
	}

	void NvInstancedModelExtGL::UpdateVertexBinder::execute() const
	{
		if (activate) {
			// Activate the instancing data by binding the instance data stream and setting
			// up all of the offsets into each of the attributes
			pInstancingVertexBinder->Activate(pInstanceDataStream);
		}
		else {
			pInstancingVertexBinder->Deactivate();
		}
	}

	// Use utility to generate global functions from member functions.
	const cb::RenderContext::function_t NvInstancedModelExtGL::RenderNonInstanced::kDispatchFunction = &cb::makeExecuteFunction<NvInstancedModelExtGL::RenderNonInstanced>;
	const cb::RenderContext::function_t NvInstancedModelExtGL::RenderInstanced::kDispatchFunction = &cb::makeExecuteFunction<NvInstancedModelExtGL::RenderInstanced>;
	const cb::RenderContext::function_t NvInstancedModelExtGL::RenderInstancedUpdate::kDispatchFunction = &cb::makeExecuteFunction<NvInstancedModelExtGL::RenderInstancedUpdate>;
	const cb::RenderContext::function_t NvInstancedModelExtGL::UpdateVertexBinder::kDispatchFunction = &cb::makeExecuteFunction<NvInstancedModelExtGL::UpdateVertexBinder>;
}

