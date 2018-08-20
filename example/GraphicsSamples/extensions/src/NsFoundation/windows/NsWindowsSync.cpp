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

#include "windows/NsWindowsInclude.h"
#include "NsUserAllocated.h"
#include "NsSync.h"

namespace nvidia
{
namespace shdfnd
{

namespace
{
HANDLE& getSync(SyncImpl* impl)
{
	return *reinterpret_cast<HANDLE*>(impl);
}
}

static const uint32_t gSize = sizeof(HANDLE);
const uint32_t& SyncImpl::getSize()
{
	return gSize;
}

SyncImpl::SyncImpl()
{
#if NV_WINRT
	getSync(this) = CreateEventEx(NULL, NULL, CREATE_EVENT_MANUAL_RESET, EVENT_ALL_ACCESS);
#else
	getSync(this) = CreateEvent(0, true, false, 0);
#endif
}

SyncImpl::~SyncImpl()
{
	CloseHandle(getSync(this));
}

void SyncImpl::reset()
{
	ResetEvent(getSync(this));
}

void SyncImpl::set()
{
	SetEvent(getSync(this));
}

bool SyncImpl::wait(uint32_t milliseconds)
{
	if(milliseconds == -1)
		milliseconds = INFINITE;
#if NV_WINRT
	return WaitForSingleObjectEx(getSync(this), milliseconds, false) == WAIT_OBJECT_0 ? true : false;
#else
	return WaitForSingleObject(getSync(this), milliseconds) == WAIT_OBJECT_0 ? true : false;
#endif
}

} // namespace shdfnd
} // namespace nvidia
