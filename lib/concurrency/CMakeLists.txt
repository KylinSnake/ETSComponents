add_library(concurrency
	src/Mutex.cpp
)

target_include_directories(concurrency
	PUBLIC
		$<INSTALL_INTERFACE:${INSTALL_HEADER_FOLDER}>
		$<BUILD_INTERFACE:${PUBLIC_HEADER_FOLDER}>
	PRIVATE
		${CMAKE_CURRENT_SOURCE_DIR}/src)
target_link_libraries(concurrency Threads::Threads)

install(TARGETS concurrency
		EXPORT snakes-exports
		LIBRARY DESTINATION lib
		ARCHIVE DESTINATION lib)


add_executable(concurrency_test 
	$<TARGET_OBJECTS:test_main>
)
target_include_directories(concurrency_test PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/src)
target_link_libraries(concurrency_test concurrency tp::catch2)
add_test(NAME ConcurrencyTest COMMAND concurrency_test)
