#ifndef SNAKE_CORE_THREAD_GROUP_H
#define SNAKE_CORE_THREAD_GROUP_H

#include <snake/concurrency/ThreadGroupT.h>
#include <snake/concurrency/ThreadQueueGroupT.h>
#include <snake/concurrency/ThreadDispatchT.h>
#include <snake/concurrency/ThreadMessageQueueT.h>
#include <snake/concurrency/ThreadMessageContainerT.h>

namespace snake
{
	template<typename T>
	using DefaultThreadMessageQueueT = ThreadMessageQueueT<T, QueueT, exit_after_handle_all>;

	template<typename T,
		typename R = void,
		template<typename U> class DispatchT = RoundRobinDispatchT,
		template<typename V> class QueueT = DefaultThreadMessageQueueT
	>
	using TaskExecutor = ThreadGroupT <T, R, SharedQueueGroupT, DispatchT, DefaultThreadMessageQueueT>;

	template<typename T,
		typename R = void,
		template<typename U> class DispatchT = RoundRobinDispatchT,
		template<typename V> class QueueT = DefaultThreadMessageQueueT
	>
	using EventLoopExecutor = ThreadGroupT <T, R, DedicatedQueueGroupT, DispatchT, DefaultThreadMessageQueueT>;
}


#endif
