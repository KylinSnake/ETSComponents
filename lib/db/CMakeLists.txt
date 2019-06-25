snake_lib(db src/KeyValueLocalDb.cpp)
snake_link(db PRIVATE leveldb::leveldb)

snake_test(db
	test/key_value_db_test.cpp
)
snake_test_link(db PRIVATE util PRIVATE concurrency)
