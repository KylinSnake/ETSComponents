add_library(util
	src/DateTime.cpp
)

target_include_directories(util
	PUBLIC
		$<INSTALL_INTERFACE:${INSTALL_HEADER_FOLDER}>
		$<BUILD_INTERFACE:${PUBLIC_HEADER_FOLDER}>
	PRIVATE
		${CMAKE_CURRENT_SOURCE_DIR}/src)

install(TARGETS util
		EXPORT snakes-exports
		LIBRARY DESTINATION lib
		ARCHIVE DESTINATION lib)

add_executable(util_test 
	$<TARGET_OBJECTS:test_main>
	test/util_test.cpp 
)
target_include_directories(util_test PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/src)
target_link_libraries(util_test util tp::catch2)
add_test(NAME UtilTest COMMAND util_test)
