#ifndef SNAKE_CORE_THREAD_GROUP_H
#define SNAKE_CORE_THREAD_GROUP_H

#include <cassert>
#include <memory>
#include <vector>
#include <tuple>
#include <algorithm>
#include "Mutex.h"
#include "ConditionVariable.h"
#include "Thread.h"
#include "Atomic.h"

namespace snake
{
	namespace core
	{
		template<class T
			, template<class T, template<class T> class QueueT> class QueueGroupT
			, class R
			, template<class T> class DispatchT
			, template<class T> class QueueT
		>
			class ThreadGroupT
		{
		public:
			using WorkFunction = std::function<R( T& )>;
			ThreadGroupT( WorkFunction&& func, size_t num ) 
				: func_(std::move(func))
				, num_(num)
				, queue_group_(num)
				, dispatcher_(num)
				, threads_(num)
				, flag_()
			{
				assert( num > 0 );
			}
			~ThreadGroupT()
			{
				stop();
			}
			ThreadGroupT( const ThreadGroupT & ) = delete;
			ThreadGroupT& operator=( const ThreadGroupT& ) = delete;

			future<R> push( T&& t )
			{
				promise<R> p;
				future<R> f = p.get_future();
				queue_group_.get_queue( dispatcher_.next(t) ).push( std::make_tuple(std::move( t ), std::move(p)) );
				return f;
			}

			void start()
			{
				for (size_t i = 0; i < num_; ++i)
				{
					QueueT<std::tuple<T, promise<R>>>& queue = queue_group_.get_queue( i );
					threads_[i].reset( new thread( [&queue, this]
					{
						std::tuple<T, promise<R>> t;
						while (queue.pop( t ))
						{
							try
							{
								std::get<1>(t).set_value( this->func_( std::get<0>(t) ));
							}
							catch (...)
							{
								try
								{
									std::get<1>( t ).set_exception( std::current_exception() );
								}
								catch (...){}
							}
						}
					}) );
				}
			}
			void stop()
			{
				if (!flag_.test_and_set())
				{
					for (size_t i = 0; i < threads_.size(); ++i)
					{
						queue_group_.get_queue( i ).stop();
					}
					for (auto& t : threads_)
					{
						t->join();
					}
					threads_.clear();
				}
			}
		private:
			WorkFunction func_;
			size_t num_;
			QueueGroupT<std::tuple<T, promise<R>>, QueueT> queue_group_;
			DispatchT<T> dispatcher_;
			std::vector<std::unique_ptr<thread>> threads_;
			atomic_flag flag_;
		};

	}
}

#endif