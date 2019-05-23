add_library(core
	src/Log.cpp
	src/Logger.cpp
	src/Application.cpp
	src/Main.cpp
	src/ServiceFactory.cpp
	src/Service.cpp
	src/Config.cpp
)

target_include_directories(core
	PUBLIC
		$<INSTALL_INTERFACE:include/core/>
		$<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
	PRIVATE
		${CMAKE_CURRENT_SOURCE_DIR}/src)
target_link_libraries(core util concurrency ${CMAKE_DL_LIBS} yaml-cpp)

install(TARGETS core
		EXPORT snakes-exports
		LIBRARY DESTINATION lib
		ARCHIVE DESTINATION lib)
install(DIRECTORY include/ DESTINATION ${CMAKE_INSTALL_PREFIX}/include/core/)


add_library(test_service SHARED test/fake_service.cpp)
target_link_libraries(test_service core)

add_executable(core_test 
	test/main.cpp 
	test/services_test.cpp
)
target_include_directories(core_test PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/src)
target_link_libraries(core_test core tp::catch2)
add_test(NAME CoreTest COMMAND core_test)
