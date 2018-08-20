// This code contains NVIDIA Confidential Information and is disclosed to you
// under a form of NVIDIA software license agreement provided separately to you.
//
// Notice
// NVIDIA Corporation and its licensors retain all intellectual property and
// proprietary rights in and to this software and related documentation and
// any modifications thereto. Any use, reproduction, disclosure, or
// distribution of this software and related documentation without an express
// license agreement from NVIDIA Corporation is strictly prohibited.
//
// ALL NVIDIA DESIGN SPECIFICATIONS, CODE ARE PROVIDED "AS IS.". NVIDIA MAKES
// NO WARRANTIES, EXPRESSED, IMPLIED, STATUTORY, OR OTHERWISE WITH RESPECT TO
// THE MATERIALS, AND EXPRESSLY DISCLAIMS ALL IMPLIED WARRANTIES OF NONINFRINGEMENT,
// MERCHANTABILITY, AND FITNESS FOR A PARTICULAR PURPOSE.
//
// Information and code furnished is believed to be accurate and reliable.
// However, NVIDIA Corporation assumes no responsibility for the consequences of use of such
// information or for any infringement of patents or other rights of third parties that may
// result from its use. No license is granted by implication or otherwise under any patent
// or patent rights of NVIDIA Corporation. Details are subject to change without notice.
// This code supersedes and replaces all information previously supplied.
// NVIDIA Corporation products are not authorized for use as critical
// components in life support devices or systems without express written approval of
// NVIDIA Corporation.
//
// Copyright (c) 2008-2014 NVIDIA Corporation. All rights reserved.
// Copyright (c) 2004-2008 AGEIA Technologies, Inc. All rights reserved.
// Copyright (c) 2001-2004 NovodeX AG. All rights reserved.

#include "NsTempAllocator.h"

#include "NsArray.h"
#include "NsMutex.h"
#include "NsAtomic.h"
#include "NvMath.h"
#include "NsIntrinsics.h"
#include "NsBitUtils.h"

#if NV_VC
#pragma warning(disable : 4706) // assignment within conditional expression
#endif

namespace nvidia
{
namespace shdfnd
{
namespace
{
typedef TempAllocatorChunk Chunk;
typedef Array<Chunk*, NonTrackingAllocator> AllocFreeTable;

class TempAllocatorGlobals
{
	NV_NOCOPY(TempAllocatorGlobals)
  public:
	TempAllocatorGlobals()
	{
	}
	AllocFreeTable allocFreeTable;
	Mutex allocMutex;
};

TempAllocatorGlobals* gTempAllocatorGlobals = 0;

NV_INLINE AllocFreeTable& getFreeTable()
{
	return gTempAllocatorGlobals->allocFreeTable;
}
NV_INLINE Mutex& getMutex()
{
	return gTempAllocatorGlobals->allocMutex;
}

const uint32_t sMinIndex = 8;  // 256B min
const uint32_t sMaxIndex = 17; // 128kB max
}

void initializeTempAllocatorGlobals()
{
	gTempAllocatorGlobals =
	    reinterpret_cast<TempAllocatorGlobals*>(NV_ALLOC(sizeof(TempAllocatorGlobals), "TempAllocatorGlobals"));
	new (gTempAllocatorGlobals) TempAllocatorGlobals();
}

void terminateTempAllocatorGlobals()
{
	AllocFreeTable& table = getFreeTable();

	NonTrackingAllocator alloc;
	for(uint32_t i = 0; i < table.size(); ++i)
	{
		for(TempAllocatorChunk* ptr = table[i]; ptr;)
		{
			TempAllocatorChunk* next = ptr->mNext;
			alloc.deallocate(ptr);
			ptr = next;
		}
	}
	table.reset();

	gTempAllocatorGlobals->~TempAllocatorGlobals();
	NV_FREE(gTempAllocatorGlobals);
	gTempAllocatorGlobals = 0;
}

void* TempAllocator::allocate(size_t size, const char* filename, int line)
{
	if(!size)
		return 0;

	uint32_t index = NvMax(highestSetBit(uint32_t(size) + sizeof(Chunk) - 1), sMinIndex);

	Chunk* chunk = 0;
	if(index < sMaxIndex)
	{
		Mutex::ScopedLock lock(getMutex());

		// find chunk up to 16x bigger than necessary
		Chunk** it = getFreeTable().begin() + index - sMinIndex;
		Chunk** end = NvMin(it + 3, getFreeTable().end());
		while(it < end && !(*it))
			++it;

		if(it < end)
			// pop top off freelist
			chunk = *it, *it = chunk->mNext, index = uint32_t(it - getFreeTable().begin() + sMinIndex);
		else
			// create new chunk
			chunk = (Chunk*)NonTrackingAllocator().allocate(size_t(2 << index), filename, line);
	}
	else
	{
		// too big for temp allocation, forward to base allocator
		chunk = (Chunk*)NonTrackingAllocator().allocate(size + sizeof(Chunk), filename, line);
	}

	chunk->mIndex = index;
	void* ret = chunk + 1;
	NV_ASSERT((size_t(ret) & 0xf) == 0); // SDK types require at minimum 16 byte allignment.
	return ret;
}

void TempAllocator::deallocate(void* ptr)
{
	if(!ptr)
		return;

	Chunk* chunk = reinterpret_cast<Chunk*>(ptr) - 1;
	uint32_t index = chunk->mIndex;

	if(index >= sMaxIndex)
		return NonTrackingAllocator().deallocate(chunk);

	Mutex::ScopedLock lock(getMutex());

	index -= sMinIndex;
	if(getFreeTable().size() <= index)
		getFreeTable().resize(index + 1);

	chunk->mNext = getFreeTable()[index];
	getFreeTable()[index] = chunk;
}

} // namespace shdfnd
} // namespace nvidia
