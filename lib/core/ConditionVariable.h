#ifndef SNAKE_CORE_CONDITION_VARIABLE_H
#define SNAKE_CORE_CONDITION_VARIABLE_H

#include <condition_variable>

namespace snake
{
	namespace core
	{
		using condition_variable = std::condition_variable;
		using condition_variable_any = std::condition_variable_any;

		using cv_status = std::cv_status;
	}
}

#endif