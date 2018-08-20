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

#include "Ns.h"
#include "NsAtomic.h"
#include "NsThread.h"
#include "NvAssert.h"
#include "NvErrorCallback.h"

#include <math.h>
#if !NV_APPLE_FAMILY && !defined(ANDROID) && !defined(__CYGWIN__) && !NV_PS4
#include <bits/local_lim.h> // PTHREAD_STACK_MIN
#endif
#include <stdio.h>
#include <pthread.h>
#include <unistd.h>
#if !NV_PS4
#include <sys/syscall.h>
#if !NV_APPLE_FAMILY
#include <asm/unistd.h>
#include <sys/resource.h>
#endif
#endif

#if NV_APPLE_FAMILY
#include <sys/types.h>
#include <sys/sysctl.h>
#endif

// fwd
#if defined(ANDROID)
extern "C" {
int android_getCpuCount(void);
}
#endif

#define NvSpinLockPause() asm("nop")

namespace nvidia
{
namespace shdfnd
{

namespace
{

typedef enum
{
	_NvThreadNotStarted,
	_NvThreadStarted,
	_NvThreadStopped
} NvThreadState;

class _ThreadImpl
{
  public:
	ThreadImpl::ExecuteFn fn;
	void* arg;
	volatile int32_t quitNow;
	volatile int32_t threadStarted;
	volatile int32_t state;

	pthread_t thread;
	pid_t tid;

	uint32_t affinityMask;
};

_ThreadImpl* getThread(ThreadImpl* impl)
{
	return reinterpret_cast<_ThreadImpl*>(impl);
}

static void setTid(_ThreadImpl& threadImpl)
{
// query TID
#if NV_PS4
// AM: TODO: neither of the below are implemented
#elif NV_APPLE_FAMILY
	threadImpl.tid = syscall(SYS_gettid);
#else
	threadImpl.tid = syscall(__NR_gettid);
#endif

	// notify/unblock parent thread
	atomicCompareExchange(&(threadImpl.threadStarted), 1, 0);
}

void* NvThreadStart(void* arg)
{
	_ThreadImpl* impl = getThread((ThreadImpl*)arg);
	impl->state = _NvThreadStarted;

	// run setTid in thread's context
	setTid(*impl);

	// then run either the passed in function or execute from the derived class (Runnable).
	if(impl->fn)
		(*impl->fn)(impl->arg);
	else if(impl->arg)
		((Runnable*)impl->arg)->execute();
	return 0;
}
}

static const uint32_t gSize = sizeof(_ThreadImpl);
const uint32_t& ThreadImpl::getSize()
{
	return gSize;
}

ThreadImpl::Id ThreadImpl::getId()
{
	return Id(pthread_self());
}

ThreadImpl::ThreadImpl()
{
	getThread(this)->thread = 0;
	getThread(this)->tid = 0;
	getThread(this)->state = _NvThreadNotStarted;
	getThread(this)->quitNow = 0;
	getThread(this)->threadStarted = 0;
	getThread(this)->fn = NULL;
	getThread(this)->arg = NULL;
	getThread(this)->affinityMask = 0;
}

ThreadImpl::ThreadImpl(ThreadImpl::ExecuteFn fn, void* arg)
{
	getThread(this)->thread = 0;
	getThread(this)->tid = 0;
	getThread(this)->state = _NvThreadNotStarted;
	getThread(this)->quitNow = 0;
	getThread(this)->threadStarted = 0;
	getThread(this)->fn = fn;
	getThread(this)->arg = arg;
	getThread(this)->affinityMask = 0;

	start(0, NULL);
}

ThreadImpl::~ThreadImpl()
{
	if(getThread(this)->state == _NvThreadStarted)
		kill();
}

void ThreadImpl::start(uint32_t stackSize, Runnable* runnable)
{
	if(getThread(this)->state != _NvThreadNotStarted)
		return;

	if(stackSize == 0)
		stackSize = getDefaultStackSize();

#if defined(PTHREAD_STACK_MIN) && !defined(ANDROID)
	if(stackSize < PTHREAD_STACK_MIN)
	{
		getErrorCallback().reportError(NvErrorCode::eDEBUG_WARNING,
		                               "ThreadImpl::start(): stack size was set below PTHREAD_STACK_MIN", __FILE__,
		                               __LINE__);
		stackSize = PTHREAD_STACK_MIN;
	}
#endif

	if(runnable && !getThread(this)->arg && !getThread(this)->fn)
		getThread(this)->arg = runnable;

	pthread_attr_t attr;
	int status = pthread_attr_init(&attr);
	NV_ASSERT(!status);
	NV_UNUSED(status);

	status = pthread_attr_setstacksize(&attr, stackSize);
	NV_ASSERT(!status);
	status = pthread_create(&getThread(this)->thread, &attr, NvThreadStart, this);
	NV_ASSERT(!status);

	// wait for thread to startup and write out TID
	// otherwise TID dependent calls like setAffinity will fail.
	while(atomicCompareExchange(&(getThread(this)->threadStarted), 1, 1) == 0)
		yield();

	// here we are sure that getThread(this)->state >= _NvThreadStarted

	status = pthread_attr_destroy(&attr);
	NV_ASSERT(!status);

	// apply stored affinity mask
	if(getThread(this)->affinityMask)
		setAffinityMask(getThread(this)->affinityMask);
}

void ThreadImpl::signalQuit()
{
	atomicIncrement(&(getThread(this)->quitNow));
}

bool ThreadImpl::waitForQuit()
{
	if(getThread(this)->state == _NvThreadNotStarted)
		return false;

	// works also with a stopped/exited thread if the handle is still valid
	pthread_join(getThread(this)->thread, NULL);
	return true;
}

bool ThreadImpl::quitIsSignalled()
{
	return atomicCompareExchange(&(getThread(this)->quitNow), 0, 0) != 0;
}

void ThreadImpl::quit()
{
	getThread(this)->state = _NvThreadStopped;
	pthread_exit(0);
}

void ThreadImpl::kill()
{
#ifndef ANDROID
	if(getThread(this)->state == _NvThreadStarted)
		pthread_cancel(getThread(this)->thread);
	getThread(this)->state = _NvThreadStopped;
#else
	getErrorCallback().reportError(NvErrorCode::eDEBUG_WARNING, "ThreadImpl::kill() called, but is not implemented",
	                               __FILE__, __LINE__);
#endif
}

void ThreadImpl::sleep(uint32_t ms)
{
	timespec sleepTime;
	uint32_t remainder = ms % 1000;
	sleepTime.tv_sec = ms - remainder;
	sleepTime.tv_nsec = remainder * 1000000L;

	while(nanosleep(&sleepTime, &sleepTime) == -1)
		continue;
}

void ThreadImpl::yield()
{
	sched_yield();
}

#if NV_PS4
uint32_t setAffinityMaskPS4(pthread_t, uint32_t);
#endif

uint32_t ThreadImpl::setAffinityMask(uint32_t mask)
{
	// Same as windows impl if mask is zero
	if(!mask)
		return 0;

	getThread(this)->affinityMask = mask;

	uint64_t prevMask = 0;

	if(getThread(this)->state == _NvThreadStarted)
	{
#if NV_PS4
		prevMask = setAffinityMaskPS4(getThread(this)->thread, mask);
#elif !NV_APPLE_FAMILY // Apple doesn't support syscall with getaffinity and setaffinity
		int32_t errGet = syscall(__NR_sched_getaffinity, getThread(this)->tid, sizeof(prevMask), &prevMask);
		if(errGet < 0)
			return 0;

		int32_t errSet = syscall(__NR_sched_setaffinity, getThread(this)->tid, sizeof(mask), &mask);
		if(errSet != 0)
			return 0;
#endif
	}

	return uint32_t(prevMask);
}

void ThreadImpl::setName(const char* name)
{
#if(defined(ANDROID) && (__ANDROID_API__ > 8))
	pthread_setname_np(getThread(this)->thread, name);
#else
	// not implemented because most unix APIs expect setName()
	// to be called from the thread's context. Example see next comment:

	// this works only with the current thread and can rename
	// the main process if used in the wrong context:
	// prctl(PR_SET_NAME, reinterpret_cast<unsigned long>(name) ,0,0,0);
	NV_UNUSED(name);
#endif
}

#if !NV_APPLE_FAMILY
static ThreadPriority::Enum convertPriorityFromLinux(uint32_t inPrio, int policy)
{
	NV_COMPILE_TIME_ASSERT(ThreadPriority::eLOW > ThreadPriority::eHIGH);
	NV_COMPILE_TIME_ASSERT(ThreadPriority::eHIGH == 0);

	int maxL = sched_get_priority_max(policy);
	int minL = sched_get_priority_min(policy);
	int rangeL = maxL - minL;
	int rangeNv = ThreadPriority::eLOW - ThreadPriority::eHIGH;

	// case for default scheduler policy
	if(rangeL == 0)
		return ThreadPriority::eNORMAL;

	float floatPrio = (float(maxL - inPrio) * float(rangeNv)) / float(rangeL);

	return ThreadPriority::Enum(int(roundf(floatPrio)));
}

static int convertPriorityToLinux(ThreadPriority::Enum inPrio, int policy)
{
	int maxL = sched_get_priority_max(policy);
	int minL = sched_get_priority_min(policy);
	int rangeL = maxL - minL;
	int rangeNv = ThreadPriority::eLOW - ThreadPriority::eHIGH;

	// case for default scheduler policy
	if(rangeL == 0)
		return 0;

	float floatPrio = (float(ThreadPriority::eLOW - inPrio) * float(rangeL)) / float(rangeNv);

	return minL + int(roundf(floatPrio));
}
#endif

void ThreadImpl::setPriority(ThreadPriority::Enum val)
{
	NV_UNUSED(val);
#if !NV_APPLE_FAMILY
	int policy;
	sched_param s_param;
	pthread_getschedparam(getThread(this)->thread, &policy, &s_param);
	s_param.sched_priority = convertPriorityToLinux(val, policy);
	pthread_setschedparam(getThread(this)->thread, policy, &s_param);
#endif
}

ThreadPriority::Enum ThreadImpl::getPriority(Id pthread)
{
	NV_UNUSED(pthread);
#if !NV_APPLE_FAMILY
	int policy;
	sched_param s_param;
	int ret = pthread_getschedparam(pthread_t(pthread), &policy, &s_param);
	if(ret == 0)
		return convertPriorityFromLinux(s_param.sched_priority, policy);
	else
		return ThreadPriority::eNORMAL;
#else
	return ThreadPriority::eNORMAL;
#endif
}

uint32_t ThreadImpl::getNbPhysicalCores()
{
#if NV_APPLE_FAMILY
	int count;
	size_t size = sizeof(count);
	return sysctlbyname("hw.physicalcpu", &count, &size, NULL, 0) ? 0 : count;
#elif defined(ANDROID)
	return android_getCpuCount();
#else
	// Linux exposes CPU topology using /sys/devices/system/cpu
	// https://www.kernel.org/doc/Documentation/cputopology.txt
	if(FILE* f = fopen("/sys/devices/system/cpu/possible", "r"))
	{
		int minIndex, maxIndex;
		int n = fscanf(f, "%d-%d", &minIndex, &maxIndex);
		fclose(f);

		if(n == 2)
			return (maxIndex - minIndex) + 1;
		else if(n == 1)
			return minIndex + 1;
	}

#if NV_PS4
	// Reducing to 6 to take into account that the OS appears to use 2 cores at peak currently.
	return 6;
#else
	// For non-Linux kernels this fallback is possibly the best we can do
	// but will report logical (hyper-threaded) counts
	int n = sysconf(_SC_NPROCESSORS_CONF);
	if(n < 0)
		return 0;
	else
		return n;
#endif
#endif
}

uint32_t TlsAlloc()
{
	pthread_key_t key;
	int status = pthread_key_create(&key, NULL);
	NV_ASSERT(!status);
	NV_UNUSED(status);
	return (uint32_t)key;
}

void TlsFree(uint32_t index)
{
	int status = pthread_key_delete((pthread_key_t)index);
	NV_ASSERT(!status);
	NV_UNUSED(status);
}

void* TlsGet(uint32_t index)
{
	return (void*)pthread_getspecific((pthread_key_t)index);
}

uint32_t TlsSet(uint32_t index, void* value)
{
	int status = pthread_setspecific((pthread_key_t)index, value);
	NV_ASSERT(!status);
	return !status;
}

// DM: On Linux x86-32, without implementation-specific restrictions
// the default stack size for a new thread should be 2 megabytes (kernel.org).
// NOTE: take care of this value on other architecutres!
uint32_t ThreadImpl::getDefaultStackSize()
{
	return 1 << 21;
}

} // namespace shdfnd
} // namespace nvidia
