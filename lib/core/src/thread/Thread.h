#ifndef SNAKE_CORE_THREAD_H
#define SNAKE_CORE_THREAD_H

#include <thread>

namespace snake
{
	namespace core
	{
		using Thread = std::thread;
		using ThreadId = std::thread::id;
	}
}

namespace global
{
	namespace current_thread
	{
		inline snake::core::ThreadId get_id() noexcept
		{
			return std::this_thread::get_id();
		}
		
		inline void yield() noexcept
		{
			return std::this_thread::yield();
		}
		
		template<typename R, typename P>
			inline void sleep_for( const std::chrono::duration<R, P>& rtime )
			{
				return std::this_thread::sleep_for( rtime );
			}
		
		template<typename C, typename P>
			inline void sleep_until( const std::chrono::time_point<C, P>& atime )
			{
				return std::this_thread::sleep_until( atime );
			}
	}
}

#endif
