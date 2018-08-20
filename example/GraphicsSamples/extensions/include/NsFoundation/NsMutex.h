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

#ifndef NV_NSFOUNDATION_NSMUTEX_H
#define NV_NSFOUNDATION_NSMUTEX_H

#include "NsAllocator.h"

/*
 * This <new> inclusion is a best known fix for gcc 4.4.1 error:
 * Creating object file for apex/src/NsAllocator.cpp ...
 * In file included from apex/include/NsFoundation.h:30,
 *                from apex/src/NsAllocator.cpp:26:
 * apex/include/NsMutex.h: In constructor  'nvidia::shdfnd::MutexT<Alloc>::MutexT(const Alloc&)':
 * apex/include/NsMutex.h:92: error: no matching function for call to 'operator new(unsigned int,
 * nvidia::shdfnd::MutexImpl*&)'
 * <built-in>:0: note: candidates are: void* operator new(unsigned int)
 */
#include <new>

namespace nvidia
{
namespace shdfnd
{
class NV_FOUNDATION_API MutexImpl
{
  public:
	/**
	The constructor for Mutex creates a mutex. It is initially unlocked.
	*/
	MutexImpl();

	/**
	The destructor for Mutex deletes the mutex.
	*/
	~MutexImpl();

	/**
	Acquire (lock) the mutex. If the mutex is already locked
	by another thread, this method blocks until the mutex is
	unlocked.
	*/
	void lock();

	/**
	Acquire (lock) the mutex. If the mutex is already locked
	by another thread, this method returns false without blocking.
	*/
	bool trylock();

	/**
	Release (unlock) the mutex.
	*/
	void unlock();

	/**
	Size of this class.
	*/
	static const uint32_t& getSize();
};

template <typename Alloc = ReflectionAllocator<MutexImpl> >
class MutexT : protected Alloc
{
	NV_NOCOPY(MutexT)
  public:
	class ScopedLock
	{
		MutexT<Alloc>& mMutex;
		NV_NOCOPY(ScopedLock)
	  public:
		NV_INLINE ScopedLock(MutexT<Alloc>& mutex) : mMutex(mutex)
		{
			mMutex.lock();
		}
		NV_INLINE ~ScopedLock()
		{
			mMutex.unlock();
		}
	};

	/**
	The constructor for Mutex creates a mutex. It is initially unlocked.
	*/
	MutexT(const Alloc& alloc = Alloc()) : Alloc(alloc)
	{
		mImpl = (MutexImpl*)Alloc::allocate(MutexImpl::getSize(), __FILE__, __LINE__);
		NV_PLACEMENT_NEW(mImpl, MutexImpl)();
	}

	/**
	The destructor for Mutex deletes the mutex.
	*/
	~MutexT()
	{
		mImpl->~MutexImpl();
		Alloc::deallocate(mImpl);
	}

	/**
	Acquire (lock) the mutex. If the mutex is already locked
	by another thread, this method blocks until the mutex is
	unlocked.
	*/
	void lock() const
	{
		mImpl->lock();
	}

	/**
	Acquire (lock) the mutex. If the mutex is already locked
	by another thread, this method returns false without blocking,
	returns true if lock is successfully acquired
	*/
	bool trylock() const
	{
		return mImpl->trylock();
	}

	/**
	Release (unlock) the mutex, the calling thread must have
	previously called lock() or method will error
	*/
	void unlock() const
	{
		mImpl->unlock();
	}

  private:
	MutexImpl* mImpl;
};

class NV_FOUNDATION_API ReadWriteLock
{
	NV_NOCOPY(ReadWriteLock)
  public:
	ReadWriteLock();
	~ReadWriteLock();

	void lockReader();
	void lockWriter();

	void unlockReader();
	void unlockWriter();

  private:
	class ReadWriteLockImpl* mImpl;
};

class ScopedReadLock
{
	NV_NOCOPY(ScopedReadLock)
  public:
	NV_INLINE ScopedReadLock(ReadWriteLock& lock) : mLock(lock)
	{
		mLock.lockReader();
	}
	NV_INLINE ~ScopedReadLock()
	{
		mLock.unlockReader();
	}

  private:
	ReadWriteLock& mLock;
};

class ScopedWriteLock
{
	NV_NOCOPY(ScopedWriteLock)
  public:
	NV_INLINE ScopedWriteLock(ReadWriteLock& lock) : mLock(lock)
	{
		mLock.lockWriter();
	}
	NV_INLINE ~ScopedWriteLock()
	{
		mLock.unlockWriter();
	}

  private:
	ReadWriteLock& mLock;
};

typedef MutexT<> Mutex;

/*
 * Use this type of lock for mutex behaviour that must operate on SPU and PPU
 * On non-PS3 platforms, it is implemented using Mutex
 */
#if !NV_PS3
class AtomicLock
{
	Mutex mMutex;
	NV_NOCOPY(AtomicLock)

  public:
	AtomicLock()
	{
	}

	bool lock()
	{
		mMutex.lock();
		return true;
	}

	bool trylock()
	{
		return mMutex.trylock();
	}

	bool unlock()
	{
		mMutex.unlock();
		return true;
	}
};

class AtomicLockCopy
{
	AtomicLock* pLock;

  public:
	AtomicLockCopy() : pLock(NULL)
	{
	}

	AtomicLockCopy& operator=(AtomicLock& lock)
	{
		pLock = &lock;
		return *this;
	}

	bool lock()
	{
		return pLock->lock();
	}

	bool trylock()
	{
		return pLock->trylock();
	}

	bool unlock()
	{
		return pLock->unlock();
	}
};
#else
struct AtomicLockImpl
{
	NV_ALIGN(128, uint32_t m_Lock);
	int32_t m_LockId;
	uint32_t m_LockCount;

	AtomicLockImpl();
};
class AtomicLock
{
	friend class AtomicLockCopy;
	AtomicLockImpl* m_pImpl;

  public:
	AtomicLock();

	~AtomicLock();

	bool lock();

	bool trylock();

	bool unlock();
};

// if an AtomicLock is copied and then the copy goes out of scope, it'll delete the atomic primitive
// (just a 128-byte aligned int) and cause a crash when it tries to delete it again
// This class just uses the atomic primitive without releasing it in the end.

class AtomicLockCopy
{
	AtomicLockImpl* m_pImpl;

  public:
	AtomicLockCopy() : m_pImpl(NULL)
	{
	}

	AtomicLockCopy(const AtomicLock& lock) : m_pImpl(lock.m_pImpl)
	{
	}

	~AtomicLockCopy()
	{
	}

	AtomicLockCopy& operator=(const AtomicLock& lock)
	{
		m_pImpl = lock.m_pImpl;
		return *this;
	}

	bool lock();

	bool trylock();

	bool unlock();
};
#endif

#if !NV_PS3

class AtomicRwLock
{
	ReadWriteLock m_Lock;
	NV_NOCOPY(AtomicRwLock)

  public:
	AtomicRwLock()
	{
	}

	void lockReader()
	{
		m_Lock.lockReader();
	}
	void lockWriter()
	{
		m_Lock.lockWriter();
	}

	bool tryLockReader()
	{
		// Todo - implement this
		m_Lock.lockReader();
		return true;
	}

	void unlockReader()
	{
		m_Lock.unlockReader();
	}
	void unlockWriter()
	{
		m_Lock.unlockWriter();
	}
};
#else

struct AtomicRwLockImpl
{
	NV_ALIGN(128, volatile uint32_t m_Lock);
	NV_ALIGN(128, volatile uint32_t m_ReadCounter);
	int32_t m_LockId;
	uint32_t m_LockCount;

	AtomicRwLockImpl();
};

class AtomicRwLock
{
	NV_NOCOPY(AtomicRwLock)
	AtomicRwLockImpl* m_pImpl;

  public:
	AtomicRwLock();

	~AtomicRwLock();

	void lockReader();

	bool tryLockReader();

	void lockWriter();

	void unlockReader();

	void unlockWriter();
};

#endif

class ScopedAtomicLock
{
	NV_INLINE ScopedAtomicLock(AtomicLock& lock) : mLock(lock)
	{
		mLock.lock();
	}
	NV_INLINE ~ScopedAtomicLock()
	{
		mLock.unlock();
	}

	NV_NOCOPY(ScopedAtomicLock)
  private:
	AtomicLock& mLock;
};

} // namespace shdfnd
} // namespace nvidia

#endif // #ifndef NV_NSFOUNDATION_NSMUTEX_H
