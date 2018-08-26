//
//  commands_debug.h
//

#pragma once

#include "config.h"

#if CB_DEBUG_TAG_COMMANDS

#include <algorithm>
#include <cassert>
#include <string>
#if CB_DEBUG_COMMANDS_PRINT
#include <sstream>
#endif

namespace cb
{
	namespace detail
	{
		struct packet_debug
		{
			static const int kSize = 128;

			void setTag(const std::string& str)
			{
				assert(str.size() < kSize);
				std::copy(str.begin(), str.end(), tag);
				tag[str.size()] = '\0';
			}
			void setTag(const std::string& str1, const std::string& str2)
			{
				size_t size = str1.size() + str2.size() + 3;
				assert(size < kSize);
				const std::string str = str1 + " : " + str2;
				std::copy(str.begin(), str.end(), tag);
				tag[size] = '\0';
			}
			void setTag(const std::string& str1, const std::string& str2, const std::string& str3)
			{
				size_t size = str1.size() + str2.size() + 3 + str3.size() + 3;
				assert(size < kSize);
				const std::string str = str1 + " : " + str2 + " : " + str3;
				std::copy(str.begin(), str.end(), tag);
				tag[size] = '\0';
			}

			char tag[kSize];
		};
	} // namespace detail
} // namespace cb

#define CB_DEBUG_STRINGIFY(x) #x
#define CB_DEBUG_TOSTRING(x) CB_DEBUG_STRINGIFY(x)

#define CB_DECLARE_COMMAND_DEBUG() cb::detail::packet_debug debug;

#define CB_DEBUG_COMMAND_TAG(cmd)                                                          \
    cb::CommandPacket::command(cmd).debug.setTag(std::string(__FUNCTION__), CB_DEBUG_TOSTRING(__LINE__), \
                                          cb::CommandPacket::commandName(cmd));

#define CB_DEBUG_COMMAND_TAG_MSG(cmd, msg)                                                 \
    cb::CommandPacket::command(cmd).debug.setTag(std::string(__FUNCTION__), CB_DEBUG_TOSTRING(__LINE__), \
                                          cb::CommandPacket::commandName(cmd) + " : " + msg);

#define CB_DEBUG_COMMAND_SET_MSG(cmd, msg) cb::CommandPacket::command(cmd).debug.setTag(msg);

#else

#define CB_DECLARE_COMMAND_DEBUG()

#define CB_DEBUG_COMMAND_TAG(cmd)

#define CB_DEBUG_COMMAND_TAG_MSG(cmd, msg)

#define CB_DEBUG_COMMAND_SET_MSG(cmd, msg)

#endif  // #ifdef CB_DEBUG_TAG_COMMANDS

