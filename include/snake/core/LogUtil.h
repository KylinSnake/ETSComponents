#ifndef SNAKE_CORE_LOGUTIL_H
#define SNAKE_CORE_LOGUTIL_H

#include <cassert>
#include <string>

enum class LogLevel
{
	UNKNOWN = -1,
	TRACE = 0,
	DEBUG = 1,
	INFO = 2,
	WARN = 3,
	ERROR = 4,
	FATAL = 5
};

inline const char * logLevel_to_string( LogLevel l )
{
	switch (l)
	{
	case LogLevel::FATAL:
		return "FATAL";
	case LogLevel::ERROR:
		return "ERROR";
	case LogLevel::WARN:
		return "WARN";
	case LogLevel::INFO:
		return "INFO";
	case LogLevel::DEBUG:
		return "DEBUG";
	case LogLevel::TRACE:
		return "TRACE";
	default:
		assert( false );
	};
	assert( false );
	return "UNKNOWN";
}

inline LogLevel string_to_logLevel(std::string s)
{
	if (s == "FATAL")
		return LogLevel::FATAL;
	if (s == "ERROR")
		return LogLevel::ERROR;
	if (s == "WARN")
		return LogLevel::WARN;
	if (s == "INFO")
		return LogLevel::INFO;
	if (s == "DEBUG")
		return LogLevel::DEBUG;
	if (s == "TRACE")
		return LogLevel::TRACE;
	return LogLevel::UNKNOWN;
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
