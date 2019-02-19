#include <dlfcn.h>

#include <iostream>

#include <Application.hpp>
#include <Logger.hpp>
#include <ServiceFactory.hpp>
#include <Log.h>
#include <Exception.h>

namespace snake
{
	namespace core
	{
		Application::~Application()
		{
			cleanup();
		}

		bool Application::load_library(const std::string& name)
		{
			if(loaded_libs_.count(name) == 0)
			{
				auto full_name = "lib" + name + ".so";
				auto ret = dlopen(full_name.c_str(), RTLD_NOW | RTLD_GLOBAL);
				if (ret == nullptr)
				{
					LOG_FATAL << "Failed to load library " << name <<", [error = " << dlerror() << ENDLOG;
					return false;
				}
				else
				{
					LOG_TRACE << "Loaded library: " << name << ENDLOG;
				}
				loaded_libs_[name] = ret;
			}
			return true;
		}

		void Application::unload_all_libraries()
		{
			for(auto& it : loaded_libs_)
			{
				if(dlclose(it.second))
				{
					LOG_ERROR << "Failed to unload library " << it.first <<", [error = " << dlerror() << ENDLOG;
				}
				else
				{
					LOG_TRACE << "Success to unload library " << it.first << ENDLOG;
				}
			}
			loaded_libs_.clear();
		}

		bool Application::load_config_file(const std::string& path)
		{
			try
			{
				config_.init_from_file(path);
				return true;
			}
			catch(Exception& e)
			{
				if (log_initialized_)
				{
					LOG_FATAL << "Failed load Config from file [" << path << "], [error = " << e.what() << ENDLOG;
				}
				else
				{
					std::cerr << "Failed load Config from file [" << path << "], [error = " << e.what() << std::endl;
				}
			}
			return false;
		}

		bool Application::initialize_log(const ConfigNode& node)
		{
			auto path = node.get_string_with_default("Path", "./");
			LogLevel log_level = LogLevel::INFO;

			if (node.has_child("LogLevel"))
			{
				auto log_level_string = node.get_string_with_default("LogLevel", "INFO");
				log_level = string_to_logLevel(log_level_string);
				if (log_level == LogLevel::UNKNOWN)
				{
					std::cerr << "Wrong log level defined: " << log_level_string << std::endl;
					return false;
				}
			}

			if(node.has_child("LogFile"))
			{
				log_file_name_ = node.get_string_value("LogFile");
				Logger::instance().set_log_file_name(log_file_name_.c_str(), path.c_str());
			}
			

			Logger::instance().set_log_level(LogLevel::TRACE);
			if (Logger::instance().start())
			{
				LOG_TRACE << "Config: " << node << ENDLOG;
				LOG_TRACE << "LogLevel: " << logLevel_to_string(log_level) << ENDLOG;
				Logger::instance().set_log_level(log_level);
				log_initialized_ = true;
			}
			else
			{
				std::cerr << "Failed to open the log file: " << Logger::instance().filename() << std::endl;
			}

			return log_initialized_;
		}

		bool Application::initialize_services(const std::list<ConfigNode>& nodes)
		{
			std::map<std::string, ConfigNode> configs;
			for (auto& node : nodes)
			{
				if (node.has_child("lib"))
				{
					for (auto name : node.get_child("lib").get_string_list_of_children())
					{
						if(!load_library(name))
						{
							return false;
						}
					}
				}
			}
			for (auto& node : nodes)
			{
				if (!node.has_child("name"))
				{
					LOG_FATAL << "No Service Name"<< ENDLOG;
					return false;
				}
				auto name = node.get_string_value("name");
				if (services_map_.count(name) > 0)
				{
					LOG_FATAL << "Duplicated Service: " << name << ENDLOG;
					return false;
				}
				auto type = name;
				if(node.has_child("type"))
				{
					type = node.get_string_value("type");
				}
				auto ptr = ServiceFactory::instance().create_service(type);
				if (ptr == nullptr)
				{
					LOG_FATAL << "No Service " << type << " registered" << ENDLOG;
					return false;
				}

				services_map_[name] = ptr;
				configs[name] = node;
				LOG_INFO << "Success to create service " << name << ENDLOG;
			}

			for (auto& pair : services_map_)
			{
				assert(configs.count(pair.first) > 0);
				ConfigNode node{};
				auto& root = configs[pair.first];
				if (root.has_child("config"))
				{
					node = root.get_child("config");
				}
				if (pair.second->initialize(pair.first, node))
				{
					LOG_INFO << "Success to initialize the service: " << pair.first << ENDLOG;
				}
				else
				{
					LOG_FATAL << "Failed to initialize the service: " << pair.first << ENDLOG;
					return false;
				}
			}
			return true;
		}

		void Application::start_services()
		{
			LOG_INFO << "Starting all services now" << ENDLOG;
			for (auto& pair : services_map_)
			{
				pair.second->start();
			}
		}

		void Application::stop_services()
		{
			LOG_INFO << "Stopping all services now" << ENDLOG;
			for(auto& pair: services_map_)
			{
				pair.second->stop();
			}
		}

		void Application::join_services()
		{
			LOG_INFO << "Joining all services now" << ENDLOG;
			for(auto& pair: services_map_)
			{
				pair.second->join();
			}
		}

		void Application::cleanup()
		{
			services_map_.clear();
			unload_all_libraries();
			Logger::instance().stop();
		}

		ConfigNode Application::root() const
		{
			return config_.root();
		}

		void Application::run()
		{
			UniqueLock<Mutex> lk(mutex_);
			is_running_ = true;
			cv_.wait(lk, [this]{return !this->is_running_;});
		}

		void Application::shutdown()
		{
			{
				UniqueLock<Mutex> lk(mutex_);
				is_running_ = false;
			}
			cv_.notify_one();
		}

		ServicePointer Application::get_service_pointer(const std::string& name) const
		{
			auto it = services_map_.find(name);
			if(it != services_map_.end())
			{
				return it->second;
			}
			return ServicePointer();
		}
	}
}
