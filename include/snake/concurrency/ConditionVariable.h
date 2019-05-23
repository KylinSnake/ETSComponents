#ifndef SNAKE_CORE_CONDITION_VARIABLE_H
#define SNAKE_CORE_CONDITION_VARIABLE_H

#include <condition_variable>

namespace snake
{
	namespace concurrency
	{
		using ConditionVariable = std::condition_variable;
		using ConditionVariableAny = std::condition_variable_any;

		using CvStatus = std::cv_status;
	}
}

#endif