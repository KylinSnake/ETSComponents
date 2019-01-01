#ifndef SNAKE_CORE_LOGUTIL_H
#define SNAKE_CORE_LOGUTIL_H

#include <cassert>

enum class LogLevel
{
	TRACE = 0,
	DEBUG = 1,
	INFO = 2,
	WARN = 3,
	ERROR = 4
};

inline const char * logLevel_to_string( LogLevel l )
{
	switch (l)
	{
	case LogLevel::ERROR:
		return "Error";
	case LogLevel::WARN:
		return "Warning";
	case LogLevel::INFO:
		return "Info";
	case LogLevel::DEBUG:
		return "Debug";
	case LogLevel::TRACE:
		return "Trace";
	default:
		assert( false );
	};
	assert( false );
	return "UNKNOWN";
}

struct __LOGEND_T
{
};

template<size_t N>
struct SET_PRECISION
{
};

namespace snake
{
	namespace core
	{
		struct LogItem
		{
			size_t length;
			char* data;
		};
	}
}

#endif
