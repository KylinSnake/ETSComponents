#ifndef SNAKE_CORE_MUTEX_H
#define SNAKE_CORE_MUTEX_H

#include <mutex>
#include <snake/concurrency/Atomic.h>

namespace snake
{
	namespace core
	{
		using Mutex = std::mutex;
		using RecurisveMutex = std::recursive_mutex;
		using RecursiveTimedMutex = std::recursive_timed_mutex;
		using TimedMutex = std::timed_mutex;

		template <class T>
		using LockGuard = std::lock_guard<T>;

		template <class T>
		using UniqueLock = std::unique_lock<T>;

		// Std Requirement: Lockable
		class LockFreeMutex
		{
		public:
			constexpr LockFreeMutex() noexcept = default;
			~LockFreeMutex() = default;
			LockFreeMutex(const LockFreeMutex&) = delete;
			const LockFreeMutex& operator=(const LockFreeMutex&) = delete;

			bool try_lock();
			void lock();
			void unlock();

		private:
			AtomicFlag flag_{};
		};

		// Std Requirement: SharedMutex
		class LockFreeSharedMutex
		{
			constexpr LockFreeSharedMutex() noexcept = default;
			~LockFreeSharedMutex() = default;
			LockFreeSharedMutex(const LockFreeMutex&) = delete;
			const LockFreeSharedMutex& operator=(const LockFreeSharedMutex&) = delete;

			bool try_lock();
			bool try_lock_shared();

			void lock();
			void lock_shared();

			void unlock();
			void unlock_shared();

		private:
			LockFreeMutex write_{};
#if ATOMIC_INT_LOCK_FREE == 2
			Atomic<unsigned int> count_{0};
#else
			LockFreeMutex read_ {};
			unsigned std::int32_t count_{0};
#endif
		};
	}
}

#endif
