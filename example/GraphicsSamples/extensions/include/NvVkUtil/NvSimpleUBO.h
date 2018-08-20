//----------------------------------------------------------------------------------
// File:        NvVkUtil/NvSimpleUBO.h
// SDK Version: v3.00 
// Email:       gameworks@nvidia.com
// Site:        http://developer.nvidia.com/
//
// Copyright (c) 2014-2015, NVIDIA CORPORATION. All rights reserved.
//
// Redistribution and use in source and binary forms, with or without
// modification, are permitted provided that the following conditions
// are met:
//  * Redistributions of source code must retain the above copyright
//    notice, this list of conditions and the following disclaimer.
//  * Redistributions in binary form must reproduce the above copyright
//    notice, this list of conditions and the following disclaimer in the
//    documentation and/or other materials provided with the distribution.
//  * Neither the name of NVIDIA CORPORATION nor the names of its
//    contributors may be used to endorse or promote products derived
//    from this software without specific prior written permission.
//
// THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS ``AS IS'' AND ANY
// EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
// IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR
// PURPOSE ARE DISCLAIMED.  IN NO EVENT SHALL THE COPYRIGHT OWNER OR
// CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL,
// EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO,
// PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR
// PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY
// OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
// (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
// OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
//
//----------------------------------------------------------------------------------
#ifndef NV_SIMPLE_UBO
#define NV_SIMPLE_UBO

#include <NvSimpleTypes.h>
#include <NvVkUtil/NvSimpleSyncRing.h>
#include <NV/NvLogs.h>

/// A simple, start to end allocator of UBO memory for multiple UBOs from a single memory allocation
/// Does not support deletion - designed for allocations that last the lifetime of the app
/// A simple method of implementing suballocation
class NvSimpleUBOAllocator {
public:
	/// Initialize the allocator with the desired size
	/// \param[in] vk the queue/device to use
	/// \param[in] the size of the entire allocation pool in bytes
	/// \return true on success and false on failure
	bool Initialize(NvVkContext& vk, uint32_t size) {
		m_size = size;
		m_offset = 0;

		VkResult result = vk.createAndFillBuffer(size,
			VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT, m_buffer);
		if (result != VK_SUCCESS)
			return false;

		result = vkMapMemory(vk.device(), m_buffer.mem, 0, size, 0, (void**)&m_mapped);
		if (result != VK_SUCCESS)
			return false;

		return true;
	}

	/// Return the parent buffer used for all allocations
	NvVkBuffer& buffer() { return m_buffer; }

	/// Allocate a block of memory from the buffer, return its location in the buffer and a mapping if needed
	/// \param[in] size thew size of the block to allocate, in bytes
	/// \param[out] offset the offset into the buffer object of the resulting allocation
	/// \param[in,out] mapped if non-NULL, a pointer to a void* into which the CPU mapped address of the block should be written
	/// \return true on success and false on failure
	bool Alloc(uint32_t size, uint32_t& offset, void** mapped = NULL) {
		if (size + m_offset > m_size)
			return false;

		offset = m_offset;
		m_offset += size;

		if (mapped) {
			*mapped = m_mapped;
		}

		m_mapped = (void*)(((unsigned char*)m_mapped) + size);

		return true;
	}

protected:

	NvVkBuffer m_buffer;
	uint32_t m_size;
	uint32_t m_offset;
	void* m_mapped;
};

template <class UBOType, uint32_t m_copies = 4>
class NvSimpleSharedUBO {
public:
	NvSimpleSharedUBO() { }
	~NvSimpleSharedUBO() { }

	/// Returns the buffer space required for an instance of this object.  
	/// Used to estimate buffer sizes for shared allocation
	static uint32_t GetBufferSpace() { 
		return ((sizeof(UBOType) + 0xff) & 0xffffff00) * m_copies;
	}

	/// Initializes the UBO mapping, associating it with a queue and device
	/// \param[in] vk the device and queue to associate with the UBO
	/// an internal buffer can be safely re-used
	/// \param[in] ring the sync ring to use to round-robin the copies
	/// \return true on success and false on failure
	bool Initialize(NvVkContext& vk, NvSyncRing<m_copies>* ring) {
		m_ring = ring;
		m_device = vk.device();

		// round size up to 256 byte multiple - this is the stride
		m_stride = sizeof(UBOType);
		m_stride = (m_stride + 0xff) & 0xffffff00;

		uint32_t size = m_copies * m_stride;

		VkResult result = vk.createAndFillBuffer(size,
			VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT, m_ubo);
		if (result != VK_SUCCESS)
			return false;

		m_offset = 0;

		result = vkMapMemory(m_device, m_ubo.mem, m_offset, size, 0, (void**)&m_mapped);
		if (result != VK_SUCCESS)
			return false;

		return true;
	}

	/// Initializes the UBO mapping, associating it with a queue and device and using buffer sharing
	/// \param[in] vk the device and queue to associate with the UBO
	/// an internal buffer can be safely re-used
	/// \param[in] alloc the shared allocator to use
	/// \param[in] ring the sync ring to use to round-robin the copies
	/// \return true on success and false on failure
	bool Initialize(NvVkContext& vk, NvSimpleUBOAllocator& alloc, NvSyncRing<m_copies>* ring) {
		m_ring = ring;
		m_device = vk.device();

		// round size up to 256 byte multiple - this is the stride
		m_stride = sizeof(UBOType);
		m_stride = (m_stride + 0xff) & 0xffffff00;

		uint32_t size = m_copies * m_stride;

		if (!alloc.Alloc(size, m_offset, (void**)&m_mapped))
			return false;

		m_ubo = alloc.buffer();

		return true;
	}

	/// Release all API resources associated with the UBO
	void Finalize() {
		m_ring->Finish();
	}

	bool Update() {
		// All updates are unsynchronized right now
		return UpdateUnsynchronized();
	}

	/// Updates the current CPU-accessible structure values to a UBO
	/// copy without checking any synchronization.  If an app uses more
	/// than one UBO with the same sync object, then the FIRST update in the
	/// set must be synchronized via Update, while the other UBOs must be
	/// updated via this function
	/// \return true on success and false on failure
	bool UpdateUnsynchronized() {
		if (!m_ring->BeginUpdate())
			return false;

		uint32_t index = m_ring->getIndex();
		VkResult result;

		memcpy(((uint8_t*)m_mapped) + index * m_stride, &m_cpuCopy, sizeof(UBOType));

		VkMappedMemoryRange range = { VK_STRUCTURE_TYPE_MAPPED_MEMORY_RANGE };
		range.offset = m_offset + m_stride * index;
		range.size = m_stride;
		range.memory = m_ubo.mem;
		result = vkFlushMappedMemoryRanges(m_device, 1, &range);

		return result == VK_SUCCESS;
	}

	/// Return the address of the current UBO backing store to be used
	/// in draw calls; this value will change with every call to #Update,
	/// so take care when caching the value of this in a command buffer
	/// \return the BufferAddress of the current copy
	void GetDesc(VkDescriptorBufferInfo& desc) {
		desc.buffer = m_ubo();
		desc.offset = m_offset;
		desc.range = m_stride;
	}

	/// Get the dynamic offset into the UBO of the current copy of the data
	uint32_t getDynamicOffset() {
		return m_stride * m_ring->getIndex();
	}

	/// Accessor for easy mapping of the wrapper to the CPU-accessible structure
	/// Changes to the structure pointed to by this pointer will NOT be 
	/// reflected in a bound UBO until after a subsequent call to Update
	/// \return a pointer to the CPU accessible struct
	UBOType* operator-> () { return &m_cpuCopy; }

	/// Read-only Accessor for easy mapping of the wrapper to the CPU-accessible
	/// structure
	/// \return a const pointer to the CPU accessible struct
	const UBOType* operator-> () const { return &m_cpuCopy; }

	/// Accessor for easy mapping of the wrapper to the CPU-accessible structure
	/// Changes to the structure pointed to by this reference will NOT be 
	/// reflected in a bound UBO until after a subsequent call to Update
	/// \return a reference to the CPU accessible struct
	UBOType& operator* () { return m_cpuCopy; }

	/// Read-only Accessor for easy mapping of the wrapper to the
	/// CPU-accessible structure
	/// \return a const reference to the CPU accessible struct
	const UBOType& operator* () const { return m_cpuCopy; }

	/// Accessor for easy mapping of the wrapper to the CPU-accessible
	/// structure
	/// Changes to the structure pointed to by this pointer will NOT be 
	/// reflected in a bound UBO until after a subsequent call to Update
	/// \return a pointer to the CPU accessible struct
	operator UBOType*() { return &m_cpuCopy; }

protected:
	NvSyncRing<m_copies>*	m_ring;
	VkDevice	m_device;
	UBOType		m_cpuCopy;
	UBOType*	m_mapped;
	uint32_t	m_stride;
	uint32_t	m_offset;
	NvVkBuffer  m_ubo;
};

template <class UBOType, uint32_t m_copies = 4>
class NvSimpleUBO {
public:
	NvSimpleUBO() { }
	~NvSimpleUBO() { }

	/// Returns the buffer space required for an instance of this object.  
	/// Used to estimate buffer sizes for shared allocation
	static uint32_t GetBufferSpace() {
		return ((sizeof(UBOType) + 0xff) & 0xffffff00) * m_copies;
	}

	/// Initializes the UBO mapping, associating it with a queue and device
	/// \param[in] vk the device/queue to associate with the UBO
	/// \return true on success and false on failure
	bool Initialize(NvVkContext& vk) {
		if (!m_localSync.Initialize(vk))
			return false;
		if (!m_ubo.Initialize(vk, &m_localSync))
			return false;

		return true;
	}

	/// Initializes the UBO mapping, associating it with a queue and device, sharing its buffer with other UBOs
	/// \param[in] vk the device/queue to associate with the UBO
	/// \param[in] alloc the shared allocator to use
	/// \return true on success and false on failure
	bool Initialize(NvVkContext& vk, NvSimpleUBOAllocator& alloc) {
		if (!m_localSync.Initialize(vk))
			return false;
		if (!m_ubo.Initialize(vk, alloc, &m_localSync))
			return false;

		return true;
	}

	/// Release all API resources associated with the UBO
	void Finalize() {
	}

	/// Updates the current CPU-accessible structure values to a UBO
	/// copy.  Changes to the CPU-accessible structure will not appear
	/// in rendering calls until after a subsequent update call.
	/// Will fail if there is no buffer currently available for use, which
	/// will happen if Update is called too frequently for the number of
	/// buffers as set by the template parameter
	/// \return true on success and false on failure
	bool Update() {
		return m_ubo.Update();
	}

	/// Return the address of the current UBO backing store to be used
	/// in draw calls; this value will change with every call to #Update,
	/// so take care when caching the value of this in a command buffer
	/// \return the BufferAddress of the current copy
	void GetDesc(VkDescriptorBufferInfo& desc) {
		m_ubo.GetDesc(desc);
	}

	/// Get the dynamic offset into the UBO of the current copy of the data
	uint32_t getDynamicOffset() {
		return m_ubo.getDynamicOffset();
	}

	/// Accessor for easy mapping of the wrapper to the CPU-accessible
	/// structure
	/// Changes to the structure pointed to by this pointer will NOT be 
	/// reflected in a bound UBO until after a subsequent call to Update
	/// \return a pointer to the CPU accessible struct
	UBOType* operator-> () { return m_ubo; }

	/// Read-only Accessor for easy mapping of the wrapper to the
	/// CPU-accessible structure
	/// \return a const pointer to the CPU accessible struct
	const UBOType* operator-> () const { return &m_ubo; }

	/// Accessor for easy mapping of the wrapper to the CPU-accessible
	/// structure
	/// Changes to the structure pointed to by this reference will NOT be 
	/// reflected in a bound UBO until after a subsequent call to Update
	/// \return a reference to the CPU accessible struct
	UBOType& operator* () { return *m_ubo; }

	/// Read-only Accessor for easy mapping of the wrapper to the
	/// CPU-accessible structure
	/// \return a const reference to the CPU accessible struct
	const UBOType& operator* () const { return m_ubo; }

	/// Accessor for easy mapping of the wrapper to the CPU-accessible
	/// structure
	/// Changes to the structure pointed to by this pointer will NOT be 
	/// reflected in a bound UBO until after a subsequent call to Update
	/// \return a pointer to the CPU accessible struct
	operator UBOType*() { return m_ubo; }

protected:
	NvSimpleSharedUBO<UBOType, m_copies> m_ubo;
	NvSyncRing<m_copies> m_localSync;
};


#endif // NV_SIMPLE_UBO
