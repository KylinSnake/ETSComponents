#ifndef SNAKE_CORE_LOGGER_HPP
#define SNAKE_CORE_LOGGER_HPP

#include <string>
#include <snake/core/LogUtil.h>
#include <snake/util/BufferPool.h>
#include <snake/util/SingletonT.h>
#include <snake/concurrency/ThreadGroup.h>

namespace snake
{
	namespace core
	{
		class Logger : public snake::util::SingletonT<Logger>
		{
		public:
			Logger();
			~Logger();

			// for now, we only use filename and some fix value to initialize,
			// in future, we will use config to initialize
			void set_log_file_name(const char* filename, const char* filepath);
			void set_log_level(LogLevel l);
			bool start();
			void stop();

			LogLevel log_level() const
			{
				return level_;
			}

			LogItem alloc_log_item();
			void push( std::list<LogItem>&& list );
			void release( std::list<LogItem>&& list );

			const std::string& filename() const { return filename_; }

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

			std::unique_ptr<snake::util::BufferPool<char>> pool_ptr_;
			snake::concurrency::Mutex pool_lock_;
			std::unique_ptr<snake::concurrency::EventLoopExecutor<std::list<LogItem>>> loop_ptr_;
			size_t log_item_data_capacity_;
			bool is_started_;
			LogLevel level_;
		};
	}
}

#endif
