#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <cassert>
#include <algorithm>
#include "logFile.hpp"

namespace snake
{
	namespace core
	{
		LogFile::LogFile()
			: fd_( -1 )
			, size_of_block_( 0 )
			, current_block_( 0 )
			, allocated_blocks_( 0 )
			, cap_allocate_blocks_(0)
			, max_blocks_( 0 )
		{
		}

		LogFile::~LogFile()
		{
			if (isOpen())
			{
				close();
			}
		}

		int LogFile::open( const char* filename, size_t size_of_block,
			size_t init_blocks, size_t cap_allocate_blocks, size_t max_blocks )
		{
			assert( cap_allocate_blocks <= max_blocks && init_blocks > 0 && init_blocks <= cap_allocate_blocks);
			fd_ = ::open( filename, O_WRONLY | O_CREAT );
			if (fd_ > 0)
			{
				size_of_block_ = size_of_block;
				max_blocks_ = max_blocks;
				cap_allocate_blocks_ = cap_allocate_blocks;
				current_block_ = 0;
				allocated_blocks_ = 0;
				allocate( init_blocks );
			}
			return fd_;
		}

		void LogFile::close()
		{
			::close( fd_ );
			fd_ = -1;
			size_of_block_ = 0;
			current_block_ = 0;
			allocated_blocks_ = 0;
			cap_allocate_blocks_ = 0;
			max_blocks_ = 0;
		}

		bool LogFile::isOpen() const
		{
			return fd_ >= 0;
		}

		bool LogFile::isEOF() const
		{
			return allocated_blocks_ == max_blocks_;
		}

		size_t LogFile::allocate( size_t n )
		{
			assert( isOpen() );
			size_t allocated = std::min( n, allocated_blocks_ - current_block_);
			if (allocated < n)
			{
				size_t capacity = std::min(std::max(allocated_blocks_, n - allocated), 
					std::min( cap_allocate_blocks_, max_blocks_ - allocated_blocks_ ));
				if (capacity > 0)
				{
					lseek( fd_, size_of_block_ * capacity - 1, SEEK_END );
					write( fd_, '\0', 1);
					allocated_blocks_ += capacity;
					allocated = std::min( allocated + capacity, n );
				}
			}
			return allocated;
		}

		bool LogFile::forward_move( size_t n )
		{
			if (current_block_ + n <= allocated_blocks_)
			{
				current_block_ += n;
				return true;
			}
			return false;
		}

		size_t LogFile::current_block() const
		{
			return current_block_;
		}

		size_t LogFile::size_of_block() const
		{
			return size_of_block_;
		}

		int LogFile::fd() const
		{
			return fd_;
		}
	}
}