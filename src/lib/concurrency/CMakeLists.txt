add_library(concurrency
	src/Mutex.cpp
)

target_include_directories(concurrency
	PUBLIC
		$<INSTALL_INTERFACE:include/concurrency/>
		$<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
	PRIVATE
		${CMAKE_CURRENT_SOURCE_DIR}/src)
target_link_libraries(concurrency Threads::Threads)

install(TARGETS concurrency
		EXPORT snakes-exports
		LIBRARY DESTINATION lib
		ARCHIVE DESTINATION lib)
install(DIRECTORY include/ DESTINATION ${CMAKE_INSTALL_PREFIX}/include/concurrency/)


add_executable(concurrency_test 
	test/main.cpp 
)
target_include_directories(concurrency_test PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/src)
target_link_libraries(concurrency_test concurrency tp::catch2)
add_test(NAME ConcurrencyTest COMMAND concurrency_test)
