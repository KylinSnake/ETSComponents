#include "Logger.hpp"
#include <sys/mman.h>
#include <sys/types.h>
#include <unistd.h>
#include <sys/time.h>
#include <cassert>
#include <ctime>
#include <cstring>

constexpr size_t ONE_MEGA = 1024 * 1024; // 1M
constexpr size_t MAX_FILE_SIZE = 10 * 1024 * ONE_MEGA; // 10G

namespace snake
{
	namespace core
	{
		const char * Logger::logLevel_to_string( Logger::LogLevel l )
		{
			switch (l)
			{
			case ERROR:
				return "Error";
			case WARNING:
				return "Warning";
			case INFO:
				return "Info";
			case DEBUG:
				return "Debug";
			case TRACE:
				return "Trace";
			default:
				assert( false );
			};
			assert( false );
			return "UNKNOWN";
		}
		Logger::Logger()
			:file_()
			,block_size_(0)
			,max_blocks_(0)
			,blocks_in_chunk_(0)
			,filename_()
			,file_seq_(0)
			,start_(nullptr)
			,avail_in_current_chunk_(0)
			,pool_ptr_(nullptr)
			,loop_ptr_(nullptr)
			,log_item_data_capacity_(0)
			,is_started_(false)
			,level_(INFO)
		{
		}

		Logger::~Logger()
		{
		}

		bool Logger::init( const char* filename, const char* filepath, LogLevel l)
		{
			block_size_ = ::sysconf(_SC_PAGESIZE);
			max_blocks_ = MAX_FILE_SIZE / block_size_;
			blocks_in_chunk_ = 4;
			file_seq_ = 0;
			filename_ = filename;
			if (filepath != nullptr)
			{
				filename_ = std::string( filepath ) + filename_;
			}
			start_ = nullptr;
			avail_in_current_chunk_ = 0;

			log_item_data_capacity_ = 128;
			level_ = l;
			pool_ptr_.reset( new boost::pool<>( log_item_data_capacity_ ) );
			loop_ptr_.reset( new EventLoopExecutor<std::pair<pid_t, LogRecord>>( [this] ( std::pair<pid_t, LogRecord>& r )
			{
				this->handle_record( r );
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
			if (start_ != nullptr)
			{
				::munmap( start_, blocks_in_chunk_ * block_size_ );
				start_ = nullptr;
				avail_in_current_chunk_ = 0;
			}
			if (file_.is_open())
			{
				file_.close();
			}
		}

		Logger::LogItem Logger::alloc_log_item()
		{
			assert( pool_ptr_ != nullptr );
			LogItem ret;
			ret.data = static_cast<char*>(pool_ptr_->malloc());
			ret.length = log_item_data_capacity_;
			return ret;
		}
		void Logger::push( LogRecord&& item )
		{
			loop_ptr_->push( std::make_pair( pthread_self(), std::move( item ) ) );
		}

		void Logger::handle_record( std::pair<pid_t, LogRecord>& t )
		{
			LogRecord& item = t.second;
			if (item.level > level_)
			{
				char time[128];
				struct timeval tv;
				gettimeofday( &tv, NULL );
				size_t n = std::strftime( time, sizeof( time ), "%Y-%m-%d %H:%M:%S", std::localtime( &tv.tv_sec ) );
				n += std::sprintf( time + n, ".%09ld | %ld | %s |", tv.tv_usec, static_cast<long>(t.first), logLevel_to_string(item.level));
				write( time, n );
				for (auto& i : item.items)
				{
					write( i.data, i.length );
				}
			}
			for (auto& i : item.items)
			{
				pool_ptr_->free( i.data );
			}
		}

		bool Logger::is_open() const
		{
			return file_.is_open() && start_ != nullptr;
		}

		bool Logger::open_log()
		{
			if (is_open())
			{
				return false;
			}
			assert( start_ == nullptr );
			auto t = std::time( nullptr );
			char time[128];
			size_t n = std::strftime( time, sizeof( time ), "%Y%m%dT%H%M%S", std::localtime( &t ) );
			sprintf( time + n, "_%u.log", static_cast<unsigned int>(file_seq_) );
			auto name = filename_ + time;
			if (file_.open( name.c_str(), block_size_, blocks_in_chunk_, max_blocks_ ) >= 0)
			{
				start_ = (char*)::mmap( nullptr, blocks_in_chunk_ * block_size_, 
					PROT_WRITE, MAP_SHARED, file_.fd(), 0 );
				avail_in_current_chunk_ = blocks_in_chunk_ * block_size_;
				++file_seq_;

				assert( is_open() );
				return true;
			}
			return false;
		}

		bool Logger::rotate_file()
		{
			if (is_open())
			{
				close_log();
			}
			return open_log();
		}

		bool Logger::write( const char* str, size_t len )
		{
			assert( start_ != nullptr );
			size_t n = std::min( len, avail_in_current_chunk_ );
			if (n > 0)
			{
				std::memcpy( start_ + (blocks_in_chunk_ * block_size_) - avail_in_current_chunk_, str, n );
				str += n;
				avail_in_current_chunk_ -= n;
				len -= n;
			}
			if (len > 0)
			{
				assert( avail_in_current_chunk_ == 0 );
				assert( file_.current_block() + blocks_in_chunk_ <= file_.allocated_blocks() );

				::munmap( start_, blocks_in_chunk_ * block_size_ );

				file_.forward_move( blocks_in_chunk_ );
				file_.allocate( blocks_in_chunk_ );

				start_ = (char*)::mmap( nullptr, blocks_in_chunk_ * block_size_,
					PROT_WRITE, MAP_SHARED, file_.fd(), file_.current_block() * block_size_ );
				avail_in_current_chunk_ = blocks_in_chunk_ * block_size_;

				return write( str, len );
			}
			if (file_.allocated_blocks() >= file_.max_blocks())
			{
				 return rotate_file();
			}
			return true;
		}
	}
}