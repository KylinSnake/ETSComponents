#ifndef SNAKE_CORE_THREAD_QUEUE_GROUP_H
#define SNAKE_CORE_THREAD_QUEUE_GROUP_H

#include <vector>
#include <cassert>

namespace snake
{
	namespace core
	{
		template<class T, template<class T> class ContainerT>
		class SharedQueueGroupT
		{
		public:
			SharedQueueGroupT( size_t )
			{
			}
			~SharedQueueGroupT() = default;
			SharedQueueGroupT( const SharedQueueGroupT<T, ContainerT>& ) = delete;
			SharedQueueGroupT& operator= ( const SharedQueueGroupT<T, ContainerT>& ) = delete;
			ContainerT<T>& get_queue( size_t )
			{
				return queue_;
			}
			const ContainerT<T>& get_queue( size_t ) const
			{
				return queue_;
			}
		private:
			ContainerT<T> queue_;
		};

		template<class T, template<class T> class ContainerT>
		class DedicatedQueueGroupT
		{
		public:
			DedicatedQueueGroupT( size_t t ) : queues_( t )
			{
			}
			~DedicatedQueueGroupT() = default;
			DedicatedQueueGroupT( const DedicatedQueueGroupT<T, ContainerT>& ) = delete;
			DedicatedQueueGroupT& operator= ( const DedicatedQueueGroupT<T, ContainerT>& ) = delete;
			ContainerT<T>& get_queue( size_t i )
			{
				assert( i < queues_.size() );
				return queues_[i];
			}
			const ContainerT<T>& get_queue( size_t i ) const
			{
				assert( i < queues_.size() );
				return queues_[i];
			}
		private:
			std::vector<ContainerT<T>> queues_;
		};
	}
}

#endif