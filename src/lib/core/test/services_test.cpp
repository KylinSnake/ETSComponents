#include <future>
#include <chrono>
#include <iostream>

#include <catch.hpp>
#include <ServiceT.h>
#include <Application.hpp>

#include "fake_service.hpp"

using namespace snake::core;

TEST_CASE("test_service_lib", "[services][lib]")
{
	Application app;
	std::string yaml_content = R"(
LogLevel: TRACE
Services:
  - name: FakeService
)";
	Config config;
	REQUIRE_NOTHROW(config.init_from_string(yaml_content));
	REQUIRE(app.initialize_log(config.root()));
	std::list<ConfigNode> nodes = config.root().get_child("Services").get_node_list_of_children();
	REQUIRE(nodes.size() == 1);
	REQUIRE(nodes.front().get_string_value("name") == "FakeService");
	REQUIRE_FALSE(app.initialize_services(nodes));

	yaml_content = R"(
Services:
  - name: FakeService
    lib:
      - test_service_1
)";
	REQUIRE_NOTHROW(config.init_from_string(yaml_content));
	REQUIRE_FALSE(app.initialize_services(config.root().get_child("Services").get_node_list_of_children()));

	yaml_content = R"(
LogLevel: INFO
Services:
  - name: FakeService
    lib:
      - test_service
    config:
      Pass: True
)";
	REQUIRE_NOTHROW(config.init_from_string(yaml_content));
	REQUIRE(app.initialize_log(config.root()));
	nodes = config.root().get_child("Services").get_node_list_of_children();
	REQUIRE(app.initialize_services(nodes));

	auto ptr = app.get_service_pointer("FakeService");
	REQUIRE(ptr != nullptr);
	auto svc_ptr = std::dynamic_pointer_cast<snake::test::FakeService>(ptr);
	ptr.reset();
	REQUIRE(svc_ptr != nullptr);
	REQUIRE(svc_ptr->is_reload_successful);
	REQUIRE(svc_ptr->is_init_successful);

	app.start_services();

	REQUIRE(svc_ptr->is_before_start_successful);
	REQUIRE(svc_ptr->is_on_start_successful);

	auto fut = std::async([&app]()
	{
		using namespace std::chrono_literals;
		std::this_thread::sleep_for(2s);
		app.shutdown();
	});

	app.run();
	fut.get();
	
	app.stop_services();
	REQUIRE(svc_ptr->is_before_stop_successful);
	REQUIRE(svc_ptr->is_on_stop_successful);
	app.join_services();
	REQUIRE(svc_ptr->is_on_join_successful);
	REQUIRE(svc_ptr->is_on_stopped_successful);
	svc_ptr.reset();
}
