#ifndef SNAKE_MAIN_H
#define SNAKE_MAIN_H

#include <snake/core/Service.h>

namespace snake
{
	class Main final
	{
	public:
		static int main(int argc, char* argv[]);
		template<class T>
		static ServicePointerT<T> get_service(const std::string& name);
		static void shutdown();
	private:
		static ServicePointer get_service_ptr(const std::string& name);
	};

	template<class T>
	ServicePointerT<T> Main::get_service(const std::string& name)
	{
		auto ptr = get_service_ptr(name);
		if (ptr)
		{
			return std::dynamic_pointer_cast<T>(ptr);
		}
		return ServicePointerT<T>();
	}
}

#endif
