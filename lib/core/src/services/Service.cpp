#include <Service.h>

namespace snake
{
	namespace core
	{
		bool Service::initialize(const ConfigNode& config)
		{
			if (config.has_child("name"))
			{
				name_ = config.get_string_value("name");
				return init(config);
			}
			return false;
		}

		bool Service::init(const ConfigNode& config)
		{
			return reload(config);
		}

		void Service::start()
		{
			if (!(is_running_ || is_stopping_))
			{
				before_start();
				is_running_ = true;
				on_start();
			}
		}

		void Service::stop()
		{
			if(is_running_ && !is_stopping_)
			{
				before_stop();
				is_running_ = false;
				is_stopping_ = true;
				on_stop();
			}
		}

		void Service::join()
		{
			if(!is_running_ && is_stopping_)
			{
				on_join();
				is_stopping_ = false;
				on_stopped();
			}
		}
	}
}
