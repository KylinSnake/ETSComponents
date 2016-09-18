#ifndef SNAKE_CORE_THREAD_MESSAGE_QUEUE_T_H
#define SNAKE_CORE_THREAD_MESSAGE_QUEUE_T_H

#include "ThreadMessageContainerT.h"
#include "ThreadExitStrategyT.h"
#include "ConditionVariable.h"
#include <exception>

namespace snake
{
	namespace core
	{
		template<typename T, 
			template<typename T> class ContainerT = QueueT,
			template<typename T> class ExitStrategyT = exit_after_handle_all,
			typename LockT = mutex>
		class ThreadMessageQueueT
		{
		public:
			ThreadMessageQueueT()
				: list_()
				, lock_()
				, cv_()
				, exit_strategy_()
			{
			}
			~ThreadMessageQueueT() = default;
			ThreadMessageQueueT( const ThreadMessageQueueT& ) = delete;
			ThreadMessageQueueT& operator=( const ThreadMessageQueueT& ) = delete;

			bool push( T&& t )
			{
				unique_lock<LockT> guard( lock_ );
				if (!exit_strategy_.push_stopped(list_))
				{
					list_.push( std::move( t ) );
					cv_.notify_all();
					return true;
				}
				return false;
			}
			bool pop(T& t)
			{
				unique_lock<LockT> guard( lock_ );
				while (!exit_strategy_.pop_stopped( list_ ) && list_.empty())
				{
					cv_.wait_for( guard, std::chrono::milliseconds( 50 ) );
				}
				if (exit_strategy_.pop_stopped( list_ ))
				{
					return false;
				}
				t = std::move(list_.top());
				list_.pop();
				return true;
			}
			bool empty() const
			{
				unique_lock<LockT> guard( lock_ );
				return list_.empty();
			}

			void stop()
			{
				unique_lock<LockT> guard( lock_ );
				if (exit_strategy_.available_stop( list_ ))
				{
					exit_strategy_.stop();
					cv_.notify_all();
				}
			}
		private:
			ContainerT<T> list_;
			LockT lock_;
			condition_variable cv_;
			ExitStrategyT<ContainerT<T>> exit_strategy_;
		};
	}
}

#endif