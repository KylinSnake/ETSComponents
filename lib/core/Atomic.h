#ifndef SNAKE_CORE_ATOMIC_H
#define SNAKE_CORE_ATOMIC_H

#include <atomic>

namespace snake
{
	namespace core
	{
		template<typename T>
		using atomic = std::atomic<T>;

		class atomic_flag
		{
		public:
			atomic_flag() noexcept
				:flag_( ATOMIC_FLAG_INIT )
			{
			}
			~atomic_flag() noexcept
			{
			}
			atomic_flag( const atomic_flag& ) = delete;
			atomic_flag& operator=( const atomic_flag& ) = delete;
			bool test_and_set() noexcept
			{
				return flag_.test_and_set();
			}
			void clear() noexcept
			{
				return flag_.clear();
			}
		private:
			std::atomic_flag flag_;
		};
	}
}

#endif