//----------------------------------------------------------------------------------
// File:        NvAppBase\android/NvThreadAnd.h
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

#ifndef NV_THREAD_AND_H
#define NV_THREAD_AND_H

#include <NvAppBase/NvThread.h>
#include <pthread.h>

/// \file NvThreadAnd.h
/// Cross-platform threading wrapper API

/// \brief Abstract class that represents a thread instance.
/// Designed to be a wrapper with an almost direct mapping between the
/// thread-related APIs in the nn::os namespace. It should be derived and
/// implemented according to platform specifications.
class NvThreadAnd : public NvThread {

public:

	/// \brief Constructor
	/// Only constructor that creates an instance of a thread.
	/// \param[in] function the function pointer the thread should execute.
	/// \param[in] argument data structure with arguments to the function.
	/// \param[in] stack pointer to allocated stack memory for the thread.
	/// \param[in] stackSize size of allocated stack memory for the thread.
	///						 It must be aligned with the NVTHREAD_STACK_ALIGN
	///						 attribute declaration.
	/// \param[in] priority priority assigned to the thread; integer value
	///						between #HighestThreadPriority and
	///						#LowestThreadPriority.
	NvThreadAnd(NvThreadFunction function, void* argument, void* stack,
		size_t stackSize, int priority);

	/// \brief Destructor
	virtual ~NvThreadAnd(void);

	/// \name Threading API pure virtual methods
	/// To be overridden with platform specific calls by the implementing
	/// derived classes.
	///@{

	/// \brief Start thread execution.
	/// Starts execution of the assigned function for the subject thread.
	virtual void startThread(void);

	/// \brief Wait for thread completion.
	/// Blocks the currently executing thread until the subject thread finishes
	/// execution of its assigned function.
	virtual void waitThread(void);

	/// \brief Changes the priority of the thread.
	/// Changes the priority of the subject thread.
	/// \param[in] priority priority assigned to the thread; integer value
	///						between #HighestThreadPriority and
	///						#LowestThreadPriority.
	/// \return the previous priority value before the change.
	virtual int changeThreadPriority(int priority) {
		return 0;
	}

	/// \brief Gets the original priority of the thread.
	/// Gets the priority of the subject thread as assigned at its creation
	/// (constructor call).
	/// \return the original priority value.
	virtual int getThreadPriority(void) const {
		return 0;
	}

	/// \brief Gets the current priority of the thread.
	/// Gets the current priority of the subject thread as determined by the
	/// last call to #changeThreadPriority, if any; otherwise, as determined
	/// by the constructor call.
	/// \return the current priority value.
	virtual int getThreadCurrentPriority(void) const {
		return 0;
	}

	/// \brief Sets the name of the thread.
	/// Sets the name of the thread; useful for debugging purposes. The pointer
	/// to the character string DOES NOT need to be kept for the thread to use.
	/// \param[in] name character string with the name.
	virtual void setThreadName(const char* name) { }

	/// \brief Sets the name of the thread.
	/// Sets the name of the thread; useful for debugging purposes. The pointer
	/// to the character string DOES need to be kept for the thread to use.
	/// \param[in] name character string with the name.
	virtual void setThreadNamePointer(const char* name) { }

	/// \brief Gets the name of the thread.
	/// Gets a pointer to character string with the name of the thread; useful
	/// for debugging purposes. Do NOT modify the string.
	/// \return pointer to the character string with the name.
	virtual const char* getThreadNamePointer(void) const {
		return NULL;
	}

protected:
	/// \brief Default constructor
	/// Do NOT use. Overwritten for safety; it should trigger an assert or
	/// exception.
	NvThreadAnd(void) {}

	/// \brief Copy constructor
	/// Do NOT use. Overwritten for safety; it should trigger an assert or
	/// exception.
	/// \param[in] obj reference to instance in attempted copy
	NvThreadAnd(const NvThread& obj) { NV_UNUSED(obj); }

	pthread_t m_impl;
	pthread_attr_t m_attr;
	NvThreadFunction m_func;
	void* m_arg;
};

/// \brief Abstract class that represents a mutex instance.
/// Designed to be a wrapper with an almost direct mapping between the
/// mutex-related APIs in the nn::os namespace. It should be derived and
/// implemented according to platform specifications.
class NvMutexAnd : public NvMutex {

public:
	friend class NvConditionVariableAnd;

	/// \brief Constructor
	/// Only constructor that creates an instance of a mutex.
	/// \param[in] recursive flag that determines if the mutex is recursive
	///						 (it allows the same thread to acquire multiple
	///						 levels of ownership over the same mutex)
	/// \param[in] lockLevel maximum number of ownership levels allowed for the
	///						 mutex and the same thread
	NvMutexAnd(const bool recursive, const int lockLevel);

	/// \brief Destructor
	virtual ~NvMutexAnd(void);

	/// \name Mutex API pure virtual methods
	/// To be overridden with platform specific calls by the implementing derived
	/// classes.
	///@{

	/// \brief Lock the mutex.
	/// The currently executing thread acquires ownership of the mutex. This is
	/// a BLOCKING call (thread execution is stopped until the thread acquires
	/// the mutex, and the method will not return until this is the case).
	virtual void lockMutex(void);

	/// \brief Lock the mutex.
	/// The currently executing thread acquires ownership of the mutex. This is
	/// a NON-BLOCKING call (thread execution is NOT stopped until the thread
	/// acquires the mutex, and the method will return after the attempt).
	/// \return whether the thread acquired the mutex or not.
	virtual bool tryLockMutex(void);

	/// \brief Unlock the mutex
	/// If the mutex is recursive, and the currently executing thread had
	/// acquired ownership of the mutex, then the lock level is decreased by 1.
	/// If the lock level is 0 or if the mutex is non-recursive, and the
	/// currently executing thread had acquired ownership of the mutex, then
	/// the mutex is unlocked and available to other threads.
	virtual void unlockMutex(void);

	/// \brief Check if the mutex is owned by the current thread.
	/// This function is provided mostly for use in error-checking code.
	/// \return whether the mutex is owned by the current thread.
	virtual bool isMutexLockedByCurrentThread(void);

	///@}
protected:
	/// \brief Default constructor
	/// Do NOT use. Overwritten for safety; it should trigger an assert or
	/// exception.
	NvMutexAnd(void) {}

	/// \brief Copy constructor
	/// Do NOT use. Overwritten for safety; it should trigger an assert or
	/// exception.
	/// \param[in] obj reference to instance in attempted copy.
	NvMutexAnd(const NvMutex& obj) { NV_UNUSED(obj); }

	pthread_mutex_t m_impl;
};

/// \brief Abstract class that represents a condition variable instance.
/// Designed to be a wrapper with an almost direct mapping between the
/// condition-variable-related APIs in the nn::os namespace. It should be
/// derived and implemented according to platform specifications.
class NvConditionVariableAnd : public  NvConditionVariable {

public:

	/// \brief Default constructor
	/// Only constructor that creates an instance of a condition variable.
	NvConditionVariableAnd(void);

	/// \brief Destructor
	virtual ~NvConditionVariableAnd(void);

	/// \name Conditon variable API pure virtual methods
	/// To be overridden with platform specific calls by the implementing derived
	/// classes.
	///@{

	/// \brief Notify one thread waiting on the condition variable.
	/// Usually used to dispatch work to other threads after placing it in a
	/// queue (monitor paradigm).
	virtual void signalConditionVariable(void);

	/// \brief Notify all threads waiting on the condition variable.
	/// Usually used to dispatch work to other threads after placing it in a
	/// queue (monitor paradigm).
	virtual void broadcastConditionVariable(void);

	/// \brief Make the thread wait until the mutex is available.
	/// It will also unlock the mutex while the thread sleeps, and it will
	/// reacquire it once it wakes up to check on the condition again.
	/// \param[in] mutex the mutex associated with the condition variable.
	virtual void waitConditionVariable(NvMutex* mutex);

	/// \brief Make the thread wait until the mutex is available for a fixed amount of
	/// time.
	/// It will also unlock the mutex while the thread sleeps, and it will
	/// reacquire it once it wakes up to check on the condition again.
	/// \param[in] mutex the mutex associated with the condition variable.
	/// \param[in] timeout requested waiting time in nanoseconds.
	/// \return the #NvConditionVariableStatus enumeration signaling whether
	///			the timed wait on a condition variable exited due to a timeout
	///			or a successful mutex lock.
	virtual NvConditionVariableStatus timedWaitConditionVariable(
		NvMutex* mutex, long long int timeout);

	///@}
protected:
	/// \brief Copy constructor
	/// Do NOT use. Overwritten for safety; it should trigger an assert or
	/// exception.
	/// \param[in] obj reference to instance in attempted copy
	NvConditionVariableAnd(const NvConditionVariable& obj) {}

	pthread_cond_t m_impl;
};

/// \brief Exception class
/// Abstract exception class available in some architectures for signaling
/// when the threading API implementation entered an illegal state.
class NvThreadExceptionAnd : public NvThreadException {

public:

	/// \brief Returns exception message.
	/// Return a string with details on the illegal state the application
	/// entered.
	/// \return character string with an implementation-dependent exception
	///			message.
	virtual const char* what(void) const throw();
};

/// \brief Abstract class used to create, destroy and manage the threading
/// API's data structures.
/// While derived classes should have similar functionality, their role is to
/// create objects related to the platform-specific implementations of threads,
/// mutexes and condition variables. A single instance of this class should
/// exist for the application.
class NvThreadManagerAnd : public NvThreadManager {

public:

	/// \brief Default constructor.
	/// Use it to create a singleton instance of the derived, implementing
	/// class.
	NvThreadManagerAnd(void);

	/// \brief Destructor
	virtual ~NvThreadManagerAnd(void);

	/// \name #NvThread related methods.
	/// Used to handle creation, destruction and other non-instance-related
	/// interactions with #NvThread instances.
	///@{

	/// \brief Create an instance of a thread.
	/// Used to facilitate constructor handling as well as tracking all
	/// #NvThread objects by the application.
	/// \param[in] function the function pointer the thread should execute.
	/// \param[in] argument data structure with arguments to the function.
	/// \param[in] stack pointer to allocated stack memory for the thread.
	/// \param[in] stackSize size of allocated stack memory for the thread.
	///						 It must be aligned with the NVTHREAD_STACK_ALIGN
	///						 attribute declaration.
	/// \param[in] priority priority assigned to the thread; integer value
	///						between #HighestThreadPriority and
	///						#LowestThreadPriority.
	/// \return new thread instance.
	virtual NvThread* createThread(NvThreadFunction function, void* argument,
		void* stack, size_t stackSize, int priority);

	/// \brief Destroys an instance of a thread.
	/// This ONLY works if the thread instance has created by calling the
	/// #createThread method, as opposed to calling the #NvThread constructor
	/// directly.
	/// \param[in] thread instance to be destroyed.
	virtual void destroyThread(NvThread* thread);

	/// \brief Yields to a similarly prioritized thread.
	/// Cedes the execution opportunity of the currently executing thread to
	/// another of similar priority.
	virtual void yieldThread(void);

	/// \brief Sleep for a fixed amount of time.
	/// Pauses execution of the currently executing thread for a fixed amount of
	/// time.
	/// \param[in] time sleep time in nanoseconds.
	virtual void sleepThread(long long int time);

	/// \brief Gets the currently executing thread.
	/// This ONLY works if the thread instance has created by calling the
	/// #createThread method, as opposed to calling the #NvThread constructor
	/// directly.
	/// \return pointer to the current thread.
	virtual NvThread* getCurrentThread(void) const;

	/// \brief Gets the current processor number.
	/// Queries and returns the processor number for the currently executing
	/// thread.
	/// \return the processor number.
	virtual int getCurrentProcessorNumber(void) const;

	///@}

	/// \name #NvMutex related methods.
	/// Used to handle initialization and finalization of #NvMutex instances.
	///@{

	/// \brief Create a new mutex instance.
	/// Mutexes can be recursive (i.e. they allows the same thread to acquire
	/// multiple levels of ownership over the same mutex) or not.
	/// \param[in] recursive flag that determines if the mutex is recursive
	/// \param[in] lockLevel maximum number of ownership levels allowed for the
	///						 mutex and the same thread.
	/// \return pointer to the new mutex instance.
	virtual NvMutex* initializeMutex(const bool recursive, const int lockLevel)
		const;

	/// \brief Destroy a mutex instance.
	/// \param[in] mutex instance to be destroyed.
	virtual void finalizeMutex(NvMutex* mutex) const;

	///@}

	/// \name #NvConditionVariable related methods.
	/// Used to handle initialization and finalization of #NvConditionVariable
	/// instances.
	///@{

	/// \brief Create a new condition variable instance.
	/// \return pointer to the new condition variable instance.
	virtual NvConditionVariable* initializeConditionVariable(void) const;

	/// \brief Destroy a condition variable instance.
	/// \param[in] condition variable instance to be destroyed.
	virtual void finalizeConditionVariable(
		NvConditionVariable* conditionVariable) const;

	///@}
protected:
	/// \brief Copy constructor
	/// Do NOT use. Overwritten for safety; it should trigger an assert or
	/// exception.
	/// \param[in] obj reference to instance in attempted copy
	NvThreadManagerAnd(const NvThreadManager& obj) { NV_UNUSED(obj); }

};

#undef NV_UNUSED

#endif
