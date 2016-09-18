#ifndef SNAKE_CORE_THREAD_MESSAGE_CONTAINER_T_H
#define SNAKE_CORE_THREAD_MESSAGE_CONTAINER_T_H

#include <list>

namespace snake
{
	namespace core
	{
		template<typename T>
		class QueueT
		{
		public:
			QueueT() = default;
			~QueueT() = default;
			QueueT( const QueueT& ) = delete;
			QueueT& operator =( const QueueT& ) = delete;
			using CollectionType = std::list<T>;

			void push( T&& t )
			{
				list_.push_back( std::move( t ) );
			}
			void pop()
			{
				list_.pop_front();
			}
			T& top()
			{
				return list_.front();
			}
			bool empty() const
			{
				return list_.empty();
			}
		private:
			std::list<T> list_;
		};
	}
}

#endif
