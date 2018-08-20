
#pragma once

#include "NV/NvPlatformGL.h"

#include "RenderContext.h"

class ThreadedRenderingGL;
class NvGLSLProgram;
namespace Nv
{
	class NvSharedVBOGLPool;
}

///@brief These are the commands that can be issued to the command buffer.
///@note All commands must be PODs.
namespace cmds
{
	struct WaitFenceCommand
	{
		static const cb::RenderContext::function_t kDispatchFunction;

		uint32_t numDrawAheadFrames;
		GLsync* fences;
		uint32_t* currentFenceIndex;

		static void execute(const void* data, cb::RenderContext* rc);
	};

	struct VboPoolUpateCommand
	{
		static const cb::RenderContext::function_t kDispatchFunction;

		Nv::NvSharedVBOGLPool* vboPool;
		bool begin;

		static void execute(const void* data, cb::RenderContext* rc);
	};

	//  Draws the skybox with lighting in color and depth
	struct DrawSkyboxCommand
	{
		static const cb::RenderContext::function_t kDispatchFunction;

		GLuint projUBO_Location;
		GLuint projUBO_Id;
		GLuint sandTex;
		GLuint gradientTex;
		NvGLSLProgram* shader;

		static void execute(const void* data, cb::RenderContext* rc);
	};

	//  Draws the ground plane with caustics
	struct DrawGroundCommand
	{
		static const cb::RenderContext::function_t kDispatchFunction;

		GLuint projUBO_Location;
		GLuint projUBO_Id;
		GLuint lightingUBO_Location;
		GLuint lightingUBO_Id;
		GLuint caustic1Tex;
		GLuint caustic2Tex;
		GLuint skyboxSandTex;
		NvGLSLProgram* shader;

		static void execute(const void* data, cb::RenderContext* rc);
	};

}  // namespace cmd


