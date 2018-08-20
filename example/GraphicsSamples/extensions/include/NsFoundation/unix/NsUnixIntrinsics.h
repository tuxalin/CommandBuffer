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

#ifndef NV_UNIX_NSUNIXINTRINSICS_H
#define NV_UNIX_NSUNIXINTRINSICS_H

#include "Ns.h"
#include "NvAssert.h"
#include <math.h>

#if NV_ANDROID
#include <signal.h> // for Ns::debugBreak() { raise(SIGTRAP); }
#endif

#if 0
#include <libkern/OSAtomic.h>
#endif

// this file is for internal intrinsics - that is, intrinsics that are used in
// cross platform code but do not appear in the API

#if !(NV_LINUX || NV_ANDROID || NV_PS4 || NV_APPLE_FAMILY)
#error "This file should only be included by unix builds!!"
#endif

namespace nvidia
{
namespace shdfnd
{

NV_FORCE_INLINE void memoryBarrier()
{
	__sync_synchronize();
}

/*!
Return the index of the highest set bit. Undefined for zero arg.
*/
NV_INLINE uint32_t highestSetBitUnsafe(uint32_t v)
{

	// http://graphics.stanford.edu/~seander/bithacks.html
	static const uint32_t MultiplyDeBruijnBitPosition[32] = { 0,  9,  1,  10, 13, 21, 2,  29, 11, 14, 16,
		                                                      18, 22, 25, 3,  30, 8,  12, 20, 28, 15, 17,
		                                                      24, 7,  19, 27, 23, 6,  26, 5,  4,  31 };

	v |= v >> 1; // first round up to one less than a power of 2
	v |= v >> 2;
	v |= v >> 4;
	v |= v >> 8;
	v |= v >> 16;

	return MultiplyDeBruijnBitPosition[(uint32_t)(v * 0x07C4ACDDU) >> 27];
}

/*!
Return the index of the highest set bit. Undefined for zero arg.
*/
NV_INLINE int32_t lowestSetBitUnsafe(uint32_t v)
{
	// http://graphics.stanford.edu/~seander/bithacks.html
	static const uint32_t MultiplyDeBruijnBitPosition[32] = { 0,  1,  28, 2,  29, 14, 24, 3,  30, 22, 20,
		                                                      15, 25, 17, 4,  8,  31, 27, 13, 23, 21, 19,
		                                                      16, 7,  26, 12, 18, 6,  11, 5,  10, 9 };
	int32_t w = v;
	return MultiplyDeBruijnBitPosition[(uint32_t)((w & -w) * 0x077CB531U) >> 27];
}

/*!
Returns the index of the highest set bit. Undefined for zero arg.
*/
NV_INLINE uint32_t countLeadingZeros(uint32_t v)
{
	int32_t result = 0;
	uint32_t testBit = (1 << 31);
	while((v & testBit) == 0 && testBit != 0)
		result++, testBit >>= 1;
	return result;
}

/*!
Prefetch aligned 64B x86, 32b ARM around \c ptr+offset.
*/
NV_FORCE_INLINE void prefetchLine(const void* ptr, uint32_t offset = 0)
{
	__builtin_prefetch((char * NV_RESTRICT)(ptr) + offset, 0, 3);
}

/*!
Prefetch \c count bytes starting at \c ptr.
*/
#if NV_ANDROID || NV_IOS
NV_FORCE_INLINE void prefetch(const void* ptr, uint32_t count = 1)
{
	const char* cp = (char*)ptr;
	size_t p = (size_t)ptr;
	uint32_t startLine = uint32_t(p >> 5), endLine = uint32_t((p + count - 1) >> 5);
	uint32_t lines = endLine - startLine + 1;
	do
	{
		prefetchLine(cp);
		cp += 32;
	} while(--lines);
}
#else
NV_FORCE_INLINE void prefetch(const void* ptr, uint32_t count = 1)
{
	const char* cp = (char*)ptr;
	uint64_t p = size_t(ptr);
	uint64_t startLine = p >> 6, endLine = (p + count - 1) >> 6;
	uint64_t lines = endLine - startLine + 1;
	do
	{
		prefetchLine(cp);
		cp += 64;
	} while(--lines);
}
#endif

//! \brief platform-specific reciprocal
NV_CUDA_CALLABLE NV_FORCE_INLINE float recipFast(float a)
{
	return 1.0f / a;
}

//! \brief platform-specific fast reciprocal square root
NV_CUDA_CALLABLE NV_FORCE_INLINE float recipSqrtFast(float a)
{
	return 1.0f / ::sqrtf(a);
}

//! \brief platform-specific floor
NV_CUDA_CALLABLE NV_FORCE_INLINE float floatFloor(float x)
{
	return ::floorf(x);
}

#define NS_EXPECT_TRUE(x) x
#define NS_EXPECT_FALSE(x) x

} // namespace shdfnd
} // namespace nvidia

#endif // #ifndef NV_UNIX_NSUNIXINTRINSICS_H
