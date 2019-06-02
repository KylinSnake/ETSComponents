#ifndef SNAKE_CORE_ATOMIC_H
#define SNAKE_CORE_ATOMIC_H

#include <atomic>

namespace snake
{
	template<typename T>
	using Atomic = std::atomic<T>;

	class AtomicFlag
	{
	public:
		constexpr AtomicFlag() noexcept = default;
		~AtomicFlag() = default;

		AtomicFlag( const AtomicFlag& ) = delete;
		AtomicFlag& operator=( const AtomicFlag& ) = delete;
		bool test_and_set() noexcept
		{
			return flag_.test_and_set();
		}
		void clear() noexcept
		{
			return flag_.clear();
		}
	private:
		std::atomic_flag flag_ {ATOMIC_FLAG_INIT};
	};
}

#endif
