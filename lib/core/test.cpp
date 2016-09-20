// This file is dummy, it is only used to test header file and template class
#include "Thread.h"
#include "Mutex.h"
#include "ConditionVariable.h"
#include "Future.h"
#include "Atomic.h"
#include "ThreadGroupT.h"
#include "ThreadQueueGroupT.h"
#include "ThreadDispatchT.h"
#include "ThreadMessageQueueT.h"
#include "ThreadMessageContainerT.h"
#include <iostream>
#include <vector>

template<typename T>
using LockableQueue = snake::core::ThreadMessageQueueT<T, snake::core::QueueT, snake::core::exit_immediately>;

template<class T>
using ThreadGroupT = snake::core::ThreadGroupT<T
	, std::tuple<T, snake::core::thread::id>
	, snake::core::DedicatedQueueGroupT
	, snake::core::RoundRobinDispatchT
	, LockableQueue>;

	void test()
{

	snake::core::thread t;
	global::current_thread::get_id();
	snake::core::mutex m1;
	snake::core::recurisve_mutex m2;
	snake::core::lock_guard<snake::core::mutex> l1(m1);
	snake::core::unique_lock<snake::core::recurisve_mutex> l2( m2 );
	snake::core::condition_variable v1;
	snake::core::condition_variable_any v2;
	snake::core::future<int> f1;
	snake::core::packaged_task<int> f2;
	snake::core::atomic<int> a1;
	snake::core::atomic<bool> a2;
	snake::core::atomic_flag af;
	if (af.test_and_set()) af.clear();

}

int main()
{
	std::cout << global::current_thread::get_id() << std::endl;

	ThreadGroupT<int> group( [] ( int i )
	{
		return std::make_tuple(i, global::current_thread::get_id());
	}, 4 );
	group.start();
	global::current_thread::sleep_for( std::chrono::seconds( 1 ) );
	std::vector<snake::core::future<std::tuple<int, snake::core::thread::id>>> vec;
	for (int i = 0; i < 1000; ++i)
	{
		auto f = group.push( std::move( i ) );
		vec.push_back(std::move(f));
		if (i == 800)
		{
			group.stop();
		}
	}

	for (size_t i = 0; i < vec.size(); ++i)
	{
		try
		{
			if (vec[i].valid() && vec[i].wait_for( std::chrono::seconds( 1 ) ) == snake::core::future_status::ready)
			{
				auto result = vec[i].get();
				std::cout << "int i = " << std::get<0>( result ) << ", thread Id = " << std::get<1>( result ) << std::endl;
			}
			else
			{
				std::cout << "int i = " << i << " invalid" << std::endl;
			}
		}
		catch (const std::exception& e)
		{
			std::cout << "int i = " << i << " has Exception: " << e.what() << std::endl;
		}
	}

	return 0;
}