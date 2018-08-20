//----------------------------------------------------------------------------------
// File:        NvAppBase/NvFoundationInit.cpp
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
#include "NvFoundationInit.h"
#include "NsUserAllocated.h"
#include "NvErrorCallback.h"
#include "NvAllocatorCallback.h"
#include "NsArray.h"
#include "NsGlobals.h"
#include "NsVersionNumber.h"


class DefaultErrorCallback : public nvidia::NvErrorCallback
{
public:
	DefaultErrorCallback(void)
	{
	}

	virtual void reportError(nvidia::NvErrorCode::Enum code, const char* message, const char* file, int line)
	{
		NV_UNUSED(code);
		NV_UNUSED(file);
		NV_UNUSED(line);
		//		printf("PhysX: %s\r\n", message );
	}
private:
};

#if NV_WINDOWS_FAMILY
// on win32 we only have 8-byte alignment guaranteed, but the CRT provides special aligned allocation
// fns
#include <malloc.h>
#include <crtdbg.h>

static void* platformAlignedAlloc(size_t size)
{
	return _aligned_malloc(size, 16);	
}

static void platformAlignedFree(void* ptr)
{
	_aligned_free(ptr);
}
#elif NV_LINUX || NV_ANDROID
static void* platformAlignedAlloc(size_t size)
{
	return ::memalign(16, size);
}

static void platformAlignedFree(void* ptr)
{
	::free(ptr);
}
#elif NV_WIIU
static void* platformAlignedAlloc(size_t size)
{
	size_t pad = 15 + sizeof(size_t); // store offset for delete.
	PxU8* base = (PxU8*)::malloc(size+pad);
	if(!base)
		return NULL;

	PxU8* ptr = (PxU8*)(size_t(base + pad) & ~(15)); // aligned pointer
	((size_t*)ptr)[-1] = ptr - base; // store offset

	return ptr;
}

static void platformAlignedFree(void* ptr)
{
	if(ptr == NULL)
		return;

	PxU8* base = ((PxU8*)ptr) - ((size_t*)ptr)[-1];
	::free(base);
}
#else

// on PS3, XBox and Win64 we get 16-byte alignment by default
static void* platformAlignedAlloc(size_t size)
{
	void *ptr = ::malloc(size);	
	NV_ASSERT((reinterpret_cast<size_t>(ptr) & 15)==0);
	return ptr;
}

static void platformAlignedFree(void* ptr)
{
	::free(ptr);			
}
#endif


class DefaultAllocator : public nvidia::NvAllocatorCallback
{
public:
	DefaultAllocator(void)
	{
	}

	~DefaultAllocator(void)
	{
	}


	virtual void* allocate(size_t size, const char* typeName, const char* filename, int line)
	{
		NV_UNUSED(typeName);
		NV_UNUSED(filename);
		NV_UNUSED(line);
		void *ret = platformAlignedAlloc(size);
		return ret;
	}

	virtual void deallocate(void* ptr)
	{
		platformAlignedFree(ptr);
	}
private:
};


DefaultAllocator		gGameDefaultAllocator;
DefaultErrorCallback	gGameDefaultErrorCallback;

void NvInitSharedFoundation(void)
{
	nvidia::NvAllocatorCallback *allocatorCallback = &gGameDefaultAllocator;
	nvidia::NvErrorCallback *errorCallback = &gGameDefaultErrorCallback;
	nvidia::shdfnd::initializeSharedFoundation(NV_FOUNDATION_VERSION,*allocatorCallback,*errorCallback);
	nvidia::shdfnd::setReflectionAllocatorReportsNames(true);
}

void NvReleaseSharedFoundation(void)
{
	nvidia::shdfnd::terminateSharedFoundation();
}
