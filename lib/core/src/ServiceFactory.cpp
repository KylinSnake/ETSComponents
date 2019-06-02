#include <ServiceFactory.hpp>
#include <snake/core/Log.h>

namespace snake
{
	namespace __impl__
	{
		bool register_service(const char* name, ServiceFactory::Creator f)
		{
			if(ServiceFactory::instance().register_service(name, f))
			{
				LOG_INFO << "Success to register service: " << name << ENDLOG;
				return true;
			}
			LOG_ERROR << "Failed to register service: " << name << ENDLOG;
			return false;
		}
	}
}
