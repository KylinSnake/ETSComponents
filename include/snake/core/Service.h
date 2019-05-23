#ifndef SNAKE_CORE_SERVICE_H
#define SNAKE_CORE_SERVICE_H

#include <snake/core/Config.h>
#include <string>
#include <memory>

namespace snake
{
	namespace core
	{
		class Service
		{
		public:
			Service() = default;
			virtual ~Service() = default;

			Service(const Service&) = delete;
			Service& operator=(const Service&) = delete;


			std::string name() const { return name_; }

			bool initialize(const std::string& name, const ConfigNode& node);
			virtual bool reload(const ConfigNode& node) { return true; }

			void start();
			void stop();
			void join();

			bool is_running() const { return is_running_; }
			bool is_stopping() const { return is_stopping_; }
			bool is_initialized() const { return is_initialized_;}
		protected:
			virtual bool init(const ConfigNode& node);
			virtual void before_start(){}
			virtual void on_start(){}
			virtual void before_stop(){}
			virtual void on_stop(){}
			virtual void on_join(){}
			virtual void on_stopped(){}

		private:
			bool is_running_ {false};
			bool is_stopping_ {false};
			bool is_initialized_{false};
			std::string name_{};
		};

		using ServicePointer = std::shared_ptr<Service>;

		template<class T>
		using ServicePointerT = std::shared_ptr<T>;

	}
}

#endif
