#ifndef SNAKE_DB_DBEXCEPTION_H
#define SNAKE_DB_DBEXCEPTION_H

#include <string>
#include <snake/util/Exception.h>

namespace snake
{
	class DBException : public Exception
	{
	public:
		DBException(const std::string& db_name, const std::string& error) 
			: what_arg("Error on " + db_name + " : " +  error)
		{
		}
		DBException(const DBException&) = default;
		DBException& operator=(const DBException&) = default;
		virtual const char* what() const noexcept override { return what_arg.data(); }
	private:
		std::string what_arg;
	};
}

#endif
