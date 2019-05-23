#ifndef SNAKE_TEST_FAKE_SERVICE_HPP
#define SNAKE_TEST_FAKE_SERVICE_HPP

#include <snake/core/ServiceT.h>

namespace snake
{
	namespace test
	{
	using namespace snake::core;
	class FakeService : public ServiceT<FakeService>
	{
	public:
		bool is_reload_successful { false };
		bool is_init_successful { false };
		bool is_before_start_successful { false };
		bool is_on_start_successful { false };
		bool is_before_stop_successful { false };
		bool is_on_stop_successful { false };
		bool is_on_join_successful { false };
		bool is_on_stopped_successful { false };
	public:
		using Base=snake::core::ServiceT<FakeService>;
		FakeService() = default;
		virtual ~FakeService() = default;

		static const char* type_name()
		{
			return "FakeService";
		}

		virtual bool reload(const ConfigNode& node) override
		{
			is_reload_successful = node.has_child("Pass");
			return Base::reload(node);
		}
	protected:
		virtual bool init(const ConfigNode& node) override
		{
			is_init_successful = node.has_child("Pass");
			return Base::init(node) && node.get_boolean_with_default("Pass", true);
		}

		virtual void before_start() override
		{
			is_before_start_successful = is_initialized() && !(is_running() || is_stopping());
			Base::before_start();
		}

		virtual void on_start() override
		{
			is_on_start_successful = is_initialized() && is_running() && !is_stopping();
			Base::on_start();
		}

		virtual void before_stop() override
		{
			is_before_stop_successful = is_initialized() && is_running() && !is_stopping();
			Base::before_stop();
		}

		virtual void on_stop() override
		{
			is_on_stop_successful = is_initialized() && !is_running() && is_stopping();
			Base::on_stop();
		}

		virtual void on_join() override
		{
			is_on_join_successful = is_initialized() && !is_running() && is_stopping();
			Base::on_join();
		}

		virtual void on_stopped() override
		{
			is_on_stopped_successful = is_initialized() && !(is_running() || is_stopping());
			Base::on_stopped();
		}

	};

	}
}

#endif
