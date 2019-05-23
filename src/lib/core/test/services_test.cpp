#include <future>
#include <chrono>
#include <iostream>

#include <catch.hpp>
#include <ServiceT.h>
#include <Application.hpp>
#include <Config.h>
#include <Exception.h>

#include "fake_service.hpp"

using namespace snake::core;

TEST_CASE("test_config", "[Config][YAML][Util]")
{
	std::string config_content= R"(
SecurityCache: "<%= ENV['HOME'] %>/jupyter/workstation/pyTrading/data/security.csv"
MarketDataManager: 
  Indicator:    
    - Id: CCI_18      
      SecId: IF0001 
      Type: CCI 
      Parameters: 18 
    - Id: DMI_14 
      SecId: IF0001 
      Type: DMI 
      Parameters: 
        DI Length: 14 
        ADX Smooth: 14 
  MarketData: 
    - HSIY0 
    - IF0001
  Number:
    - 10
    - 20
    - 30
  Score:
    - 1.0
    - 2.0
    - 3.0
  Answer:
    - True
    - False
    - True
    - False
TradeManager: 
RiskManager: 
  MaxCapitalPercentage: 50 
  TotalRiskPercentage: 2.0
  IsInitialized: True
)";
	Config config{};
	REQUIRE_FALSE(config.root().is_valid());
	REQUIRE_NOTHROW(config.init_from_string(config_content));
	
	REQUIRE(config.root().is_valid());
	auto root = config.root();
	REQUIRE(root.has_child("RiskManager"));
	REQUIRE_FALSE(root.has_child("NO_NODE"));
	auto risk_node = root.get_child("RiskManager");
	REQUIRE(risk_node.is_valid());
	REQUIRE(risk_node.get_integer_value("MaxCapitalPercentage") == 50);
	REQUIRE(risk_node.get_float_value("TotalRiskPercentage") == 2.0);
	REQUIRE(risk_node.get_boolean_value("IsInitialized"));

	REQUIRE(risk_node.get_integer_with_default("MaxCapitalPercentage", 1) == 50);
	REQUIRE(risk_node.get_integer_with_default("MaxCapitalPercentage_XX", 1) == 1);
	REQUIRE(risk_node.get_float_with_default("TotalRiskPercentage", 10.0) == 2.0);
	REQUIRE(risk_node.get_float_with_default("TotalRiskPercentage_XX", 10.0) == 10.0);
	REQUIRE(risk_node.get_boolean_with_default("IsInitialized", false));
	REQUIRE(risk_node.get_boolean_with_default("IsInitialized_XX", false) == false);

	std::string sec_path;
	REQUIRE_NOTHROW((sec_path=root.get_string_value("SecurityCache")));
	REQUIRE_THROWS_AS(root.get_integer_value("SecurityCache"), WrongTypeException); 
	REQUIRE_THROWS_AS(root.get_string_value("SecurityCache__XXX"), InvalidArgumentException); 
	using Catch::EndsWith;
	REQUIRE_THAT(sec_path, EndsWith("data/security.csv"));

	auto md_node = root.get_child("MarketDataManager");
	auto ind_node = md_node.get_child("Indicator");
	std::list<ConfigNode> nodes = ind_node.get_node_list_of_children();
	REQUIRE(nodes.size() == 2);
	REQUIRE(nodes.front().get_string_value("Id") == "CCI_18");
	REQUIRE(nodes.back().get_string_value("Id") == "DMI_14");

	REQUIRE(md_node.get_child("MarketData").get_string_list_of_children() == std::list<std::string>{"HSIY0", "IF0001"});
	REQUIRE(md_node.get_child("Number").get_integer_list_of_children() == std::list<int>{10,20,30});
	REQUIRE(md_node.get_child("Score").get_float_list_of_children() == std::list<double>{1.0,2.0,3.0});
	REQUIRE(md_node.get_child("Answer").get_boolean_list_of_children() == std::list<bool>{true,false,true,false});
}

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
