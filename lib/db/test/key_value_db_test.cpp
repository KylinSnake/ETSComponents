#include <catch.hpp>
#include <cstdlib>
#include <snake/util/DateTime.h>
#include <snake/db/KeyValueLocalDb.h>
#include <snake/db/ConcurrentPersistMap.h>
#include <iostream>
#include <thread>

using namespace snake;

struct TestFolder
{
	TestFolder()
	{
		folder = "db_" + DateTime::now().format("%Y%m%d_%H%M%S",0);
		std::string cmd = "mkdir -p ./" + folder;
		REQUIRE(0 == std::system(cmd.data()));
	}
	~TestFolder()
	{
		std::string cmd = "rm -rf ./" + folder;
		std::system(cmd.data());
	}
	std::string folder{};
};

struct TestType
{
	int key1{};
	std::string key2{};
	std::list<double> marks;

	void deserialize(const char* s, std::size_t size)
	{
		TestType t{};
		size_t cur = 0;
		if(sizeof(int) + sizeof(size_t) * 2 <= size)
		{
			memcpy(&t.key1, s + cur, sizeof(int));
			cur += sizeof(int);
			size_t length = *(reinterpret_cast<const size_t*>(s + cur));
			cur += sizeof(size_t);
			if(cur + length + sizeof(size_t) <= size)
			{
				t.key2=std::string{s+cur, length};
				cur += length;
				length=*(reinterpret_cast<const size_t*>(s + cur));
				cur += sizeof(size_t);
				if (cur + length * sizeof(double) <= size)
				{
					for(size_t i = 0; i < length; ++i)
					{
						double d = *(reinterpret_cast<const double*>(s + cur));
						t.marks.push_back(d);
						cur += sizeof(double);
					}
					assert(cur == size);
					*this = t;
					return;
				}
			}
		}
		throw OverflowException("deserialize overflow");
	}

	size_t serialize(char* s, std::size_t capacity) const
	{
		size_t size = sizeof(int) + sizeof(size_t) + key2.size() + sizeof(size_t) + sizeof(double) * marks.size();
		if(size <= capacity)
		{
			size_t cur = 0;
			memcpy(s, &key1, sizeof(int));
			cur += sizeof(int);
			*(reinterpret_cast<size_t*>(s + cur)) = key2.size();
			cur += sizeof(size_t);
			memcpy(s + cur, key2.data(), key2.size());
			cur += key2.size();
			*(reinterpret_cast<size_t*>(s + cur)) = marks.size();
			cur += sizeof(size_t);
			for(double i: marks)
			{
				memcpy(s + cur, &i, sizeof(double));
				cur += sizeof(double);
			}
			assert(cur == size);
			return size;
		}
		else
		{
			throw OverflowException("serialize overflow");
		}
	}
};

TEST_CASE("test_key_value_local_db", "[KeyValue][Db][Local]")
{
	TestFolder f;
	DbOption option;
	KeyValueLocalDb db1{};
	REQUIRE(db1.is_open() == false);
	option.create_if_not_exist_ = false;
	REQUIRE_THROWS_AS(db1.open(f.folder, "db1", option),DBException); 
	option.create_if_not_exist_ = true;
	REQUIRE_NOTHROW(db1.open(f.folder, "db1", option));
	REQUIRE(db1.is_open());
	KeyValueLocalDb db2{};
	REQUIRE_THROWS_AS(db2.open(f.folder, option), DBException);
	REQUIRE(db2.is_open() == false);
	db2 = std::move(db1);
	REQUIRE(db2.name() == "db1");
	REQUIRE(db1.is_open() == false);
	REQUIRE(db2.is_open());
	KeyValueLocalDb db{std::move(db2)};
	REQUIRE(db2.is_open() == false);
	REQUIRE(db.is_open());
	db.close();
	REQUIRE(db.is_open() == false);
	option.create_if_not_exist_ = false;
	REQUIRE_NOTHROW(db.open(f.folder, "db", option));
	REQUIRE(db.name() == "db");

	double key1 = 12.345;
	int key2 = 34;
	const char* key3 = "TestKey";
	bool key4 = false;
	std::string key5 = "stringKey";
	char key6 = 'A';
	
	char value1 = 'B';
	std::string value2 = "ValueString";
	bool value3 = true;
	float value4 = 1.431;
	int value5 = 55;
	double value6 = -10.2;

	REQUIRE(!db.has(key1));
	REQUIRE(!db.has(key2));
	REQUIRE(!db.has(key3));
	REQUIRE(!db.has(key4));
	REQUIRE(!db.has(key5));
	REQUIRE(!db.has(key6));

	REQUIRE_NOTHROW(db.put(key1, value1));
	REQUIRE_NOTHROW(db.put(key2, value2));
	REQUIRE_NOTHROW(db.put(key3, value3));
	REQUIRE_NOTHROW(db.put(key4, value4));
	REQUIRE_NOTHROW(db.put(key5, value5));
	REQUIRE_NOTHROW(db.put(key6, value6));

	REQUIRE(db.has(key1));
	REQUIRE(db.has(key2));
	REQUIRE(db.has(key3));
	REQUIRE(db.has(key4));
	REQUIRE(db.has(key5));
	REQUIRE(db.has(key6));

	char v1{};
	std::string v2{};
	bool v3{};
	float v4{};
	int v5{};
	double v6{};

	REQUIRE(db.get(key1, v1));
	REQUIRE(db.get(key2, v2));
	REQUIRE(db.get(key3, v3));
	REQUIRE(db.get(key4, v4));
	REQUIRE(db.get(key5, v5));
	REQUIRE(db.get(key6, v6));

	REQUIRE(v1 == value1);
	REQUIRE(v2 == value2);
	REQUIRE(v3 == value3);
	REQUIRE(v4 == value4);
	REQUIRE(v5 == value5);
	REQUIRE(v6 == value6);

	REQUIRE_NOTHROW(db.remove(key1));
	REQUIRE_NOTHROW(db.remove(key6));
	REQUIRE_NOTHROW(db.remove(key3));
	REQUIRE_NOTHROW(db.remove(key3));
	REQUIRE(db.has(key1) == false);
	REQUIRE(db.has(key2));
	REQUIRE(db.has(key3) == false);
	REQUIRE(db.has(key4));
	REQUIRE(db.has(key5));
	REQUIRE(db.has(key6) == false);
	REQUIRE(db.get(key1, v1) == false);

	REQUIRE_NOTHROW(db.remove(key1));
	REQUIRE_NOTHROW(db.remove(key2));
	REQUIRE_NOTHROW(db.remove(key3));
	REQUIRE_NOTHROW(db.remove(key4));
	REQUIRE_NOTHROW(db.remove(key5));
	REQUIRE_NOTHROW(db.remove(key6));

	REQUIRE_NOTHROW(db.put(10,-10.0));
	REQUIRE_NOTHROW(db.put(20,-20.0));
	REQUIRE_NOTHROW(db.put(30,-30.0));
	REQUIRE_NOTHROW(db.put(40,-40.0));
	REQUIRE_NOTHROW(db.put(50,-50.0));
	REQUIRE_NOTHROW(db.put(60,-60.0));
	REQUIRE_NOTHROW(db.put(70,-70.0));

	std::unordered_map<int, double> all_values{};
	std::list<std::pair<int,double>> list_values{};
	REQUIRE_NOTHROW(list_values = db.read_all<int,double>());
	for(auto& p : list_values)
	{
		all_values[p.first] = p.second;
	}
	REQUIRE(all_values[10] == -10.0);
	REQUIRE(all_values[20] == -20.0);
	REQUIRE(all_values[30] == -30.0);
	REQUIRE(all_values[40] == -40.0);
	REQUIRE(all_values[50] == -50.0);
	REQUIRE(all_values[60] == -60.0);
	REQUIRE(all_values[70] == -70.0);
	REQUIRE_NOTHROW(db.unsafe_clear());
	REQUIRE_NOTHROW(list_values = db.read_all<int,double>());
	REQUIRE(list_values.size() == 0);

	TestType key{};
	TestType value{};
	key.key1=100;
	key.key2="Key Test";
	key.marks={1.2,3.5,6.7};

	value.key1 = 10;
	value.key2 = "Value Test Again,haha";
	value.marks = {100.1,20.5,7.0,-12.0,1203.2,44.0,1221.4};

	REQUIRE_NOTHROW(db.put(key, value));

	TestType tv{};
	REQUIRE_NOTHROW(db.get(key, tv));
	REQUIRE(tv.key1 == value.key1);
	REQUIRE(tv.key2 == value.key2);
	REQUIRE(tv.marks == value.marks);

	REQUIRE_NOTHROW(db.close());
	option.key_buffer_size_ = 16;
	option.value_buffer_size_ = 16;
	REQUIRE_NOTHROW(db.open(f.folder, "db", option));
	REQUIRE(db.is_open());
	REQUIRE_THROWS_AS(db.put(key, value), OverflowException);
	db.set_key_buffer_size(1024);
	REQUIRE_THROWS_AS(db.put(key, value), OverflowException);
	db.set_value_buffer_size(1024);
	REQUIRE_NOTHROW(db.put(key, value));

	REQUIRE_NOTHROW(db.close());
}

TEST_CASE("test_concurrent_persist_db", "[KeyValue][Db][Local][Hash][Map][Concurrent]")
{
	TestFolder f;
	DbOption option;
	{
		ConcurrentPersistMap<int, std::string> pmap{};
		REQUIRE(pmap.inited() == false);
		REQUIRE(pmap.init(f.folder, option));
		REQUIRE(pmap.inited());
		REQUIRE(pmap.size() == 0);

		std::string value1 = "Test1";
		REQUIRE(pmap.put(1, value1));
		REQUIRE(pmap.put(2, std::string{"Test2"}));
		REQUIRE_NOTHROW(pmap.persist(1));
	}
	{
		ConcurrentPersistMap<int, std::string> pmap{};
		REQUIRE(pmap.init(f.folder, option));
		REQUIRE(pmap.inited());
		REQUIRE(pmap.size() == 1);
		REQUIRE(pmap.has(1));
		REQUIRE(pmap.has(2) == false);
		std::string ret{};
		REQUIRE(pmap.get_copy(1, ret));
		REQUIRE(ret == "Test1");
		REQUIRE_NOTHROW(pmap.remove(1));
		auto iter = pmap.get_or_create(2);
		REQUIRE(pmap.size() == 1);
		REQUIRE(iter.valid());
		iter.value() = "iter test";
		REQUIRE_NOTHROW(iter.commit_and_release());
		REQUIRE(pmap.get_copy(2, ret));
		REQUIRE(ret == "iter test");
		REQUIRE(iter.valid() == false);

		REQUIRE(pmap.size() == 1);
		iter = pmap.get_or_create(3);
		REQUIRE(pmap.size() == 2);
		iter.value() = "iter test 2";
		REQUIRE_NOTHROW(iter.rollback_and_release());
		REQUIRE(iter.valid() == false);
		REQUIRE(pmap.size() == 1);
		REQUIRE(pmap.has(3) == false);

		iter = pmap.get_or_create(3);
		REQUIRE(pmap.size() == 2);
		iter.value() = "iter test 3";
		REQUIRE_NOTHROW(iter.commit_and_release());
		REQUIRE(pmap.get_copy(3, ret));
		REQUIRE(ret == "iter test 3");
		REQUIRE(iter.valid() == false);

		iter = pmap.get_or_create(3);
		REQUIRE(pmap.size() == 2);
		iter.value() = "iter test 3 again and again";
		REQUIRE_NOTHROW(iter.rollback_and_release());
		REQUIRE(iter.valid() == false);
		REQUIRE(pmap.size() == 2);
		REQUIRE(pmap.get_copy(3, ret));
		REQUIRE(ret == "iter test 3");

		iter = pmap.get_or_create(2);
		REQUIRE(pmap.size() == 2);
		REQUIRE_NOTHROW(iter.remove_and_commit());
		REQUIRE(pmap.size() == 1);
		REQUIRE(pmap.has(3));
		REQUIRE(pmap.has(2) == false);
	}
	{
		ConcurrentPersistMap<int, std::string> pmap{};
		REQUIRE(pmap.init(f.folder, option));
		REQUIRE(pmap.inited());
		REQUIRE(pmap.size() == 1);
		REQUIRE(pmap.has(3));

		pmap.for_each([](const std::pair<int, std::string>& p)
		{
			REQUIRE(p.first == 3);
			REQUIRE(p.second == "iter test 3");
		});
	}
}

TEST_CASE("test_concurrent_persist_db_in_mult_thread", "[KeyValue][Db][Local][Hash][Map][Concurrent][MultiThread]")
{
	ConcurrentHashMap<int, int> umap{};
	TestFolder f;
	DbOption option;
	{
		ConcurrentPersistMap<int, int> pmap{};
		REQUIRE(pmap.init(f.folder, option));
		REQUIRE(pmap.inited());
		auto e = [&umap, &pmap]()
		{
			const size_t iterations = 1000;
			for(size_t n = 0; n < iterations; ++n)
			{
				int i = std::rand() % iterations;
				int j = std::rand() % 5;

				if(j == 0)
				{
					auto p = pmap.get_or_create(i);
					p.value() = std::rand();
					umap.put(i, p.value(), true);
					p.commit_and_release();
				}
				else if (j == 1)
				{
					auto p = pmap.get_or_create(i);
					p.value() = std::rand();
					p.rollback_and_release();
				}
				else if (j == 2)
				{
					auto p = pmap.get(i);
					if(p.valid())
					{
						umap.remove(i);
						p.remove_and_commit();
					}
				}
				else if (j == 3)
				{
					pmap.get(i);
				}
			}
		};

		std::vector<std::thread> threads;
		size_t threads_num = 4;
		for(size_t i = 0; i < threads_num; ++i)
		{
			threads.push_back(std::thread{e});
		}

		for(size_t i = 0; i < threads_num; ++i)
		{
			threads[i].join();
		}
	}
	{
		ConcurrentPersistMap<int, int> pmap{};
		REQUIRE(pmap.init(f.folder, option));
		REQUIRE(pmap.inited());
		REQUIRE(pmap.size() == umap.size());
		umap.for_each([&pmap](const std::pair<int, int>& p)
		{
			int out;
			REQUIRE(pmap.get_copy(p.first, out));
			REQUIRE(out == p.second);
		});
	}
	
}
