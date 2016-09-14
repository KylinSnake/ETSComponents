#ifndef SNAKE_CORE_THREAD_QUEUE_H
#define SNAKE_CORE_THREAD_QUEUE_H

#include <list>
#include "ConditionVariable.h"

namespace snake
{
	namespace core
	{
		template<class T, class LockT>
		class QueueT
		{
		public:
			QueueT()
				: list_()
				, lock_()
				, cv_()
				, running_(true)
			{
			}
			~QueueT() = default;
			QueueT( const QueueT& ) = delete;
			QueueT& operator=( const QueueT& ) = delete;

			void push( const T& t )
			{
				unique_lock<LockT> guard( lock_ );
				list_.push_back( t );
				cv_.notify_all();
			}
			void push( T&& t )
			{
				unique_lock<LockT> guard( lock_ );
				list_.push_back( std::move( t ) );
				cv_.notify_all();
			}
			bool pop(T& t)
			{
				unique_lock<LockT> guard( lock_ );
				while (running_ && list_.empty())
				{
					cv_.wait_for( guard, std::chrono::milliseconds( 50 ) );
				}
				if (list_.empty())
				{
					return false;
				}

				t = std::move( list_.front() );
				list_.pop_front();;
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
				if (running_)
				{
					running_ = false;
					cv_.notify_all();
				}
			}
		private:
			std::list<T> list_;
			LockT lock_;
			condition_variable cv_;
			bool running_;
		};
	}
}

#endif