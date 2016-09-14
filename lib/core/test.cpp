// This file is dummy, it is only used to test header file and template class
#include "Thread.h"
#include "Mutex.h"
#include "ConditionVariable.h"
#include "Future.h"
#include "Atomic.h"
#include "ThreadGroup.h"
#include "ThreadQueueGroup.h"
#include "ThreadDispatch.h"
#include "ThreadQueue.h"
#include <iostream>
#include <vector>

template<class T>
using LockQueueT = snake::core::QueueT<T, snake::core::mutex>;

template<class T>
using ThreadGroupT = snake::core::ThreadGroupT<T
	, snake::core::DedicatedQueueGroupT
	, std::tuple<T,snake::core::thread::id>
	, snake::core::RoundRobinDispatchT
	, LockQueueT>;

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
	std::vector<snake::core::future<std::tuple<int, snake::core::thread::id>>> vec;
	for (int i = 0; i < 1000; ++i)
	{
		vec.push_back(group.push( std::move(i) ));
	}

	for (size_t i = 0; i < vec.size(); ++i)
	{
		auto result = vec[i].get();
		std::cout << "int i = " << std::get<0>( result ) << ", thread Id = " << std::get<1>( result ) << std::endl;
	}

	return 0;
}