#ifndef SNAKE_MAIN_H
#define SNAKE_MAIN_H

#include <Service.h>

namespace snake
{
	class Main final
	{
	public:
		static int main(int argc, char* argv[]);
		template<class T>
		static core::ServicePointerT<T> get_service(const std::string& name);
		static void shutdown();
	private:
		static core::ServicePointer get_service_ptr(const std::string& name);
	};

	template<class T>
	core::ServicePointerT<T> Main::get_service(const std::string& name)
	{
		auto ptr = get_service_ptr(name);
		if (ptr)
		{
			return std::dynamic_pointer_cast<T>(ptr);
		}
		return core::ServicePointerT<T>();
	}
}

#endif
