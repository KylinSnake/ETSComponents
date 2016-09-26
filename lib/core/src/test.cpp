// This file is dummy, it is only used to test header file and template class

#include "Future.h"
#include "ThreadGroup.h"
#include "Logger.hpp"
#include "Log.h"
#include <iostream>
#include <vector>


int main()
{
	snake::core::Logger::instance().init( "test", "/tmp", LogLevel::INFO );
	snake::core::Logger::instance().start();
	LOG_INFO << "INFO" << ENDLOG;
	LOG_DEBUG << "DEBUG" << ENDLOG;
	LOG_TRACE << "TRACE" << ENDLOG;	
	LOG_INFO << "INFO2" << ENDLOG;
	LOG_DEBUG << "DEBUG2" << ENDLOG;
	LOG_TRACE << "TRACE2" << ENDLOG;
	
	snake::core::EventLoopExecutor<int, std::tuple<int, snake::core::ThreadId>> group( [] ( int i )
	{
		LOG_INFO << "In side the thread, i = " << i<< ENDLOG;
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
				LOG_INFO << "int i = " << std::get<0>( result ) << ENDLOG;
			}
			else
			{
				LOG_ERROR<< "int i = " << i << " invalid" << ENDLOG;
			}
		}
		catch (const std::exception& e)
		{
			LOG_ERROR << "int i = " << i << " has Exception: " << e.what() << ENDLOG;
		}
	}

	snake::core::Logger::instance().stop();
	return 0;
}