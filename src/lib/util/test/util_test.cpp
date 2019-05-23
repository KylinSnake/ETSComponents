#include <catch.hpp>
#include <snake/util/BufferPool.h>
#include <snake/util/String.h>
#include <snake/util/Exception.h>
#include <snake/util/DateTime.h>

#include <iostream>

using namespace snake::core;

struct buffer_item
{
	char name[32]{};
	double score{0};
	int number{0};
};

TEST_CASE("test_datetime", "[Date][Time][DateTime][Util]")
{
	auto today1 = DateTime::today();
	auto now1 = DateTime::now();
	REQUIRE(today1 == now1.date());
	REQUIRE(DateTime() == DateTime::null());
	REQUIRE_FALSE(DateTime().valid());
	REQUIRE(today1.valid());

	DateTime dt1;
	REQUIRE(DateTime::parse_from_default("20190120T123343", dt1));
	REQUIRE(dt1.year() == 2019);
	REQUIRE(dt1.month() == 1);
	REQUIRE(dt1.day() == 20);
	REQUIRE(dt1.hour() == 12);
	REQUIRE(dt1.minute() == 33);
	REQUIRE(dt1.second() == 43);
	REQUIRE(dt1.weekday() == 0);
	REQUIRE(dt1.is_weekend());
	REQUIRE(dt1.micro_second() == 0);

	DateTime dt2;
	REQUIRE(DateTime::parse_time_from_default("14:50:30", dt2));
	REQUIRE(dt2.year() == 1899);
	REQUIRE(dt2.month() == 12);
	REQUIRE(dt2.day() == 31);
	REQUIRE(dt2.hour() == 14);
	REQUIRE(dt2.minute() == 50);
	REQUIRE(dt2.second() == 30);
	REQUIRE(dt2.micro_second() == 0);

	DateTime dt3;
	REQUIRE(DateTime::parse_date_from_default("20190121", dt3));
	REQUIRE(dt3.year() == 2019);
	REQUIRE(dt3.month() == 1);
	REQUIRE(dt3.day() == 21);
	REQUIRE(dt3.hour() == 0);
	REQUIRE(dt3.minute() == 0);
	REQUIRE(dt3.second() == 0);
	REQUIRE(dt3.micro_second() == 0);
	REQUIRE(dt3.weekday() == 1);
	REQUIRE_FALSE(dt3.is_weekend());

	DateTime dt4;
	REQUIRE(DateTime::parse("%Y/%m/%d %H:%M:%S", "2019/01/20 12:33:43", dt4));
	REQUIRE(dt4 == dt1);
	REQUIRE(dt4 < dt3);
	REQUIRE(dt4 > dt2);
	REQUIRE(dt4 != dt3);
	REQUIRE(dt4 >= dt1);
	REQUIRE(dt4 <= dt1);

	auto dt5 = dt4 + Days(2);
	REQUIRE(dt5.format("%Y-%m-%d %H:%M:%S", 3) == "2019-01-22 12:33:43.000");
	dt5 += Microseconds(1203);
	REQUIRE(dt5.format("%Y-%m-%d %H:%M:%S", 3) == "2019-01-22 12:33:43.001");
	REQUIRE(dt5.format("%Y-%m-%d %H:%M:%S", 6) == "2019-01-22 12:33:43.001203");
	dt5 -= Nanoseconds(101);
	REQUIRE(dt5.format("%Y-%m-%d %H:%M:%S", 6) == "2019-01-22 12:33:43.001202");
	REQUIRE(dt5.format("%Y-%m-%d %H:%M:%S", 9) == "2019-01-22 12:33:43.001202899");
	REQUIRE(dt5.format("%Y-%m-%d %H:%M:%S", 7) == "2019-01-22 12:33:43.0012028");
	REQUIRE(dt5.format("%Y-%m-%d %H:%M:%S", 1) == "2019-01-22 12:33:43.0");

	REQUIRE(Days(1) == Hours(24));
	REQUIRE(Hours(1) == Minutes(60));
	REQUIRE(Minutes(1) == Seconds(60));
	REQUIRE(Seconds(1) == Milliseconds(1000));
	REQUIRE(Milliseconds(1) == Microseconds(1000));
	REQUIRE(Microseconds(1) == Nanoseconds(1000));
}

TEST_CASE("test_string", "[String][Util]")
{
	std::string a1="\t\n 1234\t\n\r ";
	std::string a2=a1;
	auto& a3 = ltrim(a2);
	REQUIRE(&a3 == &a2);
	REQUIRE(a2.length() == 8);
	REQUIRE(a2.substr(0,4) == "1234");
	auto& a4 = rtrim(a2);
	REQUIRE(&a4 == &a3);
	REQUIRE(&a4 == &a2);
	REQUIRE(a2.length() == 4);
	REQUIRE(a2 == "1234");
	auto b2 = a1;
	auto& b3 = trim(b2);
	REQUIRE(&b3 == &b2);
	REQUIRE(b2 == "1234");

	std::string c="1 23 45  67 890;321;;12";
	auto v = split(c, " ;");
	REQUIRE(v.size() == 9);
	REQUIRE(v[0]=="1");
	REQUIRE(v[1]=="23");
	REQUIRE(v[2]=="45");
	REQUIRE(v[3]=="");
	REQUIRE(v[4]=="67");
	REQUIRE(v[5]=="890");
	REQUIRE(v[6]=="321");
	REQUIRE(v[7]=="");
	REQUIRE(v[8]=="12");

	c="123aBcD \t";
	std::string cA="123ABCD \t";
	std::string cB="123abcd \t";
	REQUIRE(to_upper(c) == cA);
	REQUIRE(to_lower(c) == cB);

	std::string d="TEsT.aNd.cHeCK";
	auto e=to_camel(d, '.');
	REQUIRE(e == "TestAndCheck");
	REQUIRE(camel_to_lower_case(e) == "test_and_check");
}


TEMPLATE_TEST_CASE("test_buffer_pool", "[BufferPool][Util]", int, double, buffer_item)
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
