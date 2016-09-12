#ifndef SNAKE_CORE_FUTURE_H
#define SNAKE_CORE_FUTURE_H

#include <future>

namespace snake
{
	namespace core
	{
		template <class T>
		using future = std::future<T>;
		template <class T, class ...Arg>
		using packaged_task = std::packaged_task<T(Arg...)>;
		template <class T>
		using promise = std::promise<T>;
		template<class T>
		using shared_future = std::shared_future<T>;
	}
}

#endif