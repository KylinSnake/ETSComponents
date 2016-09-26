#include <sys/types.h>
#include <sys/stat.h>
#include <sys/time.h>
#include <unistd.h>
#include <fcntl.h>
#include <cassert>
#include <ctime>
#include <cstring>
#include "Logger.hpp"

constexpr size_t ONE_MEGA = 1024 * 1024;
constexpr size_t MAX_FILE_SIZE = 10 * 1024 * ONE_MEGA; // 10G

namespace snake
{
	namespace core
	{
		Logger::Logger()
			: fd_( -1 )
			, max_size_( 0 )
			, current_size_( 0 )
			, filename_()
			, file_seq_( 0 )
			, pool_ptr_( nullptr )
			, pool_lock_()
			, loop_ptr_( nullptr )
			, log_item_data_capacity_( 0 )
			, is_started_( false )
			, level_( LogLevel::INFO )
		{
		}

		Logger::~Logger()
		{
		}

		bool Logger::init( const char* filename, const char* filepath, LogLevel l )
		{
			file_seq_ = 0;
			filename_ = filename;
			max_size_ = MAX_FILE_SIZE;
			if (filepath != nullptr)
			{
				filename_ = std::string( filepath ) + "/" + filename_;
			}

			log_item_data_capacity_ = 128;
			level_ = l;
			pool_ptr_.reset( new boost::pool<>( log_item_data_capacity_ ) );
			loop_ptr_.reset( new EventLoopExecutor<std::list<LogItem>>( [this] ( std::list<LogItem>& r )
			{
				char time[128];
				struct timeval tv;
				gettimeofday( &tv, NULL );
				size_t n = std::strftime( time, sizeof( time ), "%Y-%m-%d %H:%M:%S", std::localtime( &tv.tv_sec ) );
				n += std::sprintf( time + n, ".%09ld ", tv.tv_usec );
				write( time, n );
				for (auto& i : r)
				{
					write( i.data, i.length );
				}
				release( std::move( r ) );
			} ) );
			return true;
		}

		void Logger::start()
		{
			if (!is_started_)
			{
				open_log();
				assert( loop_ptr_ != nullptr && pool_ptr_ != nullptr );
				loop_ptr_->start();
				is_started_ = true;
			}
		}

		void Logger::stop()
		{
			if (is_started_)
			{
				loop_ptr_->stop();
				close_log();
				is_started_ = false;
			}
		}

		void Logger::close_log()
		{
			if (fd_ >= 0)
			{
				::close( fd_ );
				fd_ = -1;
			}
		}

		LogItem Logger::alloc_log_item()
		{
			assert( pool_ptr_ != nullptr );
			LogItem ret;
			{
				UniqueLock<Mutex> l( pool_lock_ );
				ret.data = static_cast<char*>(pool_ptr_->malloc());
			}
			ret.length = log_item_data_capacity_;
			return ret;
		}

		void Logger::push( std::list<LogItem>&& list )
		{
			loop_ptr_->push( std::move( list ) );
		}

		void Logger::release( std::list<LogItem>&& list )
		{
			UniqueLock<Mutex> l( pool_lock_ );
			for (auto& i : list)
			{
				pool_ptr_->free( i.data );
			}
		}

		bool Logger::open_log()
		{
			assert( fd_ == -1 );
			auto t = std::time( nullptr );
			char time[128];
			size_t n = std::strftime( time, sizeof( time ), "_%Y%m%dT%H%M%S", std::localtime( &t ) );
			sprintf( time + n, "_%u.log", static_cast<unsigned int>(file_seq_) );
			auto name = filename_ + time;
			if ((fd_ = ::open( name.c_str(), O_RDWR | O_CREAT, mode_t( 0644 ) )) >= 0)
			{
				current_size_ = 0;
				file_seq_++;
				return true;
			}
			return false;
		}

		bool Logger::rotate_file()
		{
			close_log();
			return open_log();
		}

		size_t Logger::write( const char* str, size_t len )
		{
			assert( fd_ >= 0 );
			auto ret = ::write( fd_, str, len );
			current_size_ += ret;
			if (current_size_ > max_size_)
			{
				rotate_file();
			}
			return ret;
		}
	}
}