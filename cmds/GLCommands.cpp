
#include "GLCommands.h"

#include <cassert>

#ifndef USE_GLEW
#define GL_GLEXT_PROTOTYPES
#include <gl/glext.h>
#endif

#if _WIN32 || _WIN64
#if _WIN64
#define USE_64_BIT
#else
#define USE_32_BIT
#endif
#endif
#if __GNUC__
#if __x86_64__ || __ppc64__
#define USE_64_BIT
#else
#define USE_32_BIT
#endif
#endif

namespace cmds
{
#ifdef USE_64_BIT
	typedef uint64_t index_size_t;
#else
	typedef uint32_t index_size_t;
#endif
	static const index_size_t kIndexSizes[] = { sizeof(uint32_t), sizeof(uint16_t) };
	static const GLenum kIndexTypes[] = { GL_UNSIGNED_INT, GL_UNSIGNED_SHORT };

	enum class BufferLockType
	{
		eDiscard,
		eReadOnly,
		eWriteOnly
	};

	void drawArrays(const void* commandData, cb::RenderContext*)
	{
		const auto& cmd = *reinterpret_cast<const cmds::DrawArrays*>(commandData);
		glBindVertexArray(cmd.vao);
		glDrawArrays(cmd.primitive, cmd.base, cmd.count);
	}

	void drawIndexed(const void* commandData, cb::RenderContext*)
	{
		const auto& cmd = *reinterpret_cast<const cmds::DrawIndexed*>(commandData);

		auto base = reinterpret_cast<const void*>(kIndexSizes[cmd.useShortIndices]);

		glBindVertexArray(cmd.vao);
		glDrawElements(cmd.primitive, cmd.count, kIndexTypes[cmd.useShortIndices], base);
	}

	void drawInstanced(const void* commandData, cb::RenderContext*)
	{
		const auto& cmd = *reinterpret_cast<const cmds::DrawInstanced*>(commandData);
		glBindVertexArray(cmd.vao);
		glDrawArraysInstanced(cmd.primitive, cmd.base, cmd.count, cmd.instanceCount);
	}

	void clearColor(const void* commandData, cb::RenderContext*)
	{
		const auto& cmd = *reinterpret_cast<const cmds::ClearColor*>(commandData);
		glClearColor(cmd.red, cmd.green, cmd.blue, cmd.alpha);
		glClear(cmd.flags);
	}

	void depthSetup(const void* commandData, cb::RenderContext*)
	{
		const auto& cmd = *reinterpret_cast<const cmds::DepthSetup*>(commandData);
		glDepthMask(cmd.depthWrite);
		if (cmd.depthTest)
			glEnable(GL_DEPTH_TEST);
		else
			glDisable(GL_DEPTH_TEST);
		glDepthFunc(cmd.compare);
	}

	void blendSetup(const void* commandData, cb::RenderContext*)
	{
		const auto& cmd = *reinterpret_cast<const cmds::BlendSetup*>(commandData);
		if (cmd.enable)
			glEnable(GL_BLEND);
		else
			glDisable(GL_BLEND);
		glBlendFunc(cmd.src, cmd.dst);
	}

	void bindShader(const void* commandData, cb::RenderContext*)
	{
		const auto& cmd = *reinterpret_cast<const cmds::BindShader*>(commandData);
		glUseProgram(cmd.shader);
	}

	void bindTexture(const void* commandData, cb::RenderContext*)
	{
		const auto& cmd = *reinterpret_cast<const cmds::BindTexture*>(commandData);
		glActiveTexture(GL_TEXTURE0 + cmd.textureUnit);
		glBindTexture(cmd.type, cmd.texture);
	}

	void bindUniformBuffer(const void* commandData, cb::RenderContext*)
	{
		const auto& cmd = *reinterpret_cast<const cmds::BindUniformBuffer*>(commandData);
		glBindBufferBase(GL_UNIFORM_BUFFER, cmd.location, cmd.ubo);
	}

	void bindFramebuffer(const void* commandData, cb::RenderContext*)
	{
		const auto& cmd = *reinterpret_cast<const cmds::BindFramebuffer*>(commandData);
		glBindFramebuffer(cmd.target, cmd.fbo);
	}

	void unbindFramebuffer(const void* commandData, cb::RenderContext*)
	{
		const auto& cmd = *reinterpret_cast<const cmds::BindFramebuffer*>(commandData);
		glBindFramebuffer(cmd.target, 0);
	}

	void vertexBufferUpdate(const void* commandData, cb::RenderContext*)
	{
		const auto& cmd = *reinterpret_cast<const cmds::VertexBufferUpdate*>(commandData);
		glBindBuffer(cmd.target, cmd.buffer);
		glBufferSubData(cmd.target, cmd.offset, cmd.size, cmd.src);
	}

	uint8_t* lockBuffer(GLenum target, GLuint buffer, size_t offset, size_t length, bool isWriteOnly, BufferLockType lockType)
	{
		glBindBuffer(target, buffer);

		GLenum access = 0;
		if (isWriteOnly)
		{
			access = GL_MAP_WRITE_BIT;
			access |= GL_MAP_FLUSH_EXPLICIT_BIT;
			if (lockType == BufferLockType::eDiscard)
			{
				// Discard the buffer
				access |= GL_MAP_INVALIDATE_RANGE_BIT;
			}
		}
		else if (lockType == BufferLockType::eReadOnly)
			access = GL_MAP_READ_BIT;
		else
			access = GL_MAP_READ_BIT | GL_MAP_WRITE_BIT;

		return reinterpret_cast<uint8_t*>(glMapBufferRange(target, (GLintptr)offset, (GLsizeiptr)length, access));
	}

	void unlockBuffer(GLenum target, GLuint buffer, size_t lockStart, size_t lockSize, bool isWriteOnly)
	{
		glBindBuffer(target, buffer);
		// flush for write only buffer
		if (isWriteOnly)
			glFlushMappedBufferRange(target, lockStart, lockSize);

		GLboolean success = glUnmapBuffer(target);
		assert(success);
	}

	void vertexBufferCopy(const void* commandData, cb::RenderContext*)
	{
		const auto& cmd = *reinterpret_cast<const cmds::VertexBufferCopy*>(commandData);

#ifdef GL_ARB_copy_buffer
		// faster way to copy
		glBindBuffer(GL_COPY_READ_BUFFER, cmd.srcBuffer);
		glBindBuffer(GL_COPY_WRITE_BUFFER, cmd.dstBuffer);
		glCopyBufferSubData(GL_COPY_READ_BUFFER, GL_COPY_WRITE_BUFFER, (GLintptr)cmd.srcOffset, (GLintptr)cmd.dstOffset,
			(GLintptr)cmd.size);

		glBindBuffer(GL_COPY_READ_BUFFER, 0);
		glBindBuffer(GL_COPY_WRITE_BUFFER, 0);
#else
		const uint8_t* srcPtr = lockBuffer(cmd.target, cmd.srcBuffer, cmd.srcOffset, cmd.size, false, BufferLockType::eReadOnly);

		glBindBuffer(cmd.target, cmd.dstBuffer);
		glBufferSubData(cmd.target, cmd.dstBuffer, cmd.size, srcPtr);

		unlockBuffer(cmd.target, cmd.srcBuffer, cmd.srcOffset, cmd.size, false);
#endif  // #ifdef GL_ARB_copy_buffer
	}

	const cb::RenderContext::function_t DrawArrays::kDispatchFunction = &drawArrays;
	const cb::RenderContext::function_t DrawIndexed::kDispatchFunction = &drawIndexed;
	const cb::RenderContext::function_t DrawInstanced::kDispatchFunction = &drawInstanced;
	const cb::RenderContext::function_t ClearColor::kDispatchFunction = &clearColor;
	const cb::RenderContext::function_t DepthSetup::kDispatchFunction = &depthSetup;
	const cb::RenderContext::function_t BlendSetup::kDispatchFunction = &blendSetup;
	const cb::RenderContext::function_t BindShader::kDispatchFunction = &bindShader;
	const cb::RenderContext::function_t BindTexture::kDispatchFunction = &bindTexture;
	const cb::RenderContext::function_t BindUniformBuffer::kDispatchFunction = &bindUniformBuffer;
	const cb::RenderContext::function_t BindFramebuffer::kDispatchFunction = &bindFramebuffer;
	const cb::RenderContext::function_t UnbindFramebuffer::kDispatchFunction = &unbindFramebuffer;
	const cb::RenderContext::function_t VertexBufferUpdate::kDispatchFunction = &vertexBufferUpdate;
}

#ifndef USE_GLEW
#undef GL_GLEXT_PROTOTYPES
#endif
