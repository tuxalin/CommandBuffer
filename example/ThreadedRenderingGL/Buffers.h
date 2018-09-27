#pragma once

#include "NV/NvLogs.h"
#include "NvGLUtils/NvGLSLProgram.h"

#include "CommandBuffer.h"
#include "CommandKeys.h"

namespace Nv
{
	// Example of a material binder with state that has redundancy checks.
	struct MaterialBinder
	{
		struct Material
		{
			std::string name;
			GLuint location = 0;
			GLuint ubo = 0;
			NvGLSLProgram* shader = nullptr;
		};

		///@note Returns true if there are more passes to bind.
		CB_FORCE_INLINE bool operator()(cb::MaterialId material) const
		{
			if (material.id == 0) // first material is a dummy
				return false;

			const Material&  mat = materials[material.id];
			assert(mat.shader);
			if (mat.shader != activeShader)
			{
				//bind only if different shaders
				mat.shader->enable();
				activeShader = mat.shader;
			}
			if (material.id != activeMaterial)
			{
				// bind material ubo
				glBindBufferBase(GL_UNIFORM_BUFFER, mat.location, mat.ubo);
				activeMaterial = material.id;
			}

			return false;
		}

		void debugMsg(cb::MaterialId material)
		{
			if (material.id != 0) // first material is a dummy
				LOGI("Binding \'%s\' material ", materials[material.id].name.c_str());
		}

		void reset()
		{
			activeMaterial = -1;
			activeShader = nullptr;
		}

		std::vector<Material> materials;
		mutable	int activeMaterial = -1;
		mutable NvGLSLProgram* activeShader = nullptr;
	};
}

typedef cb::CommandBuffer<cb::DrawKey, cb::DefaultKeyDecoder, Nv::MaterialBinder> GeometryCommandBuffer;
typedef cb::CommandBuffer<uint32_t,cb::DummyKeyDecoder<uint32_t>> DeferredCommandBuffer;
