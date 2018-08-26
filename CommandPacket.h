//
//  CommandPacket.h
//

#pragma once

#include <cstdint>

#include "RenderContext.h"
#include "command_debug.h"
#include "command_internal.h"

namespace cb
{
	/// Contains one or more commands packed together in a specific order.
	struct CommandPacket
	{
		CB_DECLARE_COMMAND_DEBUG();

		template <class CommandClass>
		struct is_valid_type
		{
			enum
			{
				// must not be CommandPacket and must be pod, not void
				value = std::is_same<CommandPacket, CommandClass>::value == 0 &&  //
				std::is_void<CommandClass>::value == 0 && cb::detail::is_pod<CommandClass>::value
			};
		};

		template <class CommandClass, class AllocatorClass>
		static CommandPacket* create(AllocatorClass& allocator, uint32_t auxilarySize);

		template <class CommandClass>
		static CommandPacket* getCommandPacket(CommandClass* commandData);
		template <class CommandClass>
		static const CommandPacket* getCommandPacket(const CommandClass* commandData);
		template <class CommandClass>
		static CommandClass* getCommandData(CommandPacket* packet);
		template <class CommandClass, typename CastType>
		static CastType* getAuxilaryData(CommandClass* commandData);

		static void dispatch(const cb::CommandPacket* packet, cb::RenderContext* rc);

#if CB_DEBUG_TAG_COMMANDS
		template <class CommandClass>
		static CommandPacket& command(CommandClass& cmd);
		template <class CommandClass>
		static CommandPacket& command(CommandClass* cmd);
		template <class CommandClass>
		static std::string commandName(CommandClass& cmd);
		template <class CommandClass>
		static std::string commandName(CommandClass* cmd);

		typedef int(*log_function_t)(const char* fmt, ...);
		static void log(const cb::CommandPacket* packet, log_function_t logger);
#endif  // #if CB_DEBUG_TAG_COMMANDS

	private:
		// not allowed
		template <class CommandClass, typename CastType>
		static CastType* getAuxilaryData(CommandClass**);
		template <class CommandClass>
		static CommandPacket* getCommandPacket(CommandClass**);
		template <class CommandClass>
		static const CommandPacket* getCommandPacket(const CommandClass**);

	public:
		cb::RenderContext::function_t dispatchFunction;
		void*                         commandData;
		void*                         auxilaryData;  // optional
		CommandPacket*                nextCommand;
	};

	//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

	template <class CommandClass, class AllocatorClass>
	CB_FORCE_INLINE CommandPacket* CommandPacket::create(AllocatorClass& allocator, uint32_t auxilarySize)
	{
		static_assert(is_valid_type<CommandClass>::value, "COMMAND_INVALID_TYPE");

#if CB_COMMAND_PACKET_ALIGNED
		static_assert(alignof(CommandPacket) == alignof(CommandClass), "COMMAND_MUST_BE_ALIGNED_WITH_PACKET");
		const uint32_t alignment = alignof(CommandClass);
#else
		const uint32_t a1 = alignof(CommandPacket);
		const uint32_t a2 = alignof(CommandClass);
		const uint32_t alignment = std::max(a1, a2);
#endif

		uint8_t* data = allocator.alloc(sizeof(CommandPacket) + sizeof(CommandClass) + auxilarySize, alignment);

		CommandPacket* packet = reinterpret_cast<CommandPacket*>(data);
		packet->commandData = data + sizeof(CommandPacket);
		packet->auxilaryData = data + sizeof(CommandPacket) + sizeof(CommandClass);
#ifndef NDEBUG
		if (auxilarySize == 0)
			packet->auxilaryData = NULL;
#endif
		packet->nextCommand = NULL;

		return packet;
	}

	template <class CommandClass>
	CB_FORCE_INLINE CommandPacket* CommandPacket::getCommandPacket(CommandClass* commandData)
	{
		static_assert(is_valid_type<CommandClass>::value, "COMMAND_INVALID_TYPE");
		return reinterpret_cast<CommandPacket*>(reinterpret_cast<uint8_t*>(commandData) - sizeof(CommandPacket));
	}

	template <class CommandClass>
	CB_FORCE_INLINE const CommandPacket* CommandPacket::getCommandPacket(const CommandClass* commandData)
	{
		static_assert(is_valid_type<CommandClass>::value, "COMMAND_INVALID_TYPE");
		return reinterpret_cast<const CommandPacket*>(reinterpret_cast<const uint8_t*>(commandData) -
			sizeof(CommandPacket));
	}

	template <class CommandClass>
	CB_FORCE_INLINE CommandClass* CommandPacket::getCommandData(CommandPacket* packet)
	{
		static_assert(is_valid_type<CommandClass>::value, "COMMAND_INVALID_TYPE");
		return reinterpret_cast<CommandClass*>(packet->commandData);
	}

	template <class CommandClass, typename CastType>
	CB_FORCE_INLINE CastType* CommandPacket::getAuxilaryData(CommandClass* commandData)
	{
		static_assert(is_valid_type<CommandClass>::value, "COMMAND_INVALID_TYPE");
		CommandPacket* packet = CommandPacket::getCommandPacket<CommandClass>(commandData);
		return reinterpret_cast<CastType*>(packet->auxilaryData);
	}

	CB_FORCE_INLINE void CommandPacket::dispatch(const cb::CommandPacket* packet, cb::RenderContext* rc)
	{
		do
		{
			(*packet->dispatchFunction)(packet->commandData, rc);
			packet = packet->nextCommand;
		} while (packet != NULL);
	}

#if CB_DEBUG_TAG_COMMANDS
	template <class CommandClass>
	CB_FORCE_INLINE CommandPacket& CommandPacket::command(CommandClass& cmd)
	{
		return *CommandPacket::getCommandPacket<CommandClass>(&cmd);
	}

	template <class CommandClass>
	CB_FORCE_INLINE CommandPacket& CommandPacket::command(CommandClass* cmd)
	{
		return *CommandPacket::getCommandPacket<CommandClass>(cmd);
	}

	template <class CommandClass>
	CB_FORCE_INLINE std::string CommandPacket::commandName(CommandClass& cmd)
	{
		return typeid(&cmd).name();
	}

	template <class CommandClass>
	CB_FORCE_INLINE std::string CommandPacket::commandName(CommandClass* cmd)
	{
		return typeid(cmd).name();
	}

	CB_FORCE_INLINE void CommandPacket::log(const cb::CommandPacket* packet, log_function_t logger)
	{
		do
		{
			if (packet->nextCommand != NULL)
				(*logger)("%s ->", packet->debug.tag);
			else
				(*logger)("%s", packet->debug.tag);
			packet = packet->nextCommand;
		} while (packet != NULL);
		(*logger)("\n");
	}
#endif  // #if CB_DEBUG_TAG_COMMANDS

}  // namespace cb
