#ifndef SNAKE_CORE_SERVICE_FACTORY_H
#define SNAKE_CORE_SERVICE_FACTORY_H

#include <map>
#include <string>
#include <type_traits>

#include <SingletonT.h>
#include <Service.h>

namespace snake
{
	namespace core
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
}

#endif
