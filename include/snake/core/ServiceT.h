#ifndef SNAKE_CORE_SERVICET_H
#define SNAKE_CORE_SERVICET_H

#include <snake/core/Service.h>
#include <memory>

#define REGISTER_SERVICE(X)  bool registered_##X = X::register_service(); 

namespace snake
{
	namespace core
	{
		namespace __impl__
		{
			bool register_service(const char* name, std::add_pointer<ServicePointer(void)>::type); 
		}

		template<class T>
		class ServiceT : public Service, public std::enable_shared_from_this<T>
		{
		public:
			ServiceT() = default;
			virtual ~ServiceT() = default;
			static const char* type_name();
			static ServicePointer create_instance()
			{
				return std::make_shared<T>(); 
			}
			static bool register_service()
			{
				return __impl__::register_service(T::type_name(), T::create_instance);
			}
			ServicePointerT<T> This() const
			{
				return this->shared_from_this();
			}
		};
	}
}
#endif
