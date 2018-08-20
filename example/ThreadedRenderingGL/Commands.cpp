
#include "Commands.h"

#include "NV/NvLogs.h"
#include "NvGLUtils/NvGLSLProgram.h"
#include "NvGLUtils/NvShapesGL.h"
#include "NvSharedVBOGL.h"

namespace cmds
{
	void WaitFenceCommand::execute(const void* data, cb::RenderContext* rc)
	{
		auto& cmd = *reinterpret_cast<const WaitFenceCommand*>(data);

		uint32_t nextFenceIndex = (*cmd.currentFenceIndex + 1) % cmd.numDrawAheadFrames;

		if (nullptr != cmd.fences[nextFenceIndex])
		{
			GLenum waitStatus = glClientWaitSync(cmd.fences[nextFenceIndex], 0, 1000000);
			switch (waitStatus)
			{
			case GL_TIMEOUT_EXPIRED:
			{
#ifdef _DEBUG
				LOGI("Timed out waiting for NvSharedVBOGL sync!");
#endif
				return;
			}
			case GL_WAIT_FAILED:
			{
#ifdef _DEBUG
				LOGI("Failed waiting for NvSharedVBOGL sync!");
#endif
				return;
			}
			}

			// Successfully waited for the fence.  Clear it and continue;
			glDeleteSync(cmd.fences[nextFenceIndex]);
			cmd.fences[nextFenceIndex] = nullptr;
		}
		*cmd.currentFenceIndex = nextFenceIndex;
	}

	void VboPoolUpateCommand::execute(const void* data, cb::RenderContext* rc)
	{
		auto& cmd = *reinterpret_cast<const VboPoolUpateCommand*>(data);
		if (cmd.begin)
		{
			cmd.vboPool->BeginUpdate();
		}
		else
		{
			cmd.vboPool->EndUpdate();
		}
	}

	void DrawSkyboxCommand::execute(const void* data, cb::RenderContext* rc)
	{
		auto& cmd = *reinterpret_cast<const DrawSkyboxCommand*>(data);

		glDisable(GL_DEPTH_TEST);
		glBindBufferBase(GL_UNIFORM_BUFFER, cmd.projUBO_Location, cmd.projUBO_Id);
		glActiveTexture(GL_TEXTURE4);
		glBindTexture(GL_TEXTURE_2D, cmd.sandTex);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
		glActiveTexture(GL_TEXTURE5);
		glBindTexture(GL_TEXTURE_2D, cmd.gradientTex);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);

		cmd.shader->enable();
		NvDrawQuadGL(0);
		cmd.shader->disable();

		glEnable(GL_DEPTH_TEST);
	}

	void DrawGroundCommand::execute(const void* data, cb::RenderContext* rc)
	{
		// vertex positions in NDC tex-coords
		static const float groundplaneQuadData[] =
		{
			1.0f,  1.0f, 1.0f, 0.0f,
			1.0f, -1.0f, 1.0f, 1.0f,
			-1.0f,  1.0f, 0.0f, 0.0f,
			-1.0f, -1.0f, 0.0f, 1.0f
		};

		auto& cmd = *reinterpret_cast<const DrawGroundCommand*>(data);

		glActiveTexture(GL_TEXTURE2);
		glBindTexture(GL_TEXTURE_2D, cmd.caustic1Tex);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
		glActiveTexture(GL_TEXTURE3);
		glBindTexture(GL_TEXTURE_2D, cmd.caustic2Tex);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);

		glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, 4 * sizeof(float), groundplaneQuadData);
		glEnableVertexAttribArray(0);
		glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), &groundplaneQuadData[2]);
		glEnableVertexAttribArray(1);

		glDisable(GL_DEPTH_TEST);
		glBindBufferBase(GL_UNIFORM_BUFFER, cmd.projUBO_Location, cmd.projUBO_Id);
		glBindBufferBase(GL_UNIFORM_BUFFER, cmd.lightingUBO_Location, cmd.lightingUBO_Id);

		glActiveTexture(GL_TEXTURE4);
		glBindTexture(GL_TEXTURE_2D, cmd.skyboxSandTex);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);

		glDisable(GL_CULL_FACE);
		cmd.shader->enable();
		glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
		cmd.shader->disable();

		glEnable(GL_DEPTH_TEST);

		glDisableVertexAttribArray(0);
		glDisableVertexAttribArray(1);
	}

	const cb::RenderContext::function_t WaitFenceCommand::kDispatchFunction = &WaitFenceCommand::execute;
	const cb::RenderContext::function_t VboPoolUpateCommand::kDispatchFunction = &VboPoolUpateCommand::execute;
	const cb::RenderContext::function_t DrawSkyboxCommand::kDispatchFunction = &DrawSkyboxCommand::execute;
	const cb::RenderContext::function_t DrawGroundCommand::kDispatchFunction = &DrawGroundCommand::execute;
}
