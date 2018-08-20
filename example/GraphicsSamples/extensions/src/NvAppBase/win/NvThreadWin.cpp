//----------------------------------------------------------------------------------
// File:        NvAppBase\win/NvThreadWin.cpp
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
#include "NvThreadWin.h"

#include <cstdlib>
#include <WinNT.h>
#include "NV/NvLogs.h"

#define NVTHREADWIN_DEBUG 0
#if NVTHREADWIN_DEBUG
	#define NVTHREADWIN_LOG(arg) LOGI((arg))
#else
	#define NVTHREADWIN_LOG(arg) ((void)0)
#endif

// Some constants (defined once for type safety and reuse)
static const long long int NV_ZERO_LONG_LONG  = static_cast<long long int>(0);
static const         DWORD NV_0x1000_DWORD    = static_cast<DWORD>(0x1000);
static const         DWORD NV_ZERO_DWORD      = static_cast<DWORD>(0);
static const         DWORD NV_MINUS_ONE_DWORD = static_cast<DWORD>(-1);
static const  unsigned int NV_MUTEX_LOCK_MAX_ATTEMPTS_PER_TRY = 20U;

// Setting thread names: https://msdn.microsoft.com/en-us/library/xcb2z8hs.aspx
static const DWORD MS_VC_EXCEPTION = static_cast<DWORD>(0x406D1388);

#pragma pack(push,8)
typedef struct THREADNAME_INFO_t
{
	DWORD dwType;     // Must be 0x1000.
	LPCSTR szName;    // Pointer to name (in user addr space).
	DWORD dwThreadID; // Thread ID (-1=caller thread).
	DWORD dwFlags;    // Reserved for future use, must be zero.
} THREADNAME_INFO;
#pragma pack(pop)

void getLastError(void){

	const DWORD lastError = GetLastError();
	LOGI("last error = %lu", lastError);
}

NvThreadWin::NvThreadWin(void) :
	mThreadHnd(nullptr),
	mThreadId(0U),
	mName(nullptr),
	mPriority(0) {

	// No default ctor
	throw new NvThreadExceptionWin;
}

NvThreadWin::NvThreadWin(const NvThread& obj) :
	mThreadHnd(nullptr),
	mThreadId(0U),
	mName(nullptr),
	mPriority(0) {

	UNREFERENCED_PARAMETER(obj);

	// No copy ctor
	throw new NvThreadExceptionWin;
}

NvThreadWin::NvThreadWin(NvThreadFunction function, void* argument,
	void* stack, size_t stackSize, int priority) :
	mThreadHnd(nullptr),
	mThreadId(0U),
	mName(nullptr),
	mPriority(NvThread::DefaultThreadPriority) {

	NVTHREADWIN_LOG("NvThreadWin::ctor");

	mThreadHnd = CreateThread(nullptr, 0, (LPTHREAD_START_ROUTINE)function,
		argument, CREATE_SUSPENDED, &mThreadId);
	if (nullptr == mThreadHnd) {
		getLastError();
		throw new NvThreadExceptionWin;
	}
}

NvThreadWin::~NvThreadWin(void) {

	NVTHREADWIN_LOG("NvThreadWin::dtor");

	this->freeThreadName();

	const BOOL result = CloseHandle(mThreadHnd);
	if (result != TRUE) {
		getLastError();
		return;
	}

	mThreadHnd = nullptr;
	mThreadId = 0U;
	mName = nullptr;
	mPriority = NvThread::DefaultThreadPriority;
}

void NvThreadWin::startThread(void) {

	NVTHREADWIN_LOG("NvThreadWin::startThread");

	const DWORD result = ResumeThread(mThreadHnd);

	if (result == NV_MINUS_ONE_DWORD) {
		getLastError();
		throw new NvThreadExceptionWin;
	}
}

void NvThreadWin::waitThread(void) {

	NVTHREADWIN_LOG("NvThreadWin::waitThread");

	const DWORD result = WaitForSingleObjectEx(mThreadHnd, INFINITE, FALSE);

#if NVTHREADWIN_DEBUG
	switch (result) {
	case WAIT_ABANDONED:
		LOGI("NvThreadWin::waitThread -> WAIT_ABANDONED");
		break;
	case WAIT_IO_COMPLETION:
		LOGI("NvThreadWin::waitThread -> WAIT_IO_COMPLETION");
		break;
	case WAIT_OBJECT_0:
		LOGI("NvThreadWin::waitThread -> WAIT_OBJECT_0");
		break;
	case WAIT_TIMEOUT:
		LOGI("NvThreadWin::waitThread -> WAIT_TIMEOUT");
		break;
	case WAIT_FAILED:
		LOGI("NvThreadWin::waitThread -> WAIT_FAILED");
		break;
	default:
		LOGI("NvThreadWin::waitThread -> UNKNOWN");
		break;
	}
#endif // NVTHREADWIN_DEBUG
}

int NvThreadWin::changeThreadPriority(int priority) {

	NVTHREADWIN_LOG("NvThreadWin::changeThreadPriority");

	if (priority == mPriority)
		return priority;

	const int prevPriority = mPriority;
	mPriority = priority;

	const int winPriority = this->getNNToWinPriority(priority);

#if NVTHREADWIN_DEBUG
	LOGI("Setting Windows thread priority to %d", winPriority);
#endif // NVTHREADWIN_DEBUG

	const BOOL result = SetThreadPriority(mThreadHnd, winPriority);
	if (result != TRUE) {
		getLastError();
		throw new NvThreadExceptionWin;
	}

	return prevPriority;
}

int NvThreadWin::getThreadPriority(void) const {

	NVTHREADWIN_LOG("NvThreadWin::getThreadPriority");

	return this->getThreadPriorityPrivate();
}

int NvThreadWin::getThreadCurrentPriority(void) const {

	NVTHREADWIN_LOG("NvThreadWin::getThreadCurrentPriority");

	return this->getThreadPriorityPrivate();
}

void NvThreadWin::setThreadName(const char* name) {

	NVTHREADWIN_LOG("NvThreadWin::setThreadName");

	this->setThreadNamePrivate(name);
}

void NvThreadWin::setThreadNamePointer(const char* name) {

	NVTHREADWIN_LOG("NvThreadWin::setThreadNamePointer");

	this->setThreadNamePrivate(name);
}

const char* NvThreadWin::getThreadNamePointer(void) const {

	NVTHREADWIN_LOG("NvThreadWin::getThreadNamePointer");

	// There is no Windows native API to retrieve a thread's name in Windows.
	// Instead, if there is no currently assigned name, a string with the
	// thread ID will be used instead.

	if (nullptr != mName)
		return mName;

	const unsigned int nameLen =
		static_cast<unsigned int>(NvThread::ThreadNameLengthMax);

	void* nameVoid = malloc( (nameLen + 1U) * sizeof(char) );
	if (nullptr == nameVoid)
		throw new NvThreadExceptionWin;

	char* name = static_cast<char*>(nameVoid);

	name[nameLen] = '\0';

#ifdef _WIN32
	sprintf_s(name, nameLen+1, "%ld", mThreadId);
#else
	sprintf(name, "%ld", mThreadId);
#endif

	return name; // remember to free!
}

void NvThreadWin::setThreadNamePrivate(const char* name) {

	NVTHREADWIN_LOG("NvThreadWin::setThreadNamePrivate");

	if (nullptr == mName) {
		if (nullptr == name)
			return;
	}
	else if (strcmp(mName, name) == 0)
		return;

	if (nullptr != name) {
		this->freeThreadName();

#ifdef _WIN32
		mName = _strdup(name); //make sure to free it in destructor!
#else
		mName = strdup(name); //make sure to free it in destructor!
#endif
	}
		
	// Setting thread names:
	// https://msdn.microsoft.com/en-us/library/xcb2z8hs.aspx

	THREADNAME_INFO info;
	info.dwType = NV_0x1000_DWORD;
	info.szName = name;
	info.dwThreadID = mThreadId;
	info.dwFlags = NV_ZERO_DWORD;

	__try {
		RaiseException(MS_VC_EXCEPTION, 0, sizeof(info) / sizeof(ULONG_PTR),
			reinterpret_cast<ULONG_PTR*>(&info));
	}
	__except (EXCEPTION_EXECUTE_HANDLER) {
		// Do nothing! Just raising the exception for the VC debugger
		// (which will change the thread name)
	}
}

void NvThreadWin::freeThreadName(void) {

	NVTHREADWIN_LOG("NvThreadWin::freeThreadName");

	if (nullptr != mName)
		free(const_cast<char*>(mName));
}

int NvThreadWin::getThreadPriorityPrivate(void) const {

	NVTHREADWIN_LOG("NvThreadWin::getThreadPriorityPrivate");

	// Quick check to make sure our current NN priority maps properly to the
	// current Windows priority
	const int computedWinPriority = this->getNNToWinPriority(mPriority);
	const int actualWinPriority = GetThreadPriority(mThreadHnd);
	if (computedWinPriority != actualWinPriority)
		throw new NvThreadExceptionWin;

	return mPriority;
}

int NvThreadWin::getNNToWinPriority(const int nnPriority) const {

	NVTHREADWIN_LOG("NvThreadWin::getNNToWinPriority");

	//Windows info from
	//https://msdn.microsoft.com/en-us/library/windows/desktop/ms685100(v=vs.85).aspx
	//NN info from "os_ThreadCommon.h"
	
	// Sanitize input
	const bool descFlag =
		NvThread::HighestThreadPriority < NvThread::LowestThreadPriority;

	const int lowerBound = descFlag ?
		NvThread::HighestThreadPriority : NvThread::LowestThreadPriority;

	const int upperBound = descFlag ?
		NvThread::LowestThreadPriority  : NvThread::HighestThreadPriority;

	if ((nnPriority < lowerBound) || (nnPriority > upperBound))
		throw new NvThreadExceptionWin;

	// Check boundary cases
	switch (nnPriority) {
	case NvThread::LowestThreadPriority:
		return THREAD_PRIORITY_IDLE;
	case NvThread::DefaultThreadPriority:
		return THREAD_PRIORITY_NORMAL;
	case NvThread::HighestThreadPriority:
		return THREAD_PRIORITY_TIME_CRITICAL;
	default:
		break;
	}

	//Interpolate
	int innerPriority, outerPriority, closestOuterBoundary;
	const int distToLowest =
		std::abs(nnPriority - NvThread::LowestThreadPriority);
	const int distToHighest =
		std::abs(nnPriority - NvThread::HighestThreadPriority);

	if (distToHighest < distToLowest) {
		// Higher priority than default
		closestOuterBoundary = NvThread::HighestThreadPriority;
		innerPriority = THREAD_PRIORITY_ABOVE_NORMAL;
		outerPriority = THREAD_PRIORITY_HIGHEST;
	}
	else {
		// Lower priority than default
		closestOuterBoundary = NvThread::LowestThreadPriority;
		innerPriority = THREAD_PRIORITY_BELOW_NORMAL;
		outerPriority = THREAD_PRIORITY_LOWEST;
	}

	const int midpoint =
		std::abs(NvThread::DefaultThreadPriority - closestOuterBoundary) / 2;
	const int distToDefault =
		std::abs(NvThread::DefaultThreadPriority - nnPriority);

	if (distToDefault <= midpoint)
		return innerPriority;
	else
		return outerPriority;

	throw new NvThreadExceptionWin;
}

NvMutexWin::NvMutexWin(void) :
	mRecursive(false),
	mLockLevel(NvMutex::MutexLockLevelInitial) {

	// No default ctor
	throw new NvThreadExceptionWin;
}

NvMutexWin::NvMutexWin(const NvMutex& obj) :
	mRecursive(false),
	mLockLevel(NvMutex::MutexLockLevelInitial) {

	UNREFERENCED_PARAMETER(obj);

	// No copy ctor
	throw new NvThreadExceptionWin;
}

NvMutexWin::NvMutexWin(const bool recursive, const int lockLevel) :
	mRecursive(recursive),
	mLockLevel(lockLevel) {

	NVTHREADWIN_LOG("NvMutexWin::ctor");

	InitializeCriticalSection(&mCriticalSection);
}

NvMutexWin::~NvMutexWin(void) {

	NVTHREADWIN_LOG("NvMutexWin::dtor");

	DeleteCriticalSection(&mCriticalSection);

	mRecursive = false;
	mLockLevel = NvMutex::MutexLockLevelInitial;
}

void NvMutexWin::lockMutex(void) {

	NVTHREADWIN_LOG("NvMutexWin::lockMutex");

	EnterCriticalSection(&mCriticalSection);
}

bool NvMutexWin::tryLockMutex(void) {

	NVTHREADWIN_LOG("NvMutexWin::tryLockMutex");

	const BOOL result = TryEnterCriticalSection(&mCriticalSection);

	return (result != FALSE);
}

void NvMutexWin::unlockMutex(void) {

	NVTHREADWIN_LOG("NvMutexWin::unlockMutex");

	LeaveCriticalSection(&mCriticalSection);
}

bool NvMutexWin::isMutexLockedByCurrentThread(void) {

	NVTHREADWIN_LOG("NvMutexWin::isMutexLockedByCurrentThread");

	// This is very difficult in Windows as there is no API for this.
	// I found a couple of very convoluted WARs for this but they're even worse
	// than the voodoo for setting a thread's name.
	// We'll leave it for now until it is actually needed.

	throw new NvThreadExceptionWin;
}

const CRITICAL_SECTION* NvMutexWin::getCriticalSection(void) const {

	NVTHREADWIN_LOG("NvMutexWin::getCriticalSection");

	return &mCriticalSection;
}

NvConditionVariableWin::NvConditionVariableWin(void) {

	NVTHREADWIN_LOG("NvConditionVariableWin::ctor");

	InitializeConditionVariable(&mConditionVariable);
}

NvConditionVariableWin::NvConditionVariableWin(
	const NvConditionVariableWin& obj) {

	// No copy ctor

	throw new NvThreadExceptionWin;
}

NvConditionVariableWin::~NvConditionVariableWin(void) {

	NVTHREADWIN_LOG("NvConditionVariableWin::dtor");

	// Do nothing; there is no Windows API to delete a condition variable.
}

void NvConditionVariableWin::signalConditionVariable(void) {

	NVTHREADWIN_LOG("NvConditionVariableWin::signalConditionVariable");

	WakeConditionVariable(&mConditionVariable);
}

void NvConditionVariableWin::broadcastConditionVariable(void) {

	NVTHREADWIN_LOG("NvConditionVariableWin::broadcastConditionVariable");

	WakeAllConditionVariable(&mConditionVariable);
}

void NvConditionVariableWin::waitConditionVariable(NvMutex* mutex) {

	NVTHREADWIN_LOG("NvConditionVariableWin::waitConditionVariable");

	const bool result = baseWaitConditionVariable(mutex, INFINITE);

	if (!result) {
		getLastError();
		throw new NvThreadExceptionWin;
	}
}

NvConditionVariable::NvConditionVariableStatus
NvConditionVariableWin::timedWaitConditionVariable(NvMutex* mutex,
	long long int timeout) {

	NVTHREADWIN_LOG("NvConditionVariableWin::timedWaitConditionVariable");

	const bool result = baseWaitConditionVariable(mutex, timeout);

	const NvConditionVariableStatus status = result ?
		NvConditionVariableStatus::NvConditionVariableStatus_NoTimeout :
		NvConditionVariableStatus::NvConditionVariableStatus_Timeout;

	return status;
}

bool NvConditionVariableWin::baseWaitConditionVariable(NvMutex* mutex,
	long long int timeout) {

	NVTHREADWIN_LOG("NvConditionVariableWin::baseWaitConditionVariable");

	if (nullptr == mutex)
		throw new NvThreadExceptionWin;

	NvMutexWin* mutexWin = dynamic_cast<NvMutexWin*>(mutex);
	if (nullptr == mutexWin)
		throw new NvThreadExceptionWin;

	const CRITICAL_SECTION* temp = mutexWin->getCriticalSection();
	if (nullptr == temp)
		throw new NvThreadExceptionWin;

	CRITICAL_SECTION* critSec = const_cast<CRITICAL_SECTION*>(temp);

	const BOOL result =
		SleepConditionVariableCS(&mConditionVariable, critSec, timeout);

	return (result != FALSE);
}

const char* NvThreadExceptionWin::what(void) const throw() {

	NVTHREADWIN_LOG("NvThreadExceptionWin::what");

	return "NvThreadExceptionWin thrown!";
}

NvThreadManagerWin::NvThreadManagerWin(void) {

	NVTHREADWIN_LOG("NvThreadManagerWin::ctor");

	mThreadMap.clear();
}

NvThreadManagerWin::NvThreadManagerWin(const NvThreadManagerWin& obj) {

	UNREFERENCED_PARAMETER(obj);

	// No copy ctor
	throw new NvThreadExceptionWin;
}

NvThreadManagerWin::~NvThreadManagerWin(void) {

	NVTHREADWIN_LOG("NvThreadManagerWin::dtor");

	mThreadMap.clear(); //this calls all dtors
}

NvThread* NvThreadManagerWin::createThread(NvThreadFunction function,
	void* argument, void* stack, size_t stackSize, int priority) {

	NVTHREADWIN_LOG("NvThreadManagerWin::createThread");

	NvThreadWin* newThread =
		new NvThreadWin(function, argument, stack, stackSize, priority);
	if (nullptr == newThread)
		throw new NvThreadExceptionWin;

	const DWORD threadId = newThread->getThreadId();

	mThreadMap[threadId] = newThread;

	return newThread;
}

void NvThreadManagerWin::destroyThread(NvThread* thread) {

	NVTHREADWIN_LOG("NvThreadManagerWin::deleteThread");

	if (nullptr == thread)
		throw new NvThreadExceptionWin;

	NvThreadWin* threadWin = dynamic_cast<NvThreadWin*>(thread);
	if (nullptr == threadWin)
		throw new NvThreadExceptionWin;

	const DWORD threadId = threadWin->getThreadId();

	mThreadMap.erase(threadId);

	delete thread;
}

void NvThreadManagerWin::yieldThread(void) {

	NVTHREADWIN_LOG("NvThreadManagerWin::yieldThread");

	const BOOL result = SwitchToThread();

#if NVTHREADWIN_DEBUG
	if (result != TRUE)
		LOGI("Thread tried to yield but no other threads to execute");
#endif // NVTHREADWIN_DEBUG
}

void NvThreadManagerWin::sleepThread(long long int time) {

	NVTHREADWIN_LOG("NvThreadManagerWin::sleepThread");

	const long long int miliSecs = time / 1000LL;

	if (miliSecs > NV_ZERO_LONG_LONG) {

		const DWORD sleepTime = static_cast<DWORD>(miliSecs);
		if (NULL == sleepTime)
			throw new NvThreadExceptionWin;

		const DWORD result = SleepEx(sleepTime, false);
		if (result != NV_ZERO_DWORD)
			throw new NvThreadExceptionWin;
	}
}

NvThread* NvThreadManagerWin::getCurrentThread(void) const {

	NVTHREADWIN_LOG("NvThreadManagerWin::getCurrentThread");

	const DWORD currThreadId = GetCurrentThreadId();

	NvThreadWin* currThreadWin = mThreadMap.at(currThreadId);
	if (nullptr == currThreadWin)
		throw new NvThreadExceptionWin;

	return currThreadWin;
}

int NvThreadManagerWin::getCurrentProcessorNumber(void) const {

	NVTHREADWIN_LOG("NvThreadManagerWin::getCurrentProcessorNumber");

	const DWORD procNumDword = GetCurrentProcessorNumber();

	const int procNumInt = static_cast<int>(procNumDword);

	return procNumInt;
}

NvMutex* NvThreadManagerWin::initializeMutex(const bool recursive,
	const int lockLevel) const {

	NVTHREADWIN_LOG("NvThreadManagerWin::initializeMutex");

	NvMutexWin* newMutex = new NvMutexWin(recursive, lockLevel);
	if (nullptr == newMutex)
		throw new NvThreadExceptionWin;

	return newMutex;
}

void NvThreadManagerWin::finalizeMutex(NvMutex* mutex) const {

	NVTHREADWIN_LOG("NvThreadManagerWin::finalizeMutex");

	if (nullptr == mutex)
		throw new NvThreadExceptionWin;

	NvMutexWin* mutexWin = dynamic_cast<NvMutexWin*>(mutex);
	if (nullptr == mutexWin)
		throw new NvThreadExceptionWin;

	delete mutex;
}

NvConditionVariable*
NvThreadManagerWin::initializeConditionVariable(void) const {

	NVTHREADWIN_LOG("NvThreadManagerWin::initializeConditionVariable");

	NvConditionVariableWin* newConditionVariable =
		new NvConditionVariableWin();
	if (nullptr == newConditionVariable)
		throw new NvThreadExceptionWin;

	return newConditionVariable;
}

void NvThreadManagerWin::finalizeConditionVariable(
	NvConditionVariable* conditionVariable) const {

	NVTHREADWIN_LOG("NvThreadManagerWin::finalizeConditionVariable");

	if (nullptr == conditionVariable)
		throw new NvThreadExceptionWin;

	NvConditionVariableWin* conditionVariableWin =
		dynamic_cast<NvConditionVariableWin*>(conditionVariable);
	if (nullptr == conditionVariableWin)
		throw new NvThreadExceptionWin;

	delete conditionVariable;
}
