//----------------------------------------------------------------------------------
// File:        NvAppBase\linux/NvThreadPosix.cpp
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

#include "NvThreadPosix.h"
#include <asm-generic/errno.h>
#include <unistd.h>

NvThreadPosix::NvThreadPosix(NvThreadFunction function, void* argument, void* stack,
	size_t stackSize, int priority) {
	pthread_attr_init(&m_attr);
	pthread_attr_setstacksize(&m_attr, stackSize);
	m_func = function;
	m_arg = argument;
}

	/// \brief Destructor
NvThreadPosix::~NvThreadPosix(void) {

}

void NvThreadPosix::startThread(void) {
	pthread_create(&m_impl, &m_attr, m_func, m_arg);
}

void NvThreadPosix::waitThread(void) {
	pthread_join(m_impl, NULL);
}

NvMutexPosix::NvMutexPosix(const bool recursive, const int lockLevel) {
	pthread_mutex_init(&m_impl, NULL);
}

NvMutexPosix::~NvMutexPosix(void) {

}

void NvMutexPosix::lockMutex(void) {
	pthread_mutex_lock(&m_impl);
}

bool NvMutexPosix::tryLockMutex(void) {
	return !pthread_mutex_trylock(&m_impl);
}

void NvMutexPosix::unlockMutex(void) {
	pthread_mutex_unlock(&m_impl);
}

bool NvMutexPosix::isMutexLockedByCurrentThread(void) {
	return EDEADLK == pthread_mutex_trylock(&m_impl);
}

NvConditionVariablePosix::NvConditionVariablePosix(void) {
	pthread_cond_init(&m_impl, NULL);
}

NvConditionVariablePosix::~NvConditionVariablePosix(void) {
}

void NvConditionVariablePosix::signalConditionVariable(void) {
	pthread_cond_signal(&m_impl);
}

void NvConditionVariablePosix::broadcastConditionVariable(void) {
	pthread_cond_broadcast(&m_impl);
}

void NvConditionVariablePosix::waitConditionVariable(NvMutex* mutex) {
	pthread_cond_wait(&m_impl, &(((NvMutexPosix*)mutex)->m_impl));
}

NvConditionVariable::NvConditionVariableStatus NvConditionVariablePosix::timedWaitConditionVariable(
	NvMutex* mutex, long long int timeout) {
	struct timespec time;
	time.tv_nsec = timeout;
	time.tv_sec = 0;
	int res = pthread_cond_timedwait(&m_impl, &(((NvMutexPosix*)mutex)->m_impl), &time);
	return (res == ETIMEDOUT) ? NvConditionVariableStatus_Timeout :
		NvConditionVariableStatus_NoTimeout;
}

const char* NvThreadExceptionPosix::what(void) const throw() {
	return "NA";
}

NvThreadManagerPosix::NvThreadManagerPosix(void) { 
}

NvThreadManagerPosix::~NvThreadManagerPosix(void) {

}

NvThread* NvThreadManagerPosix::createThread(NvThreadFunction function, void* argument,
	void* stack, size_t stackSize, int priority) {
	return new NvThreadPosix(function, argument, stack, stackSize, priority);
} 

void NvThreadManagerPosix::destroyThread(NvThread* thread) {

}

void NvThreadManagerPosix::yieldThread(void) {

}

void NvThreadManagerPosix::sleepThread(long long int time) {
	usleep(time);
}

NvThread* NvThreadManagerPosix::getCurrentThread(void) const {
	return NULL;
}

int NvThreadManagerPosix::getCurrentProcessorNumber(void) const {
	return 0;
}

NvMutex* NvThreadManagerPosix::initializeMutex(const bool recursive, const int lockLevel)	const {
	return new NvMutexPosix(recursive, lockLevel);
}

void NvThreadManagerPosix::finalizeMutex(NvMutex* mutex) const {

}

NvConditionVariable* NvThreadManagerPosix::initializeConditionVariable(void) const {
	return new NvConditionVariablePosix;
}

void NvThreadManagerPosix::finalizeConditionVariable(
	NvConditionVariable* conditionVariable) const {

}
