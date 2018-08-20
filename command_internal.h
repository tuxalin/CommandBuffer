//
//  command_internal.h
//

#pragma once

#include "config.h"

#include <type_traits>

#if CB_COMMAND_PACKET_ALIGNED == 1
#define CB_COMMAND_PACKET_ALIGN() cb::detail::CommandPacketAlign<alignof(cb::CommandPacket)> alignment;
#else
#define CB_COMMAND_PACKET_ALIGN()
#endif

namespace cb
{
	namespace detail
	{
		template <typename T, bool IsClass = false>
		struct has_pod_hint_tag_impl
		{
			static const bool value = false;
		};
		template <typename T>
		struct has_pod_hint_tag_impl<T, true>
		{
			struct fallback
			{
				typedef void pod_hint_tag;
			};
			struct derived : T, fallback
			{
			};

			template <typename U>
			struct check;

			typedef char no[1];
			typedef char yes[2];

			template <typename U>
			static no& test(check<typename U::pod_hint_tag>*);

			template <typename U>
			static yes& test(...);

		public:
			static const bool value = sizeof(test<derived>(0)) == sizeof(yes);
		};
		template <typename T>
		struct has_pod_hint_tag : detail::has_pod_hint_tag_impl<T, std::is_class<T>::value>
		{
			typedef has_pod_hint_tag<T> type;
		};

		template <typename T, typename TT = void>
		struct is_pod : std::integral_constant<bool, std::is_pod<T>::value | has_pod_hint_tag<T>::value>
		{
			static const bool value = std::is_pod<T>::value | has_pod_hint_tag<T>::value;
		};

#if CB_COMMAND_PACKET_ALIGNED
		template <int Alignment>
		struct CommandPacketAlign
		{
		};

		template <>
		struct CommandPacketAlign<8>
		{
			uint64_t dummy;
		};

		template <>
		struct CommandPacketAlign<4>
		{
			uint32_t dummy;
		};
#endif  // #if CB_COMMAND_PACKET_ALIGNED
	} // namespace detail
} // namespace cb
