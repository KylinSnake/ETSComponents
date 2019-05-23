#ifndef SNAKE_CORE_FUTURE_H
#define SNAKE_CORE_FUTURE_H

#include <future>

namespace snake
{
	namespace core
	{
		template <class T>
		using Future = std::future<T>;
		template <class T, class ...Arg>
		using PackagedTask = std::packaged_task<T(Arg...)>;
		template <class T>
		using Promise = std::promise<T>;
		template<class T>
		using SharedFuture = std::shared_future<T>;
		using FutureStatus = std::future_status;
	}
}

#endif