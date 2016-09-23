#ifndef SNAKE_CORE_LOGFILE_HPP
#define SNAKE_CORE_LOGFILE_HPP

namespace snake
{
	namespace core
	{
		class LogFile
		{
		public:
			LogFile();
			~LogFile();
			LogFile( const LogFile& ) = delete;
			LogFile& operator=(const LogFile&) = delete;
			int open( const char* filename, size_t size_of_block, size_t init_blocks, size_t cap_allocate_blocks, size_t max_blocks );
			void close();
			bool isOpen() const;
			bool isEOF() const;
			size_t allocate( size_t n = 1 );
			bool forward_move( size_t n = 1 );
			size_t current_block() const;
			size_t size_of_block() const;
			int fd() const;
		private:
			int fd_;
			size_t size_of_block_;
			size_t current_block_;
			size_t allocated_blocks_;
			size_t cap_allocate_blocks_;
			size_t max_blocks_;
		};
	}
}

#endif
