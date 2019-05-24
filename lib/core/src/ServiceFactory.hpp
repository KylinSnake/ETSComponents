#ifndef SNAKE_CORE_SERVICE_FACTORY_HPP
#define SNAKE_CORE_SERVICE_FACTORY_HPP

#include <map>
#include <string>
#include <type_traits>

#include <snake/util/SingletonT.h>
#include <snake/core/Service.h>

namespace snake
{
	class ServiceFactory final : public SingletonT<ServiceFactory>
	{
	public:
		ServiceFactory() = default;
		~ServiceFactory() = default;

		using Creator = std::add_pointer<ServicePointer(void)>::type; 

		bool register_service(const char* name, Creator fun)
		{
			if (creator_map.count(name) == 0)
			{
				creator_map[name] = fun;
				return true;
			}
			return false;
		}

		ServicePointer create_service(const std::string& name)
		{
			if (creator_map.count(name) > 0)
			{
				return creator_map[name]();
			}
			return ServicePointer{};
		}

	private:
		std::map<std::string, Creator> creator_map{};
	};
}

#endif
