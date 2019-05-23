#include <sys/types.h>
#include <sys/stat.h>
#include <sys/time.h>
#include <unistd.h>
#include <stdio.h>
#include <fcntl.h>
#include <cassert>
#include <ctime>
#include <cstring>
#include <Logger.hpp>
#include <snake/util/DateTime.h>
#include <snake/util/String.h>

constexpr size_t ONE_MEGA = 1024 * 1024;
constexpr size_t MAX_FILE_SIZE = 10 * 1024 * ONE_MEGA; // 10G

using namespace snake::util;
using namespace snake::concurrency;

namespace snake
{
	namespace core
	{
		Logger::Logger()
			: fd_( fileno(stdout) )
			, max_size_( MAX_FILE_SIZE )
			, current_size_( 0 )
			, filename_()
			, pool_ptr_( nullptr )
			, pool_lock_()
			, loop_ptr_( nullptr )
			, log_item_data_capacity_( 128 )
			, is_started_( false )
			, level_( LogLevel::INFO )
		{
			pool_ptr_.reset( new BufferPool<char>( log_item_data_capacity_ ) );
			loop_ptr_.reset( new EventLoopExecutor<std::list<LogItem>>( [this] ( std::list<LogItem>& r )
			{
				char buf[64]{0};
				auto size = DateTime::format(buf, sizeof(buf), "%Y%m%d %H:%M:%S", DateTime::now(), 6);
				write(buf, size);
				for (auto& i : r)
				{
					write( i.data, i.length );
				}
				release( std::move( r ) );
			} ) );
		}

		Logger::~Logger()
		{
		}

		void Logger::set_log_file_name(const char* filename, const char* filepath)
		{
			filename_ = filename;
			trim(filename_);	
			if (filepath != nullptr && filename_.size() > 0)
			{
				filename_ = std::string( filepath ) + "/" + filename_;
			}
		}

		void Logger::set_log_level(LogLevel l)
		{
			level_ = l;
		}

		bool Logger::start()
		{
			if (!is_started_)
			{
				is_started_ = open_log();
				if(is_started_)
				{
					assert( loop_ptr_ != nullptr && pool_ptr_ != nullptr );
					loop_ptr_->start();
				}
			}
			return is_started_;
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
			auto fd = fileno(stdout);
			if(fd_ != fd && fd_ > 0)
			{
				::close( fd_ );
				fd_ = fd;
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
			assert(fd_ == fileno(stdout));
			if (filename_.size() == 0)
			{
				current_size_ = 0;
				return true;
			}
			auto t = std::time( nullptr );
			char time[128];
			std::strftime( time, sizeof( time ), "_%Y%m%dT%H%M%S", std::localtime( &t ) );
			auto name = filename_ + time + ".log";
			auto fd = fd_;
			if ((fd = ::open( name.c_str(), O_RDWR | O_CREAT, mode_t( 0644 ) )) >= 0)
			{
				fd_ = fd;
				current_size_ = 0;
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
			assert( fd_ > 0 );
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
