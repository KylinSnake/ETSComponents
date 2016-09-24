#ifndef SNAKE_CORE_LOGGER_HPP
#define SNAKE_CORE_LOGGER_HPP

#include <string>
#include <boost/pool/pool.hpp>
#include "LogFile.hpp"
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
			enum LogLevel
			{
				TRACE = 0,
				DEBUG = 1,
				INFO = 2,
				WARNING = 3,
				ERROR = 4
			};

			// for now, we only use filename and some fix value to initialize,
			// in future, we will use config to initialize
			bool init( const char* filename, const char* filepath, LogLevel );
			void start();
			void stop();

			struct LogItem
			{
				size_t length;
				char* data;
			};
			typedef struct
			{
				LogLevel level;
				std::list<LogItem> items;
			} LogRecord;

			LogItem alloc_log_item();
			void push( LogRecord&& item );

		protected:
			void handle_record( std::pair<pid_t, LogRecord>& );
			void close_log();
			bool open_log();
			bool is_open() const;
			bool write( const char*, size_t len );
			bool rotate_file();
			static const char * logLevel_to_string( LogLevel l );
		private:
			LogFile file_;
			size_t block_size_;
			size_t max_blocks_;
			size_t blocks_in_chunk_;
			std::string filename_;
			size_t file_seq_;
			char* start_;
			size_t avail_in_current_chunk_;

			std::unique_ptr<boost::pool<>> pool_ptr_;
			std::unique_ptr<EventLoopExecutor<std::pair<pid_t, LogRecord>>> loop_ptr_;
			size_t log_item_data_capacity_;
			bool is_started_;
			LogLevel level_;
		};
	}
}

#endif