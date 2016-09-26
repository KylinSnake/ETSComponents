#include <cstring>
#include "Logger.hpp"
#include "Log.h"
using namespace snake::core;

void Log::alloc_log_item()
{
	assert( buffers_.size() == capacities_.size() );
	auto ret = Logger::instance().alloc_log_item();
	capacities_.push_back( ret.length );
	ret.length = 0;
	buffers_.push_back( ret );
}

Log::Log( LogLevel l )
	: buffers_()
	, capacities_()
	, level_( l )
	, need_log_( l >= Logger::instance().log_level() )
	, precision_( 2 )
{
}

Log::~Log()
{
	if (buffers_.size() > 0 && buffers_.front().length > 0)
	{
		flush();
	}
	if (buffers_.size() > 0)
	{
		Logger::instance().release( std::move( buffers_ ) );
	}
}

size_t Log::write( const char* p, size_t len )
{
	if (need_log_)
	{
		assert( buffers_.size() == capacities_.size() );
		if (buffers_.empty())
		{
			alloc_log_item();
			buffers_.back().length = std::sprintf( buffers_.back().data, "| %lu | %s | ", global::current_thread::get_id(), logLevel_to_string( level_ ) );
		}
		LogItem& item = buffers_.back();
		size_t capacity = capacities_.back();
		size_t n = std::min( len, capacity - item.length );
		if (n > 0)
		{
			std::memcpy( item.data + item.length, p, n );
			len -= n;
			item.length += n;
			p += n;
		}
		if (len > 0)
		{
			alloc_log_item();
			n += write( p, len );
		}
		return n;
	}
	return 0;
}

void Log::flush()
{
	if (need_log_)
	{
		write( "\n", 1 );
		Logger::instance().push( std::move( buffers_ ) );
		buffers_.clear();
		capacities_.clear();
	}
}