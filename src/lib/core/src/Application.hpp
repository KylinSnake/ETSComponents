#ifndef SNAKE_CORE_APPLICATION_HPP
#define SNAKE_CORE_APPLICATION_HPP

#include <map>
#include <memory>

#include <snake/core/Service.h>
#include <snake/core/Config.h>
#include <snake/concurrency/ConditionVariable.h>
#include <snake/concurrency/Mutex.h>

namespace snake
{
	namespace core
	{
		class Application final
		{
		public:
			Application() = default;
			~Application();
			
			ServicePointer get_service_pointer(const std::string& name) const;

			void shutdown();

			ConfigNode root() const;

			bool load_config_file(const std::string& path);
			bool initialize_log(const ConfigNode& node);
			bool initialize_services(const std::list<ConfigNode>& nodes);
			
			void run();
			void start_services();
			void stop_services();
			void join_services();
		private:
			bool load_library(const std::string& name);
			void unload_all_libraries();
			void cleanup();

			Config config_{};	
			std::string config_file_{};
			std::string log_file_name_{};
			bool log_initialized_ {false};
			bool is_running_ {false};
			snake::concurrency::Mutex mutex_{};
			snake::concurrency::ConditionVariable cv_{};
			std::map<std::string, ServicePointer> services_map_{};
			std::map<std::string, void*> loaded_libs_{};
		};
	}
}

#endif
