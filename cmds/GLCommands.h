
#pragma once

#define USE_GLEW

#include "CommandPacket.h"
#include "RenderContext.h"

#include <cstdint>

#ifdef USE_GLEW
#include <gl/glew.h>
#else
#include <gl/GL.h>
#endif

namespace cmds
{
	struct DrawArrays
	{
		static const cb::RenderContext::function_t kDispatchFunction;

		GLuint vao;
		uint32_t base;
		uint32_t count;
		GLenum primitive;
	};

	struct DrawIndexed
	{
		static const cb::RenderContext::function_t kDispatchFunction;

		GLuint vao;
		uint32_t base;  // in bytes, will be converted to void*
		uint32_t count;
		GLenum primitive : 24;
		uint32_t useShortIndices : 8; // bool
	};

	struct DrawInstanced
	{
		static const cb::RenderContext::function_t kDispatchFunction;

		GLuint vao;
		uint32_t base;
		uint32_t count;
		GLenum primitive : 8;
		uint32_t instanceCount : 24;
	};

	struct ClearColor
	{
		static const cb::RenderContext::function_t kDispatchFunction;

		union {
			float color[4];
			struct {
				float red;
				float green;
				float blue;
				float alpha;
			};
		};

		GLbitfield flags;

		CB_COMMAND_PACKET_ALIGN()
	};

	struct DepthSetup
	{
		static const cb::RenderContext::function_t kDispatchFunction;

		GLenum compare;
		bool depthWrite;
		bool depthTest;

		CB_COMMAND_PACKET_ALIGN()
	};

	struct BlendSetup
	{
		static const cb::RenderContext::function_t kDispatchFunction;

		GLenum src;
		GLenum dst;
		bool enable;

		CB_COMMAND_PACKET_ALIGN()
	};

	struct BindShader
	{
		static const cb::RenderContext::function_t kDispatchFunction;

		GLuint shader;

		CB_COMMAND_PACKET_ALIGN()
	};

	struct BindTexture
	{
		static const cb::RenderContext::function_t kDispatchFunction;

		enum TextureType
		{
			e1D = GL_TEXTURE_1D,
			e2D = GL_TEXTURE_2D,
			e3D = GL_TEXTURE_3D,
			e2DArray = GL_TEXTURE_2D_ARRAY
		};

		TextureType type;
		uint32_t textureUnit;
		GLuint texture;
	};

	struct BindUniformBuffer
	{
		static const cb::RenderContext::function_t kDispatchFunction;

		GLuint ubo;
		GLuint location;
	};

	struct BindFramebuffer
	{
		static const cb::RenderContext::function_t kDispatchFunction;

		GLenum target;
		GLuint fbo;
	};

	struct UnbindFramebuffer
	{
		static const cb::RenderContext::function_t kDispatchFunction;

		GLenum target;

		CB_COMMAND_PACKET_ALIGN()
	};

	struct VertexBufferUpdate
	{
		static const cb::RenderContext::function_t kDispatchFunction;

		GLenum target;
		GLuint buffer;
		const void* src;
		uint32_t offset;
		uint32_t size;
	};

	struct VertexBufferCopy
	{
		static const cb::RenderContext::function_t kDispatchFunction;

		GLenum target;
		GLuint srcBuffer;
		GLuint dstBuffer;
		uint32_t srcOffset;
		uint32_t dstOffset;
		uint32_t size;
	};
}  // namespace cmds

