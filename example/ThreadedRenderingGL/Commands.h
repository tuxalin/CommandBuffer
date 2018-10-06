
#pragma once

#include "NV/NvPlatformGL.h"

#include "CommandPacket.h"
#include "RenderContext.h"

class ThreadedRenderingGL;
class NvGLSLProgram;
namespace Nv
{
	class NvSharedVBOGLPool;
	class NvSharedVBOGL;
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
	};

	struct VboPoolUpdateCommand
	{
		static const cb::RenderContext::function_t kDispatchFunction;

		Nv::NvSharedVBOGLPool* vboPool;
		bool begin;
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
	};

	struct VboUpdate
	{
		static const cb::RenderContext::function_t kDispatchFunction;

		Nv::NvSharedVBOGL* vbo;
		const void* data;
		size_t size;
	};

	struct ClearRenderTarget
	{
		static const cb::RenderContext::function_t kDispatchFunction;

		int bufferCount;

		CB_COMMAND_PACKET_ALIGN()
	};

}  // namespace cmd


