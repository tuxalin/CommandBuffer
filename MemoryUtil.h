//
//  MemoryUtil.h
//

#pragma once

#include <algorithm>
#include <cstddef>
#include <cstdint>

namespace cb
{
	namespace mem
	{
		/// @brief Computes the element count of a C array.
		/// @example int a[10];
		/// mem::size(a); == 10
		template <typename T, size_t N>
		size_t size(const T(&)[N])
		{
			return N;
		}

		template <typename T, size_t N, size_t M>
		size_t size(const T(&)[N][M])
		{
			return N * M;
		}

		template <typename T, size_t N>
		void fill(T(&buffer)[N], const T& value)
		{
			std::fill(buffer, buffer + N, value);
		}

		/// @brief Returns the offset of a class member.
		/// @example offsetOf(ClassB::m_data).
		template <typename T, typename U>
		size_t offsetOf(U T::*member)
		{
			const long offset = (char*)&((T*)NULL->*member) - (char*)NULL;
			assert(offset >= 0);
			return (size_t)offset;
		}

		/// @brief Returns true if an address is aligned to the type T.
		template <typename T>
		bool isAligned(const void* p)
		{
			const size_t alignment = alignof(T);
			const size_t address = reinterpret_cast<size_t>(p);
			return 0 == (address % alignment);
		}

		inline bool isBigEndian()
		{
			union {
				uint32_t i;
				char     c[4];
			} bint = { 0x01020304 };

			return bint.c[0] == 1;
		}

		inline bool isLittleEndian()
		{
			return !isBigEndian();
		}

		inline uint8_t* alignForward(uint8_t* address, uint32_t alignment)
		{
			const uint32_t a = alignment - 1;
			const uint32_t na = ~a;
			return (uint8_t*)((reinterpret_cast<uintptr_t>(address) + static_cast<uintptr_t>(a)) & static_cast<uintptr_t>(na));
		}

		inline uint32_t alignForwardPadding(const uint8_t* address, uint32_t alignment)
		{
			size_t adjustment = alignment - (reinterpret_cast<uintptr_t>(address) & static_cast<uintptr_t>(alignment - 1));

			if (adjustment == alignment)
				return 0;  // already aligned
			return static_cast<uint32_t>(adjustment);
		}

	}  // end of namespace mem
}  // end of namespace cb
