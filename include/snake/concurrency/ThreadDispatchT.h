#ifndef SNAKE_CORE_THREAD_DISPATCH_T_H
#define SNAKE_CORE_THREAD_DISPATCH_T_H

#include "Atomic.h"

namespace snake
{
	template<class T>
	class RoundRobinDispatchT
	{
	public:
		RoundRobinDispatchT(size_t N) : num_(), N_(N)
		{
			num_.store( 0 );
		}
		~RoundRobinDispatchT() = default;
		RoundRobinDispatchT( const RoundRobinDispatchT & ) = delete;
		RoundRobinDispatchT& operator=( const RoundRobinDispatchT& ) = delete;
		size_t next(const T&)
		{
			return N_ == 1 ? 0 : (num_++) % N_;
		}
	private:
		Atomic<unsigned long> num_;
		size_t N_;
	};
}


#endif
