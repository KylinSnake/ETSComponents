#ifndef SNAKE_CORE_LOGGER_HPP
#define SNAKE_CORE_LOGGER_HPP

#include "SingletonT.h"

namespace snake
{
	namespace core
	{
		class Logger : public SingletonT<Logger>
		{
		public:
			Logger() = default;
			~Logger() = default;
			bool init(const char* filename, const char* filepath)
		};
	}
}

#endif