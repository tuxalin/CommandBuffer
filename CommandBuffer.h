//
//  CommandBuffer.h
//

#pragma once

#include <algorithm>
#include <atomic>
#include <cstdint>
#include <functional> 
#include <vector>

#include "CommandKeys.h"
#include "CommandPacket.h"
#include "LinearAllocator.h"

namespace cb
{
	struct DefaultMaterialBinder
	{
		///@note Returns true if there are more passes to bind.
		CB_FORCE_INLINE bool operator()(cb::MaterialId material) const
		{
			(void)(material);
			return false;
		}

#if CB_DEBUG_COMMANDS_PRINT
		void debugMsg(cb::MaterialId) {}
#endif
	};

	struct DefaultKeyDecoder
	{
		/// note Each command key must define a decode function that will be used by the command buffer.
		CB_FORCE_INLINE cb::MaterialId operator()(cb::DrawKey key) const
		{
			return key.material();
		}
	};

	template<typename T>
	struct DummyKeyDecoder
	{
		CB_FORCE_INLINE cb::MaterialId operator()(T) const
		{
			return cb::MaterialId(0, 0);
		}
	};

	/// Utility to create global functions for commands with execute member method.
	template<class CommandClass>
	void makeExecuteFunction(const void* data, cb::RenderContext* rc)
	{
		auto& cmd = *reinterpret_cast<const CommandClass*>(data);
		cmd.execute();
	}

	template <typename KeyType>
	struct CommandPair
	{
		CommandPacket* cmd;
		KeyType        key;

		CB_FORCE_INLINE bool operator<(const CommandPair& other) const
		{
			assert(cmd);
			return key < other.key;
		}

		template<typename T>
		explicit operator T()
		{
			return(T)key;
		}
	};

	/// The command buffer is composed of more CommandPackets and their corresponding keys.
	template <typename KeyType = cb::DrawKey, class KeyDecoderClass = DefaultKeyDecoder, class MaterialBinderClass = DefaultMaterialBinder>
	class CommandBuffer
	{
	public:
		typedef KeyType key_t;
		typedef CommandPair<key_t> command_t;
		typedef std::function<void(command_t*, command_t*)> sort_func_t;
		typedef MaterialBinderClass binder_t;
		typedef KeyDecoderClass decoder_t;
		typedef int(*log_function_t)(const char* fmt, ...);

		explicit CommandBuffer(uint32_t commandCount = 5000, uint32_t commandKBytes = 512);
		explicit CommandBuffer(const MaterialBinderClass& materialBinder);

		MaterialBinderClass& materialBinder();
		const MaterialBinderClass& materialBinder() const;

		/// Returns the count of the commands in the buffer.
		/// @param countChainCommands - will also count chained commands
		size_t count(bool countChainCommands = false) const;
		/// Returns the consumed memory of the commands in the buffer, in bytes.
		size_t allocations() const;
		///@warning Should never resize when dispatching commands in progress, only before.
		void resize(uint32_t commandCount, uint32_t commandKBs);
		/// Sorts the created commands based on their key priority.
		void sort(sort_func_t sortFunc = std::sort<command_t*>);
		/// Submits the sorted commands to the GPU.
		/// @param clearBuffer - clear all created commands from the buffer.
		void submit(cb::RenderContext* rc, bool clearBuffer = true);

		/// Creates a new command in the command buffer.
		///@param auxilarySize Size of auxiliary memory required by the command.
		///@tparam CommandClass must be a POD and must provide a kDispatchFunction member of
		/// cb::RenderContext::function_t.
		///@note Must use CommandPacket::getAuxilaryData to get the auxilary data pointer.
		template <class CommandClass>
		CommandClass* addCommand(const key_t& key, uint32_t auxilarySize = 0);
		///@tparam AuxilaryData must be POD
		///@note Also copies data from the given auxiliary data.
		template <class CommandClass, typename AuxilaryData>
		CommandClass* addCommandData(const key_t& key, const AuxilaryData& data);
		template <class CommandClass, typename AuxilaryData>
		CommandClass* addCommandData(const key_t& key, const std::vector<AuxilaryData>& data);
		template <class CommandClass, typename AuxilaryData>
		CommandClass* addCommandData(const key_t& key, const AuxilaryData* data, uint32_t count);
		/// Adds the given command packet with the given key.
		///@note Only references the original packet's auxiliary data.
		cb::CommandPacket* addCommandFrom(const key_t& key, const cb::CommandPacket* referencePacket);

		/// Adds a new command and append(chain) it to the given command.
		///@tparam  CommandClass must be a POD and must provide a kDispatchFunction member of
		/// cb::RenderContext::function_t.
		///@note Must use CommandPacket::getAuxilaryData to get the auxilary data pointer.
		template <class CommandClass, class AppendCommandClass>
		CommandClass* appendCommand(AppendCommandClass* cmd, uint32_t auxilarySize = 0);
		template <class CommandClass>
		CommandClass* appendCommand(cb::CommandPacket* cmd, uint32_t auxilarySize = 0);
		///@tparam AuxilaryData must be POD
		///@note Also copies data from the given auxiliary data.
		template <class CommandClass, class AppendCommandClass, typename AuxilaryData>
		CommandClass* appendCommandData(AppendCommandClass* cmd, const AuxilaryData& data);
		template <class CommandClass, class AppendCommandClass, typename AuxilaryData>
		CommandClass* appendCommandData(AppendCommandClass* prevCmd, const std::vector<AuxilaryData>& data);
		template <class CommandClass, class AppendCommandClass, typename AuxilaryData>
		CommandClass* appendCommandData(AppendCommandClass* prevCmd, const AuxilaryData* data, uint32_t count);
		template <class CommandClass, typename AuxilaryData>
		CommandClass* appendCommandData(cb::CommandPacket* cmd, const AuxilaryData& data);

		///@tparam  CommandClass must be a POD and must provide a kDispatchFunction member of
		/// cb::RenderContext::function_t.
		template <class CommandClass>
		cb::CommandPacket* createCommandPacket(uint32_t auxilarySize = 0);
		template <class CommandClass, typename AuxilaryData>
		cb::CommandPacket* createCommandPacketData(const AuxilaryData& data);

#if CB_DEBUG_COMMANDS_PRINT
		void setLogFunction(log_function_t logger) { m_logger = logger; }
#endif

	private:
		void clear();

	private:
		struct CommandPacketReference
		{
			CB_COMMAND_PACKET_ALIGN()
		};

#if CB_COMMAND_PACKET_ALIGNED
		static const uint32_t kALignment = alignof(CommandPacket);
#else
		static const uint32_t kALignment = 0;
#endif

		cb::LinearAllocator<kALignment>	m_allocator;
		MaterialBinderClass				m_materialBinder;
		std::vector<command_t>			m_commands;
		std::atomic<uint32_t>			m_currentIndex;
#if CB_DEBUG_COMMANDS_PRINT
		log_function_t m_logger = printf;
		std::stringstream m_stringStream;
#endif
	private:
		CommandBuffer(const CommandBuffer&) = delete;
		void operator=(const CommandBuffer&) = delete;
	};  // class CommandBuffer

	//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#define COMMAND_TEMPLATE template <typename KeyType, class KeyDecoderClass, class MaterialBinderClass>
#define COMMAND_QUAL CommandBuffer<KeyType, KeyDecoderClass, MaterialBinderClass>

	COMMAND_TEMPLATE
		COMMAND_QUAL::CommandBuffer(uint32_t commandCount, uint32_t commandKBytes)
		: m_allocator(commandKBytes * 1024)  // TODO: can use TLS to avoid false sharing
		, m_materialBinder()
		, m_currentIndex(0)
	{
		assert(m_currentIndex.is_lock_free());

		m_commands.resize(commandCount);
	}

	COMMAND_TEMPLATE
		COMMAND_QUAL::CommandBuffer(const MaterialBinderClass& materialBinder)
		: m_allocator(kDefaultCommandKBs * 1024)  // TODO: can use TLS to avoid false sharing
		, m_materialBinder(materialBinder)
		, m_currentIndex(0)
	{
		assert(m_currentIndex.is_lock_free());

		m_commands.resize(kDefaultCommandCount);
	}

	COMMAND_TEMPLATE
		MaterialBinderClass& COMMAND_QUAL::materialBinder()
	{
		return m_materialBinder;
	}

	COMMAND_TEMPLATE
		const MaterialBinderClass& COMMAND_QUAL::materialBinder() const
	{
		return m_materialBinder;
	}

	COMMAND_TEMPLATE
		size_t COMMAND_QUAL::count(bool countChainCommands /*= false*/) const
	{
		if (!countChainCommands)
			return m_currentIndex;

		size_t res = 0;
		const auto end = m_commands.begin() + (int)m_currentIndex.load(std::memory_order_acquire);
		for (auto it = m_commands.begin(); it != end; ++it)
		{
			const CommandPacket* packet = it->cmd;
			do
			{
				++res;
				packet = packet->nextCommand;
			} while (packet != NULL);
		}
		return res;
	}

	COMMAND_TEMPLATE
		size_t COMMAND_QUAL::allocations() const
	{
		return m_allocator.size();
	}

	COMMAND_TEMPLATE
		void COMMAND_QUAL::resize(uint32_t commandCount, uint32_t commandKBs)
	{
		assert(m_currentIndex == 0);
		m_commands.resize(commandCount);
		m_allocator.resize(commandKBs * 1024);
	}

	COMMAND_TEMPLATE
		void COMMAND_QUAL::submit(cb::RenderContext* rc, bool clearBuffer /*= true*/)
	{
		// dispatches commands
#if CB_DEBUG_COMMANDS_PRINT
		(*m_logger)("\n\n++++ Submit ++++\n\n");
#endif
		const auto end = m_commands.begin() + (int)m_currentIndex.load(std::memory_order_acquire);
		for (auto it = m_commands.begin(); it != end; ++it)
		{
			const key_t& key = it->key;

			// decode material id from key
			cb::MaterialId material = KeyDecoderClass()(key);

#if CB_DEBUG_COMMANDS_PRINT
			m_stringStream.str(std::string());
			m_stringStream.clear();
			m_stringStream << key;
			(*m_logger)("%s\n", m_stringStream.str().c_str());
#endif
			const CommandPacket* packet = it->cmd;
			// apply the material dispatch commands
			bool nextPass;
			do
			{
				// if true then we have more passes presents in the material
				nextPass = m_materialBinder(material);
#if CB_DEBUG_COMMANDS_PRINT
				m_materialBinder.debugMsg(material);
				CommandPacket::log(packet, *m_logger);
#endif
				CommandPacket::dispatch(packet, rc);
				++material.pass;
			} while (nextPass);
		}

		// safe to dealloc all
		if (clearBuffer)
			clear();
	}

	COMMAND_TEMPLATE
		template <class CommandClass>
	CommandClass* COMMAND_QUAL::addCommand(const key_t& key, uint32_t auxilarySize)
	{
		CommandPacket* packet = CommandPacket::create<CommandClass>(m_allocator, auxilarySize);
		// store the key and command packet ptr
		{
			const uint32_t currentIndex = m_currentIndex.fetch_add(1, std::memory_order_relaxed);
			assert(currentIndex < (uint32_t)m_commands.size());
			command_t& pair = m_commands[currentIndex];
			pair.cmd = packet;
			pair.key = key;
		}
		packet->dispatchFunction = CommandClass::kDispatchFunction;
		assert(packet->dispatchFunction);

		return CommandPacket::getCommandData<CommandClass>(packet);
	}

	COMMAND_TEMPLATE
		template <class CommandClass, typename AuxilaryData>
	CommandClass* COMMAND_QUAL::addCommandData(const key_t& key, const AuxilaryData& data)
	{
		return addCommandData<CommandClass>(key, &data, 1);
	}

	COMMAND_TEMPLATE
		template <class CommandClass, typename AuxilaryData>
	CommandClass* COMMAND_QUAL::addCommandData(const key_t& key, const std::vector<AuxilaryData>& data)
	{
		return addCommandData<CommandClass>(key, data.data(), data.size());
	}

	COMMAND_TEMPLATE
		template <class CommandClass, typename AuxilaryData>
	CommandClass* COMMAND_QUAL::addCommandData(const key_t& key, const AuxilaryData* data, uint32_t count)
	{
		static_assert(cb::detail::is_pod<AuxilaryData>::value, "AUXILARY_DATA_INVALID_TYPE");

		const uint32_t size = sizeof(AuxilaryData) * count;
		CommandClass*  cmd = addCommand<CommandClass>(key, size);

		CommandPacket* packet = CommandPacket::getCommandPacket<CommandClass>(cmd);
		memcpy(packet->auxilaryData, data, size);

		// enforce that the command class has data and size members
		cmd->data = packet->auxilaryData;
		cmd->size = size;
		return cmd;
	}

	COMMAND_TEMPLATE
		cb::CommandPacket* COMMAND_QUAL::addCommandFrom(const key_t& key, const cb::CommandPacket* referencePacket)
	{
		assert(referencePacket->nextCommand == NULL);

		CommandPacket* packet = CommandPacket::create<CommandPacketReference>(m_allocator, 0);
		// store the key and command packet ptr
		{
			const uint32_t currentIndex = m_currentIndex.fetch_add(1, boost::memory_order_relaxed);
			assert(m_commands.size() > currentIndex);
			assert(currentIndex < (uint32_t)m_commands.size());
			CommandPair& pair = m_commands[currentIndex];
			pair.cmd = packet;
			pair.key = key;
		}

		// reference the packet
		*packet = *referencePacket;
		assert(packet->nextCommand == NULL);
		assert(packet->dispatchFunction);
		return packet;
	}

	COMMAND_TEMPLATE
		template <class CommandClass>
	CommandClass* COMMAND_QUAL::appendCommand(cb::CommandPacket* prevPacket, uint32_t auxilarySize)
	{
		CommandPacket* packet = CommandPacket::create<CommandClass>(m_allocator, auxilarySize);
		packet->dispatchFunction = CommandClass::kDispatchFunction;
		assert(packet->dispatchFunction);

		assert(prevPacket->nextCommand == NULL);
		prevPacket->nextCommand = packet;

		return CommandPacket::getCommandData<CommandClass>(packet);
	}

	COMMAND_TEMPLATE
		template <class CommandClass, class AppendCommandClass>
	CommandClass* COMMAND_QUAL::appendCommand(AppendCommandClass* prevCmd, uint32_t auxilarySize)
	{
		CommandPacket* packet = CommandPacket::create<CommandClass>(m_allocator, auxilarySize);
		packet->dispatchFunction = CommandClass::kDispatchFunction;
		assert(packet->dispatchFunction);

		CommandPacket* prevPacket = CommandPacket::getCommandPacket<AppendCommandClass>(prevCmd);
		assert(prevPacket->nextCommand == NULL);
		prevPacket->nextCommand = packet;

		return CommandPacket::getCommandData<CommandClass>(packet);
	}

	COMMAND_TEMPLATE
		template <class CommandClass, class AppendCommandClass, typename AuxilaryData>
	CommandClass* COMMAND_QUAL::appendCommandData(AppendCommandClass* prevCmd, const AuxilaryData& data)
	{
		return appendCommandData<CommandClass>(prevCmd, &data, 1);
	}

	COMMAND_TEMPLATE
		template <class CommandClass, class AppendCommandClass, typename AuxilaryData>
	CommandClass* COMMAND_QUAL::appendCommandData(AppendCommandClass* prevCmd, const std::vector<AuxilaryData>& data)
	{
		return appendCommandData<CommandClass>(prevCmd, data.data(), data.size());
	}

	COMMAND_TEMPLATE
		template <class CommandClass, class AppendCommandClass, typename AuxilaryData>
	CommandClass* COMMAND_QUAL::appendCommandData(AppendCommandClass* prevCmd, const AuxilaryData* data, uint32_t count)
	{
		static_assert(cb::detail::is_pod<AuxilaryData>::value, "AUXILARY_DATA_INVALID_TYPE");

		const uint32_t size = sizeof(AuxilaryData) * count;
		CommandClass*  cmd = appendCommand<CommandClass>(prevCmd, size);

		CommandPacket* packet = CommandPacket::getCommandPacket<CommandClass>(cmd);
		memcpy(packet->auxilaryData, data, size);

		// enforce that the command class has data and size members
		cmd->data = packet->auxilaryData;
		cmd->size = size;
		return cmd;
	}

	COMMAND_TEMPLATE
		template <class CommandClass, typename AuxilaryData>
	CommandClass* COMMAND_QUAL::appendCommandData(cb::CommandPacket* prevPacket, const AuxilaryData& data)
	{
		CommandClass* cmd = appendCommand<CommandClass>(prevPacket, sizeof(AuxilaryData));

		CommandPacket* packet = CommandPacket::getCommandPacket<CommandClass>(cmd);
		memcpy(packet->auxilaryData, &data, sizeof(AuxilaryData));

		// enforce that the command class has data and size members
		cmd->data = packet->auxilaryData;
		cmd->size = sizeof(AuxilaryData);
		return cmd;
	}

	COMMAND_TEMPLATE
		template <class CommandClass>
	cb::CommandPacket* COMMAND_QUAL::createCommandPacket(uint32_t auxilarySize /*= 0*/)
	{
		CommandPacket* packet = CommandPacket::create<CommandClass>(m_allocator, auxilarySize);
		packet->dispatchFunction = CommandClass::kDispatchFunction;
		assert(packet->dispatchFunction);
		return packet;
	}

	COMMAND_TEMPLATE
		template <class CommandClass, typename AuxilaryData>
	cb::CommandPacket* COMMAND_QUAL::createCommandPacketData(const AuxilaryData& data)
	{
		CommandPacket* packet = createCommandPacket<CommandClass>(sizeof(AuxilaryData));
		memcpy(packet->auxilaryData, &data, sizeof(AuxilaryData));

		CommandClass* cmd = CommandPacket::getCommandData<CommandClass>(packet);
		// enforce that the command class has data and size members
		cmd->data = packet->auxilaryData;
		cmd->size = sizeof(AuxilaryData);
		return packet;
	}

	COMMAND_TEMPLATE
		void COMMAND_QUAL::sort(sort_func_t sortFunc /*= std::sort<CommandPair*>*/)
	{
		sortFunc(m_commands.data(), m_commands.data() + (int)m_currentIndex.load(std::memory_order_acquire));

		assert(m_commands.size() > m_currentIndex.load(std::memory_order_acquire));
	}

	COMMAND_TEMPLATE
		void COMMAND_QUAL::clear()
	{
		m_allocator.deallocAll();
		m_currentIndex = 0;
	}

#undef COMMAND_TEMPLATE
#undef COMMAND_QUAL

}  // namespace cb
