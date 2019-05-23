#ifndef SNAKE_CORE_THREAD_MESSAGE_QUEUE_T_H
#define SNAKE_CORE_THREAD_MESSAGE_QUEUE_T_H

#include <snake/concurrency/ThreadMessageContainerT.h>
#include <snake/concurrency/ThreadExitStrategyT.h>
#include <snake/concurrency/ConditionVariable.h>
#include <exception>

namespace snake
{
	namespace concurrency
	{
		template<typename T, 
			template<typename U> class ContainerT = QueueT,
			template<typename V> class ExitStrategyT = exit_after_handle_all,
			typename LockT = Mutex>
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
				UniqueLock<LockT> guard( lock_ );
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
				UniqueLock<LockT> guard( lock_ );
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
				UniqueLock<LockT> guard( lock_ );
				return list_.empty();
			}

			void stop()
			{
				UniqueLock<LockT> guard( lock_ );
				if (exit_strategy_.available_stop( list_ ))
				{
					exit_strategy_.stop();
					cv_.notify_all();
				}
			}

			void clear(std::function<void(T&)> clean_function)
			{
				typename ContainerT<T>::CollectionType col;
				{
					UniqueLock<LockT> guard( lock_ );
					list_.swap( col );
				}
				for (auto& t : col)
				{
					clean_function( t );
				}
			}
		private:
			ContainerT<T> list_;
			LockT lock_;
			ConditionVariable cv_;
			ExitStrategyT<ContainerT<T>> exit_strategy_;
		};
	}
}

#endif
