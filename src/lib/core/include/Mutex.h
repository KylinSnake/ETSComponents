#ifndef SNAKE_CORE_MUTEX_H
#define SNAKE_CORE_MUTEX_H

#include <mutex>

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
	}
}

#endif