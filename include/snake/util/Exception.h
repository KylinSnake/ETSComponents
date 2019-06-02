#ifndef SNAKE_CORE_EXCEPTION_H
#define SNAKE_CORE_EXCEPTION_H

#include <exception>

namespace snake
{
	using Exception = std::exception;
	using InvalidArgumentException = std::invalid_argument;
	using WrongTypeException = std::bad_cast;
}

#endif
