snake_lib(concurrency src/Mutex.cpp)
snake_link(concurrency PUBLIC Threads::Threads)

snake_test(concurrency)
snake_test_link(concurrency)
