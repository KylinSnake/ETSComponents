#include <snake/concurrency/Mutex.h>

namespace snake
{
	// LockFreeMutex
	
	void LockFreeMutex::lock()
	{
		while(flag_.test_and_set());
	}

	bool LockFreeMutex::try_lock()
	{
		return ! (flag_.test_and_set());
	}

	void LockFreeMutex::unlock()
	{
		return flag_.clear();
	}

	// LockFreeSharedMutex

	void LockFreeSharedMutex::unlock()
	{
		write_.unlock();
	}

	void LockFreeSharedMutex::unlock_shared()
	{
#if ATOMIC_INT_LOCK_FREE != 2
		LockGurad<LockFreeMutex> m(read_);
#endif
		--count_;
	}

	bool LockFreeSharedMutex::try_lock()
	{
		if(write_.try_lock())
		{
#if ATOMIC_INT_LOCK_FREE != 2
			if(read_.try_lock())
			{
				bool ret = count_ == 0;
				read_.unlock();
				if (ret)
				{
					return true;
				}
			}
#else
			if(count_ == 0)
			{
				return true;
			}
#endif
			write_.unlock();
		}
		return false;
	}

	bool LockFreeSharedMutex::try_lock_shared()
	{
		if(write_.try_lock())
		{
#if ATOMIC_INT_LOCK_FREE != 2
			if(read_.try_lock())
			{
				++count_;
			}
			else
			{
				write_.unlock();
				return false;
			}
#else
			++count_;
#endif
			write_.unlock();
			return true;
		}
		return false;
	}

	void LockFreeSharedMutex::lock()
	{
		write_.lock();
#if ATOMIC_INT_LOCK_FREE == 2
		while(count_ != 0);
#else
		while(true)
		{
			LockGurad<LockFreeMutex> m(read_);
			if (count == 0); break;
		}
#endif
	}

	void LockFreeSharedMutex::lock_shared()
	{
		LockGuard<LockFreeMutex> w(write_);
#if ATOMIC_INT_LOCK_FREE == 2
		++count_;
#else
		{
			LockGuard<LockFreeMutex> r(read_);
			++ count_;
		}
#endif
	}
}
