#include <catch.hpp>
#include <BufferPool.h>
#include <Config.h>
#include <Exception.h>

using namespace snake::core;

struct buffer_item
{
	char name[32]{};
	double score{0};
	int number{0};
};

TEST_CASE("test_config", "[Config][YAML]")
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

TEMPLATE_TEST_CASE("test_buffer_pool", "[BufferPool]", int, double, buffer_item)
{
	BufferPool<TestType> pool{16, 2};
	auto length = pool.buffer_length();
	REQUIRE(length == 16);
	REQUIRE(pool.capacity()  == 2);
	REQUIRE(pool.size() == 0);
	auto p1 = pool.malloc();

	REQUIRE(sizeof(p1[length-1]) == sizeof(TestType));
	REQUIRE(pool.capacity()  == 2);
	REQUIRE(pool.size() == 1);
	auto p2 = pool.malloc();
	REQUIRE(pool.size() == 2);
	REQUIRE(p1 != p2);
	pool.free(p2);
	REQUIRE(pool.capacity()  == 2);
	REQUIRE(pool.size() == 1);
	auto p3 = pool.malloc();
	REQUIRE(p3 == p2);
	pool.free(p1);
	REQUIRE(pool.capacity()  == 2);
	REQUIRE(pool.size() == 1);
	auto p4 = pool.malloc();
	REQUIRE(p4 == p1);

	REQUIRE(pool.capacity()  == 2);
	REQUIRE(pool.size() == 2);

	auto p5 = pool.malloc();
	REQUIRE(pool.capacity()  == 4);
	REQUIRE(pool.size() == 3);

	pool.free(p3);
	pool.free(p4);
	pool.free(p5);

	REQUIRE(pool.capacity()  == 4);
	REQUIRE(pool.size() == 0);
}
