#ifndef SNAKE_CORE_THREAD_QUEUE_GROUP_T_H
#define SNAKE_CORE_THREAD_QUEUE_GROUP_T_H

#include <vector>
#include <cassert>

namespace snake
{
	namespace core
	{
		template<class T, template<class U> class QueueT>
		class SharedQueueGroupT
		{
		public:
			SharedQueueGroupT( size_t )
			{
			}
			~SharedQueueGroupT() = default;
			SharedQueueGroupT( const SharedQueueGroupT<T, QueueT>& ) = delete;
			SharedQueueGroupT& operator= ( const SharedQueueGroupT<T, QueueT>& ) = delete;
			QueueT<T>& get_queue( size_t )
			{
				return queue_;
			}
			const QueueT<T>& get_queue( size_t ) const
			{
				return queue_;
			}
		private:
			QueueT<T> queue_;
		};

		template<class T, template<class U> class QueueT>
		class DedicatedQueueGroupT
		{
		public:
			DedicatedQueueGroupT( size_t t ) : queues_( t )
			{
			}
			~DedicatedQueueGroupT() = default;
			DedicatedQueueGroupT( const DedicatedQueueGroupT<T, QueueT>& ) = delete;
			DedicatedQueueGroupT& operator= ( const DedicatedQueueGroupT<T, QueueT>& ) = delete;
			QueueT<T>& get_queue( size_t i )
			{
				assert( i < queues_.size() );
				return queues_[i];
			}
			const QueueT<T>& get_queue( size_t i ) const
			{
				assert( i < queues_.size() );
				return queues_[i];
			}
		private:
			std::vector<QueueT<T>> queues_;
		};
	}
}

#endif
