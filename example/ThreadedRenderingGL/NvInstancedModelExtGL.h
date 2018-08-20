//----------------------------------------------------------------------------------
// File:        es3aep-kepler\ThreadedRenderingGL/NvInstancedModelExtGL.h
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
#ifndef NvInstancedModelExtGL_H_
#define NvInstancedModelExtGL_H_

#include "NvGLUtils/NvModelExtGL.h"
#include "NvSharedVBOGL.h"
#include <map>

namespace Nv
{
	class VertexFormatBinder;

	/// \file
	/// GL-specific instanced set of multi-submesh geometric models; handling
	/// and rendering
	class NvInstancedModelExtGL
	{
	public:
		~NvInstancedModelExtGL()
		{
			Release();
			m_pSourceModel = nullptr;
		}

		/// Initialize instanced set of models with optional passed in ptr
		/// \param[in] instanceCount Initial number of instances to render
		///                          when rendering this model.
		/// \param[in] pSourceModel Optional pointer to an NvModelExtGL to use
		///							for mesh data.
		///							WARNING!!! This pointer is cached in the
		///							object, and must not be freed after this
		///							call returns unless it is removed from the
		///							instanced model. WARNING!!!
		/// \return a pointer to the GL-specific object or NULL on failure
		static NvInstancedModelExtGL* Create(uint32_t instanceCount,
			NvModelExtGL* pSourceModel = NULL)
		{
			NvInstancedModelExtGL* pInstances = new NvInstancedModelExtGL(instanceCount, pSourceModel);
			return pInstances;
		}

		/// Sets the model that is to be used as the instance template.
		/// \param[in] pSourceModel Pointer to an NvModelExtGL to use for mesh
		///							data.
		void SetSourceModel(NvModelExtGL* pSourceModel)
		{
			m_pSourceModel = pSourceModel;
		}

		/// Free the rendering resources held by this model
		void Release() {}

		/// Return the model that is being used as the instance template.
		/// \return a pointer to the #NvModelExtGL cloned by instances
		NvModelExtGL* GetModel() { return m_pSourceModel; }

		/// Retrieves the current number of instances that will be rendered
		/// when this instanced model is rendered
		/// \return Number of instances rendered by this model
		uint32_t GetInstanceCount() const { return m_instanceCount; }

		/// Sets the number of instances to render when rendering this model.
		/// \note Assumes that all instance data streams attached to this model
		/// have enough data to support the requested number of instances
		/// \param count Number of instances to render when Render() is called
		void SetInstanceCount(uint32_t count) { m_instanceCount = count; }

		/// Enables/Disables instanced rendering
		/// \param pInstancingVertexBinder Pointer to the Vertex Binder that will
		///                                handle setting up the vertex state for
		///                                rendering with the instance data stream
		/// \parm pInstanceDataStream Pointer to the thread-safe VBO wrapper
		///                           that contains the instance data for rendering
		/// \return True if instancing was enabled, but False if there was a 
		///         problem while attempting to enable it.
		bool EnableInstancing(VertexFormatBinder* pInstancingVertexBinder,
			NvSharedVBOGL* pInstanceDataStream);

		/// Enables rendering instance data in batches, using the given value as
		/// the maximum number of instances to render per batch
		/// \param batchSize Number of instances to render per draw call.  A size
		///                  of 0 will disable batch rendering and render all 
		///                  instances in a single draw call.
		void SetBatchSize(uint32_t batchSize) { m_batchSize = batchSize; }

		/// Draw the model using the current shader (positions, UVs, normals and tangents)
		/// Binds the vertex position, UV, normal and tangent arrays to the given attribute array indices and draws the
		/// model with the currently bound shader. Use a negative value for a handle for any attributes that should
		/// not be bound for use in this draw call.
		/// \param[in] positionHandle the vertex attribute array index that represents position in the current shader
		/// \param[in] normalHandle the vertex attribute array index that represents normals in the current shader
		/// \param[in] texcoordHandle the vertex attribute array index that represents UVs in the current shader
		/// \param[in] tangentHandle the vertex attribute array index that represents tangents in the current shader
		/// \return Returns the number of draw calls used to render this model
		uint32_t Render(GLint positionHandle, GLint normalHandle = -1, GLint texcoordHandle = -1, GLint tangentHandle = -1);

	private:
		/// \privatesection
		// NvInstancedModelExtGL may only be created through the factory
		// method
		NvInstancedModelExtGL(uint32_t instanceCount, NvModelExtGL* pSourceModel = nullptr);

		// Helper methods for rendering.  See Render() for parameter descriptions
		uint32_t RenderNonInstanced(GLint positionHandle, GLint normalHandle = -1, GLint texcoordHandle = -1, GLint tangentHandle = -1);
		uint32_t RenderInstanced(GLint positionHandle, GLint normalHandle = -1, GLint texcoordHandle = -1, GLint tangentHandle = -1);
		uint32_t RenderBatched(GLint positionHandle, GLint normalHandle = -1, GLint texcoordHandle = -1, GLint tangentHandle = -1);

		// Vertex data to use as the instancing data stream along with the vertex format
		// binder that defines the layout used
		VertexFormatBinder* m_pInstancingVertexBinder;
		NvSharedVBOGL*  m_pInstanceDataStream;

		// Pointer to the model to be instanced
		NvModelExtGL* m_pSourceModel;

		// Number of instances in the instance data buffer
		uint32_t m_instanceCount;

		// Number of instances to render per draw call
		uint32_t m_batchSize;
	};
}
#endif // NvInstancedModelExtGL_H_

