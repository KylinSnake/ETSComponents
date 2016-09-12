#ifndef SNAKE_CORE_MUTEX_H
#define SNAKE_CORE_MUTEX_H

#include <mutex>

namespace snake
{
	namespace core
	{
		using mutex = std::mutex;
		using recurisve_mutex = std::recursive_mutex;
		using recursive_timed_mutex = std::recursive_timed_mutex;
		using timed_mutex = std::timed_mutex;

		template <class T>
		using lock_guard = std::lock_guard<T>;

		template <class T>
		using unique_lock = std::unique_lock<T>;
	}
}

#endif