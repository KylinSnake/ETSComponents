#ifndef SNAKE_CORE_THREAD_GROUP_H
#define SNAKE_CORE_THREAD_GROUP_H

#include "ThreadGroupT.h"
#include "ThreadQueueGroupT.h"
#include "ThreadDispatchT.h"
#include "ThreadMessageQueueT.h"
#include "ThreadMessageContainerT.h"

namespace snake
{
	namespace core
	{

		template<typename T>
		using DefaultThreadMessageQueueT = ThreadMessageQueueT<T, QueueT, exit_after_handle_all>;

		template<typename T,
			typename R = void,
			template<typename T> class DispatchT = RoundRobinDispatchT,
			template<typename T> class QueueT = DefaultThreadMessageQueueT
		>
			using TaskExecutor = ThreadGroupT <T, R, SharedQueueGroupT, DispatchT, DefaultThreadMessageQueueT>;

		template<typename T,
			typename R = void,
			template<typename T> class DispatchT = RoundRobinDispatchT,
			template<typename T> class QueueT = DefaultThreadMessageQueueT
		>
			using EventLoopExecutor = ThreadGroupT <T, R, DedicatedQueueGroupT, DispatchT, DefaultThreadMessageQueueT>;
	}
}


#endif