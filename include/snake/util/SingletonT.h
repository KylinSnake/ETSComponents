#ifndef SNAKE_CORE_SINGLETON_T_H
#define SNAKE_CORE_SINGLETON_T_H

namespace snake
{
	template<typename T>
	class SingletonT
	{
	public:
		SingletonT( const SingletonT& ) = delete;
		SingletonT& operator==( const SingletonT& ) = delete;
		SingletonT( SingletonT&& ) = delete;
		SingletonT& operator==( SingletonT&& ) = delete;
		~SingletonT() = default;
		
		static T& instance()
		{
			static T t;
			return t;
		}
	protected:
		SingletonT() = default;

	};
}

#endif
