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

#include "NsAllocator.h"
#include "NsHashMap.h"
#include "NsArray.h"
#include "NsMutex.h"

namespace nvidia
{
namespace shdfnd
{

#if NV_USE_NAMED_ALLOCATOR
namespace
{
typedef HashMap<const NamedAllocator*, const char*, Hash<const NamedAllocator*>, NonTrackingAllocator> AllocNameMap;

class NamedAllocatorGlobals
{
	NV_NOCOPY(NamedAllocatorGlobals)
  public:
	NamedAllocatorGlobals()
	{
	}
	AllocNameMap allocMap;
	Mutex allocMutex;
};

NamedAllocatorGlobals* gNamedAllocatorGlobals;

NV_INLINE AllocNameMap& getMap()
{
	return gNamedAllocatorGlobals->allocMap;
}
NV_INLINE Mutex& getMutex()
{
	return gNamedAllocatorGlobals->allocMutex;
}
}

NamedAllocator::NamedAllocator(const NvEMPTY)
{
	Mutex::ScopedLock lock(getMutex());
	getMap().insert(this, 0);
}

NamedAllocator::NamedAllocator(const char* name)
{
	Mutex::ScopedLock lock(getMutex());
	getMap().insert(this, name);
}

NamedAllocator::NamedAllocator(const NamedAllocator& other)
{
	Mutex::ScopedLock lock(getMutex());
	const AllocNameMap::Entry* e = getMap().find(&other);
	NV_ASSERT(e);
	const char* name = e->second; // The copy is important because insert might invalidate the referenced hash entry
	getMap().insert(this, name);
}

NamedAllocator::~NamedAllocator()
{
	Mutex::ScopedLock lock(getMutex());
	bool erased = getMap().erase(this);
	NV_UNUSED(erased);
	NV_ASSERT(erased);
}

NamedAllocator& NamedAllocator::operator=(const NamedAllocator& other)
{
	Mutex::ScopedLock lock(getMutex());
	const AllocNameMap::Entry* e = getMap().find(&other);
	NV_ASSERT(e);
	getMap()[this] = e->second;
	return *this;
}

void* NamedAllocator::allocate(size_t size, const char* filename, int line)
{
	if(!size)
		return 0;
	Mutex::ScopedLock lock(getMutex());
	const AllocNameMap::Entry* e = getMap().find(this);
	NV_ASSERT(e);
	return getAllocator().allocate(size, e->second, filename, line);
}

void NamedAllocator::deallocate(void* ptr)
{
	if(ptr)
		getAllocator().deallocate(ptr);
}

#endif // NV_DEBUG

void initializeNamedAllocatorGlobals()
{
#if NV_USE_NAMED_ALLOCATOR
	gNamedAllocatorGlobals = reinterpret_cast<NamedAllocatorGlobals*>(
	    getAllocator().allocate(sizeof(NamedAllocatorGlobals), "NamedAllocatorGlobals", __FILE__, __LINE__));
	new (gNamedAllocatorGlobals) NamedAllocatorGlobals();
#endif
}

void terminateNamedAllocatorGlobals()
{
#if NV_USE_NAMED_ALLOCATOR
	gNamedAllocatorGlobals->~NamedAllocatorGlobals();
	getAllocator().deallocate(gNamedAllocatorGlobals);
	gNamedAllocatorGlobals = 0;
#endif
}

} // namespace shdfnd
} // namespace nvidia
