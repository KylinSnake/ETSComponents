// This file is dummy, it is only used to test header file and template class

#include "Future.h"
#include "ThreadGroup.h"
#include <iostream>
#include <vector>


int main()
{
	std::cout << global::current_thread::get_id() << std::endl;

	snake::core::EventLoopExecutor<int, std::tuple<int, snake::core::ThreadId>> group( [] ( int i )
	{
		return std::make_tuple(i, global::current_thread::get_id());
	}, 4 );
	group.start();
	global::current_thread::sleep_for( std::chrono::seconds( 1 ) );
	std::vector<snake::core::Future<std::tuple<int, snake::core::ThreadId>>> vec;
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
			if (vec[i].valid() && vec[i].wait_for( std::chrono::seconds( 1 ) ) == snake::core::FutureStatus::ready)
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