#include <catch.hpp>
#include <snake/concurrency/ConcurrentHashMap.h>
#include <snake/concurrency/Mutex.h>
#include <snake/concurrency/Thread.h>
#include <snake/concurrency/ConditionVariable.h>
#include <vector>
#include <iostream>
#include <cstdlib>

using namespace snake;

Atomic<unsigned short> signal = {0}; // 0: stop, 1: running, 2: pause
Atomic<unsigned short> exceptions = {0};
Atomic<unsigned short> thread_sync = {0};
const unsigned short threads_num = 4;
const unsigned int iterations_in_thread = 10000;
const unsigned int iterations = 10;

namespace __test_details__
{
	enum op
	{
		READ_LOCK = 0,
		READ_UNLOCK = 1,
		WRITE_LOCK = 2,
		WRITE_UNLOCK = 3
	};

	enum lock_level
	{
		TABLE = 0,
		BUCKET = 1
	};

	struct alloc_op
	{
		Atomic<unsigned long> alloc_count {0};
		Atomic<unsigned long> dealloc_count {0};
		Atomic<unsigned long> bucket_alloc {0};
		Atomic<unsigned long> bucket_dealloc {0};

		void reset()
		{
			alloc_count = 0;
			dealloc_count = 0;
			bucket_alloc = 0;
			bucket_dealloc = 0;
		}
	};

	inline std::vector<std::pair<lock_level, op>>& lock_counter()
	{
		static std::vector<std::pair<lock_level, op>> t{};
		return t;
	}

	inline alloc_op& alloc_counter()
	{
		static alloc_op t{};
		return t;
	}

	template<lock_level LEVEL>
	struct CountLock
	{
		void lock() { lock_counter().push_back(std::make_pair(LEVEL, WRITE_LOCK));}
		void unlock() { lock_counter().push_back(std::make_pair(LEVEL, WRITE_UNLOCK)); }
		void lock_shared() { lock_counter().push_back(std::make_pair(LEVEL, READ_LOCK)); }
		void unlock_shared() { lock_counter().push_back(std::make_pair(LEVEL, READ_UNLOCK));}
	};

	using namespace std::chrono;
	class MT_SharedLock : public SharedTimedMutex
	{
	public:
		MT_SharedLock() = default;
		~MT_SharedLock() = default;

		void lock()
		{
			if(!SharedTimedMutex::try_lock_for(d))
			{
				exceptions++;
				throw std::runtime_error{"Write lock failed"};
			}
		}

		void lock_shared()
		{
			if(!SharedTimedMutex::try_lock_shared_for(d))
			{
				exceptions++;
				throw std::runtime_error{"Read lock failed"};
			}

		}
	private:
		minutes d = minutes{1};
	};

	bool log_alloc = false;
	std::mutex log_alloc_mutex{};
	template<typename T>
	struct TestAllocator : public std::allocator<T>
	{
		template<typename U>
		struct rebind
		{
			typedef TestAllocator<U> other;
		};
		using base = std::allocator<T>;
		inline T* allocate(std::size_t n)
		{
			if(n == 1)
			{
				unsigned long i = ++(alloc_counter().alloc_count);
				if(log_alloc)
				{
					std::unique_lock<std::mutex> l(log_alloc_mutex);
					std::cout << "Item Allocaton: " << i << std::endl;
				}
			}
			else
			{
				alloc_counter().bucket_alloc += n;
				unsigned long i =alloc_counter().bucket_alloc;
				if(log_alloc)
				{
					std::unique_lock<std::mutex> l(log_alloc_mutex);
					std::cout << "Bucket Allocation: " << i << std::endl;
				}
			}
			return base::allocate(n);
		}

		inline void deallocate( T* p, std::size_t n )
		{
			if(n == 1)
			{
				unsigned long i = ++(alloc_counter().dealloc_count);
				if(log_alloc)
				{
					std::unique_lock<std::mutex> l(log_alloc_mutex);
					std::cout << "Item DeAllocaton: " << i << std::endl;
				}
			}
			else
			{
				alloc_counter().bucket_dealloc += n;
				unsigned long i = (alloc_counter().bucket_dealloc);
				if(log_alloc)
				{
					std::unique_lock<std::mutex> l(log_alloc_mutex);
					std::cout << "Bucket DeAllocaton: " << i << std::endl;
				}
			}
			return base::deallocate(p, n);
		}
	};

	template<typename K, typename V>
	using ST_Map=ConcurrentHashMap<K, V, std::hash<K>, std::equal_to<K>, TestAllocator< std::pair<const K,V> >,
		CountLock<TABLE>, CountLock<BUCKET>>;
	
	template<typename K, typename V>
	using MT_Map=ConcurrentHashMap<K, V, std::hash<K>, std::equal_to<K>, TestAllocator< std::pair<const K,V> >,
		MT_SharedLock, MT_SharedLock>;
}

using namespace __test_details__;

TEST_CASE("test_basic_function_in_single_thread", "[Container][Hash][Map][Concurrency]")
{
	{
		using TestMap = ST_Map<std::size_t, std::size_t>;
		TestMap a{};

		std::vector<std::pair<lock_level, op>>  rehash_seq = {
			std::make_pair(TABLE, WRITE_LOCK),
			std::make_pair(TABLE, WRITE_UNLOCK),
			std::make_pair(TABLE, READ_LOCK),
			std::make_pair(BUCKET, WRITE_LOCK),
			std::make_pair(BUCKET, WRITE_UNLOCK),
			std::make_pair(TABLE, READ_UNLOCK)
		};

		std::vector<std::pair<lock_level, op>>  normal_seq = {
			std::make_pair(TABLE, READ_LOCK),
			std::make_pair(BUCKET, WRITE_LOCK),
			std::make_pair(BUCKET, WRITE_UNLOCK),
			std::make_pair(TABLE, READ_UNLOCK)
		};

		std::vector<std::pair<lock_level, op>>  normal_lock_seq = {
			std::make_pair(TABLE, READ_LOCK),
			std::make_pair(BUCKET, WRITE_LOCK),
		};

		std::vector<std::pair<lock_level, op>>  normal_unlock_seq = {
			std::make_pair(BUCKET, WRITE_UNLOCK),
			std::make_pair(TABLE, READ_UNLOCK)
		};

		REQUIRE(alloc_counter().alloc_count == 0);
		REQUIRE(alloc_counter().dealloc_count == 0);
		REQUIRE(alloc_counter().bucket_alloc == a.bucket_size());
		REQUIRE(alloc_counter().bucket_dealloc == 0);
		for(std::size_t i = 0; i < 1000; ++i)
		{
			auto b_size = a.bucket_size();
			REQUIRE(a.insert(i, i).valid());
			REQUIRE(a.size() == i + 1);
			if(a.bucket_size() > b_size)
			{
				REQUIRE(lock_counter() == rehash_seq);
			}
			else
			{
				REQUIRE(lock_counter() == normal_seq);
			}
			REQUIRE(a.size() == (i+1));
			REQUIRE(alloc_counter().alloc_count == a.size());
			REQUIRE(alloc_counter().dealloc_count == 0);
			REQUIRE(alloc_counter().bucket_alloc - alloc_counter().bucket_dealloc == a.bucket_size());
			lock_counter().clear();

			REQUIRE(a.get_copy(i) == i);
			REQUIRE(lock_counter() == normal_seq);
			lock_counter().clear();

			b_size = a.bucket_size();
			REQUIRE(a.insert(i, i + 1).valid() == false);
			REQUIRE(a.size() == (i+1));
			if(a.bucket_size() > b_size)
			{
				REQUIRE(lock_counter() == rehash_seq);
			}
			else
			{
				REQUIRE(lock_counter() == normal_seq);
			}
			lock_counter().clear();
		}

		size_t count = 0;
		a.for_each([&count](const TestMap::value_type& v)
		{
			if(v.first < 500)
			{
				++count;
			}
		});
		REQUIRE(count == 500);
		REQUIRE(lock_counter().size() == 2 + a.bucket_size() * 2);
		REQUIRE(lock_counter()[0] == std::make_pair(TABLE, READ_LOCK));
		REQUIRE(lock_counter()[lock_counter().size()-1] == std::make_pair(TABLE, READ_UNLOCK));

		for(size_t i = 1; i < lock_counter().size()-1;)
		{
			REQUIRE(lock_counter()[i++] == std::make_pair(BUCKET, WRITE_LOCK));
			REQUIRE(lock_counter()[i++] == std::make_pair(BUCKET, WRITE_UNLOCK));
		}

		std::size_t v1 = std::rand()%1000;
		auto it = a.find_if([v1](const TestMap::value_type& v)
		{
			return v.first == v1;
		});
		REQUIRE(it->first == v1);
		it.release();
		REQUIRE(it.valid() == false);
		lock_counter().clear();
		for(std::size_t i = 0; i < 1000; ++i)
		{
			auto p = std::rand() % 1000;
			auto b_size = a.bucket_size();
			REQUIRE(a.put(TestMap::value_type(p + 1000, p+1), false) == false);
			if(a.bucket_size() > b_size)
			{
				REQUIRE(lock_counter() == rehash_seq);
			}
			else
			{
				REQUIRE(lock_counter() == normal_seq);
			}
			lock_counter().clear();
		}

		for(std::size_t i = 0; i < 1000; ++i)
		{
			auto b_size = a.bucket_size();
			auto p = std::rand() % 1000;
			REQUIRE(a.put(TestMap::value_type(p + 2000, p+1), true) == true);
			if(a.bucket_size() > b_size)
			{
				REQUIRE(lock_counter() == rehash_seq);
			}
			else
			{
				REQUIRE(lock_counter() == normal_seq);
			}
			lock_counter().clear();
		}

		REQUIRE(a.put(TestMap::value_type(0, 0), false) == true);
		lock_counter().clear();
		for(std::size_t i = 0; i < 1000; ++i)
		{
			REQUIRE(a.put(TestMap::value_type(i, i+1), false) == true);
			REQUIRE(lock_counter() == normal_seq);
			lock_counter().clear();
		}
		
		{
			lock_counter().clear();
			auto key = std::rand() % 1000;
			auto accessor = a.get(key);
			REQUIRE(accessor.valid());
			REQUIRE(lock_counter() == normal_lock_seq);
			lock_counter().clear();
			auto value = accessor->second;
			accessor->second += 100;
			accessor.release();
			REQUIRE(accessor.valid() == false);
			REQUIRE(lock_counter() == normal_unlock_seq);
			lock_counter().clear();

			accessor = a.get(key);
			REQUIRE(accessor.valid());
			REQUIRE(lock_counter() == normal_lock_seq);
			lock_counter().clear();
			REQUIRE(accessor->second - value == 100);
			accessor.release();
			REQUIRE(accessor.valid() == false);
			REQUIRE(lock_counter() == normal_unlock_seq);
			lock_counter().clear();
		
			v1 = a.size();
			auto it = a.get(100);
			REQUIRE(it.valid());
			it.erase();
			REQUIRE(it.valid() == false);
			REQUIRE(v1 - 1 == a.size());
		}

	}
	REQUIRE(alloc_counter().alloc_count - alloc_counter().dealloc_count == 0);
	REQUIRE(alloc_counter().bucket_alloc - alloc_counter().bucket_dealloc == 0);
}


template<typename T>
struct __finally__
{
	T f;
	__finally__(T a) : f(a) {}
	~__finally__() { f(); }
};

TEST_CASE("test_map_in_multithread", "[Container][Hash][Map][Concurrency]")
{
	using TestMap = MT_Map<std::size_t, std::size_t>;
	TestMap map {};
	Mutex mutex {};
	ConditionVariable cv;

	auto e = [&mutex, &cv, &map]()
	{
		while(signal != 0)
		{
			{
				UniqueLock<Mutex> lock(mutex);
				cv.wait(lock, []{ return signal != 2; });
			}
			{
				for(size_t i = 0; i < iterations_in_thread; ++i)
				{
					size_t key = std::rand() % iterations_in_thread;
					int action = std::rand() % 7;
					if(action == 0)
					{
						map.remove(key);
					}
					else if(action == 1)
					{
						auto it = map.get(key);
						if(it.valid())
						{
							it->second = i;
							it.erase();
						}
					}
					else if(action == 2)
					{
						auto it = map.get_or_create(key);
						if(it.valid())
						{
							it->second = i + 4;
						}
					}
					else if(action == 3)
					{
						map.insert(i, i + 3);
					}
					else if(action == 4)
					{
						map.put(std::pair<const std::size_t, std::size_t>(i, i + 2), true);
					}
					else if(action == 5)
					{
						map.for_each([](std::pair<const std::size_t, std::size_t>& m)
						{
							m.second = m.first + 1;
						});
					}
					else if(action == 6)
					{
						map.clear();
					}
				}
			}
			++thread_sync;
			cv.notify_all();
			{
				UniqueLock<Mutex> lock(mutex);
				cv.wait(lock, []{ return thread_sync == 0; });
			}
		}
	};

	log_alloc = true;
	signal = 2;
	std::vector<Thread> v;
	for(size_t i = 0 ; i < threads_num; ++i)
	{
		v.push_back(Thread(e));
	}
	current_thread::sleep_for(seconds(1));

	__finally__ finally([&v,&cv]()
	{
		signal = 0;
		thread_sync =0;
		cv.notify_all();
		for(auto& t: v)
		{
			t.join();
		}
	});
	signal = 1;
	for(size_t i = 0; i < iterations; ++i)
	{
		thread_sync = 0;
		cv.notify_all();
		{
			UniqueLock<Mutex> lock(mutex);
			cv.wait(lock, []{ return thread_sync == threads_num;});
		}
		// verifiation
		REQUIRE(exceptions == 0);
		REQUIRE(alloc_counter().alloc_count - alloc_counter().dealloc_count == map.size());
		REQUIRE(alloc_counter().bucket_alloc - alloc_counter().bucket_dealloc == map.bucket_size());
		// end of verification
	}
		
}
