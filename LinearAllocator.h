//
//  LinearAllocator.h
//

#pragma once

#include <atomic>

#include "MemoryUtil.h"

namespace cb
{
	/// Thread safe linear allocator.
	///@note If alignment is zero then it'll auto align.
	template <int Alignment = 0>
	class LinearAllocator
	{
	public:
		explicit LinearAllocator(uint32_t size);
		LinearAllocator(uint8_t* data, uint32_t size);
		~LinearAllocator();

		uint8_t* alloc(uint32_t bytes, uint32_t alignment = Alignment);
		void dealloc(uint8_t* p);
		void deallocAll();

		void resize(uint32_t size);

		size_t size() const;

	private:
		uint32_t              m_size;  // total size of the allocated memory
		uint8_t*              m_start;
		std::atomic<uint32_t> m_current;

		LinearAllocator(const LinearAllocator&) = delete;
		void operator=(const LinearAllocator&) = delete;
	};

	//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

	template <int Alignment>
	inline LinearAllocator<Alignment>::LinearAllocator(uint32_t size)
		: m_size(size + size % (Alignment ? Alignment : sizeof(uint32_t)))  // aligned to the given alignment or to 4 bytes
		, m_start(new uint8_t[m_size]())
		, m_current(0)
	{
		assert(m_current.is_lock_free());
		assert(m_size % sizeof(uint32_t) == 0);
	}

	template <int Alignment>
	inline LinearAllocator<Alignment>::LinearAllocator(uint8_t* data, uint32_t size)
		: m_size(size)
		, m_start(data)
		, m_current(0)
	{
		assert(m_current.is_lock_free());
		assert(m_size % sizeof(uint32_t) == 0);
	}

	template <int Alignment>
	inline LinearAllocator<Alignment>::~LinearAllocator()
	{
		delete[] m_start;
	}

	template <int Alignment>
	inline uint8_t* LinearAllocator<Alignment>::alloc(uint32_t bytes, uint32_t alignment)
	{
		assert(bytes);

		uint8_t* currentOffset;
		if (Alignment)
		{
			assert(alignment == Alignment);

			// always allocate aligned data
			const uint32_t kAlignment = alignment ? Alignment : 1;  // condition just to suppress warnings
			bytes += bytes % kAlignment;

			uint32_t current = m_current.fetch_add(bytes, std::memory_order_relaxed);
			currentOffset = m_start + current;

			assert(cb::mem::alignForwardPadding(currentOffset, alignment) == 0);
			assert((current + bytes) < m_size);
		}
		else
		{
			// must align by adding padding
			uint32_t current, padding;
			do
			{
				current = m_current.load(std::memory_order_acquire);

				currentOffset = m_start + current;
				padding = cb::mem::alignForwardPadding(currentOffset, alignment);
				currentOffset += padding;
				assert((current + padding + bytes) < m_size);
				// retry if the current offset has changed
			} while (!m_current.compare_exchange_weak(current, current + padding + bytes, std::memory_order_release));
		}

		return currentOffset;
	}

	template <int Alignment>
	inline void LinearAllocator<Alignment>::dealloc(uint8_t*)
	{
		// must use deallocAll
		assert(false);
	}

	template <int Alignment>
	inline void LinearAllocator<Alignment>::deallocAll()
	{
		m_current.store(0, std::memory_order_release);
	}

	template <int Alignment>
	inline void LinearAllocator<Alignment>::resize(uint32_t size)
	{
		if (size == m_size)
			return;

		deallocAll();

		delete[] m_start;

		m_size = size + size % (Alignment ? Alignment : sizeof(uint32_t));
		m_start = new uint8_t[m_size]();
	}

	template <int Alignment>
	inline size_t LinearAllocator<Alignment>::size() const
	{
		return m_current;
	}
}  // namespace cb
