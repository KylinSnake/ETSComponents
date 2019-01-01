#ifndef SNAKE_CORE_LOGGER_HPP
#define SNAKE_CORE_LOGGER_HPP

#include <string>
#include "LogUtil.h"
#include "BufferPool.h"
#include "SingletonT.h"
#include "ThreadGroup.h"

namespace snake
{
	namespace core
	{
		class Logger : public SingletonT<Logger>
		{
		public:
			Logger();
			~Logger();

			// for now, we only use filename and some fix value to initialize,
			// in future, we will use config to initialize
			bool init( const char* filename, const char* filepath, LogLevel );
			void start();
			void stop();

			LogLevel log_level() const
			{
				return level_;
			}

			LogItem alloc_log_item();
			void push( std::list<LogItem>&& list );
			void release( std::list<LogItem>&& list );

		protected:
			void close_log();
			bool open_log();
			size_t write( const char*, size_t len );
			bool rotate_file();
		private:
			int fd_;
			uint64_t max_size_;
			uint64_t current_size_;
			std::string filename_;
			size_t file_seq_;

			std::unique_ptr<BufferPool<char>> pool_ptr_;
			Mutex pool_lock_;
			std::unique_ptr<EventLoopExecutor<std::list<LogItem>>> loop_ptr_;
			size_t log_item_data_capacity_;
			bool is_started_;
			LogLevel level_;
		};
	}
}

#endif
