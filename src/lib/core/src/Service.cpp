#include <snake/core/Service.h>
#include <snake/core/Log.h>

namespace snake
{
	namespace core
	{
		bool Service::initialize(const std::string& name, const ConfigNode& config)
		{
			name_ = name;
			is_initialized_ = init(config);
			return is_initialized_;
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
				LOG_INFO << "Starting service " << name_ << ENDLOG;
				is_running_ = true;
				on_start();
				LOG_INFO << "Service " << name_  << " started" << ENDLOG;
			}
		}

		void Service::stop()
		{
			if(is_running_ && !is_stopping_)
			{
				before_stop();
				LOG_INFO << "Stopping service " << name_ << ENDLOG;
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
				LOG_INFO << "Service " << name_ << " stopped" << ENDLOG;
			}
		}
	}
}
