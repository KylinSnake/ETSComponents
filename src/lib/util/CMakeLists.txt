add_library(util
	src/DateTime.cpp
)

target_include_directories(util
	PUBLIC
		$<INSTALL_INTERFACE:include/util/>
		$<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
	PRIVATE
		${CMAKE_CURRENT_SOURCE_DIR}/src)

install(TARGETS util
		EXPORT snakes-exports
		LIBRARY DESTINATION lib
		ARCHIVE DESTINATION lib)
install(DIRECTORY include/ DESTINATION ${CMAKE_INSTALL_PREFIX}/include/util/)

add_executable(util_test 
	test/main.cpp 
	test/util_test.cpp 
)
target_include_directories(util_test PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/src)
target_link_libraries(util_test util tp::catch2)
add_test(NAME UtilTest COMMAND util_test)
