#ifndef SNAKE_CORE_LOG_H
#define SNAKE_CORE_LOG_H

#include <tuple>
#include <cstring>
#include "LogUtil.h"

class Log
{
public:
	Log( LogLevel );
	~Log();
	Log( const Log& ) = delete;
	Log& operator=( const Log& ) = delete;
	void flush();
	size_t write( const char*, size_t );
	size_t precision() const
	{
		return precision_;
	}
	void precision( size_t s )
	{
		precision_ = s;
	}
private:
	void alloc_log_item();
	void init();
	std::list<snake::core::LogItem> buffers_;
	std::list<size_t> capacities_;
	LogLevel level_;
	bool need_log_;
	size_t precision_;
};

inline Log& operator<< ( Log& l, const char c )
{
	l.write( &c, 1 );
	return l;
}

inline Log& operator<< ( Log& l, const char* s )
{
	l.write( s, strlen( s ) );
	return l;
}

inline Log& operator<< ( Log& l, const std::string& s )
{
	l.write( s.c_str(), s.length() );
	return l;
}

inline Log& operator<< ( Log& l, bool b )
{
	b ? l.write( "True", 4 ) : l.write( "False", 5 );
	return l;
}

template<typename T, typename ... Args>
inline Log& output_number( Log& l, T t, const char* fmt, Args... args)
{
	char s[sizeof( T ) * 8];
	int n = sprintf( s, fmt, t, args...);
	if (n > 0)
	{
		l.write( s, static_cast<size_t>(n) );
	}
	return l;
}

inline Log& operator<<( Log& l, int i )
{
	return output_number( l, i, "%d" );
}

inline Log& operator<<( Log& l, long i )
{
	return output_number( l, i, "%ld" );
}

inline Log& operator<<( Log& l, long long i )
{
	return output_number( l, i, "%lld" );
}

inline Log& operator<<( Log& l, unsigned i )
{
	return output_number( l, i, "%u" );
}

inline Log& operator<<( Log& l, unsigned long i )
{
	return output_number( l, i, "%lu" );
}

inline Log& operator<<( Log& l, unsigned long long i )
{
	return output_number( l, i, "%llu" );
}

inline Log& operator<<( Log& l, float i )
{
	return output_number( l, i, "%.*f", l.precision());
}

inline Log& operator<<( Log& l, double i )
{
	return output_number( l, i, "%.*f", l.precision());
}

inline Log& operator<<( Log& l, long double i )
{
	return output_number( l, i, "%.*Lf", l.precision());
}

template<size_t N>
inline Log& operator<<( Log& l, SET_PRECISION<N> )
{
	l.precision( N );
	return l;
}

inline Log& operator<<( Log& l, __LOGEND_T )
{
	l.flush();
	return l;
}

#define VAR1(XX,YY) XX##YY
#define VAR(XX, YY) VAR1(XX, YY)
#define LOG_VAR(XX) Log VAR(XX,__LINE__)(LogLevel::XX);VAR(XX,__LINE__)


#define LOG_TRACE LOG_VAR(TRACE)
#define LOG_DEBUG LOG_VAR(DEBUG)
#define LOG_INFO LOG_VAR(INFO)
#define LOG_WARN LOG_VAR(WARN)
#define LOG_ERROR LOG_VAR(ERROR)

#define ENDLOG " (" << std::string(__FILE__).substr(std::string(__FILE__).rfind('/') + 1) <<", Line: " << __LINE__ << ")" <<  __LOGEND_T();

#endif