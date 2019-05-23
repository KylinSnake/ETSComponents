#include <snake/message/TestMessage.h>

#include <catch.hpp>

#include <iostream>

using namespace snake::message;

TEST_CASE("test_message_generator_with_test_message", "[TestMessage][Message][MessageGenerator][Proto][Protobuf]")
{
	TestMessage msg1{}, msg2{};
	REQUIRE(msg1.type() == MessageType::TEST_MESSAGE);
	
	MessageVersion version;
	REQUIRE(version.major_version == MESSAGE_MAJOR_VERSION);
	REQUIRE(version.minor_version == MESSAGE_MINOR_VERSION);
	REQUIRE(version.minimal_major_version == MESSAGE_MINIMAL_MAJOR_VERSION);
	REQUIRE(version.minimal_minor_version == MESSAGE_MINIMAL_MINOR_VERSION);
	std::string s{};
	REQUIRE(version.serialize(s));
	MessageVersion v2;
	REQUIRE(v2.deserialize(s));
	REQUIRE(version.major_version == v2.major_version);
	REQUIRE(version.minor_version == v2.minor_version);
	REQUIRE(version.minimal_major_version == v2.minimal_major_version);
	REQUIRE(version.minimal_minor_version == v2.minimal_minor_version);

	msg1.group.push_back(version);
	msg1.group.push_back(v2);
	msg1.msg = v2;
	msg1.types.push_back(MessageType::TEST_MESSAGE);
	msg1.types.push_back(MessageType::CONTROL_MESSAGE);
	msg1.strings.push_back("Test1");
	msg1.strings.push_back("Test2");
	msg1.integers.push_back(13);
	msg1.integers.push_back(42);

	auto t = snake::util::DateTime::now();
	msg1.stamps.push_back(t);
	msg1.stamps.push_back(t - snake::util::Days(1));

	msg1.version_map["V1"] = version;
	msg1.version_map["V2"] = v2;
	msg1.time_map[1] = msg1.stamps[0];
	msg1.time_map[10] = msg1.stamps[1];

	REQUIRE(msg1.serialize(s));
	REQUIRE(msg2.deserialize(s));

	REQUIRE(msg1.group.size() == msg2.group.size());
	REQUIRE(msg1.group[0].major_version == msg2.group[0].major_version);
	REQUIRE(msg1.group[0].minor_version == msg2.group[0].minor_version);
	REQUIRE(msg1.msg.minor_version == msg2.msg.minor_version);
	REQUIRE(msg1.types.size() == msg2.types.size());
	REQUIRE(msg1.types[0] == msg2.types[0]);
	REQUIRE(msg1.types[1] == msg2.types[1]);
	REQUIRE(msg1.strings.size() == msg2.strings.size());
	REQUIRE(msg1.strings[0] == msg2.strings[0]);
	REQUIRE(msg1.strings[1] == msg2.strings[1]);
	REQUIRE(msg1.integers.size() == msg2.integers.size());
	REQUIRE(msg1.integers[0] == msg2.integers[0]);
	REQUIRE(msg1.integers[1] == msg2.integers[1]);
	REQUIRE(msg1.stamps.size() == msg2.stamps.size());
	REQUIRE(msg1.stamps[0] == msg2.stamps[0]);
	REQUIRE(msg1.stamps[1] == msg2.stamps[1]);
	REQUIRE(msg2.stamps[0] - msg2.stamps[1] == snake::util::Days(1));
	REQUIRE(msg1.version_map.size() == msg2.version_map.size());
	REQUIRE(msg1.time_map.size() == msg2.time_map.size());
	REQUIRE(msg1.version_map["V1"] == msg2.version_map["V1"]);
	REQUIRE(msg1.version_map["V2"] == msg2.version_map["V2"]);
	REQUIRE(msg1.time_map[1] == msg2.time_map[1]);
	REQUIRE(msg1.time_map[10] == msg2.time_map[10]);
}
