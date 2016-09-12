// This file is dummy, it is only used to test header file and template class
#include "Thread.h"
#include "Mutex.h"
#include "ConditionVariable.h"
#include "Future.h"
#include "Atomic.h"
#include <iostream>

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
	snake::core::thread::id j = global::current_thread::get_id();
	std::cout << j << std::endl;
	return 0;
}