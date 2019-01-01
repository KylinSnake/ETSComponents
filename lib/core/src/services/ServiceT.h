#include <Service.h>
#include <ServiceFactory.h>

#define REGISTER_SERVICE(X) { bool registered_##X = X::register_service(); }

namespace snake
{
	namespace core
	{
		template<class T>
		class ServiceT : public Service
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
				return ServiceFactory::instance().register_service(T::type_name(), T::create_instance);
			}
		}
	}
}
