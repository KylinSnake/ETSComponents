cmake_minimum_required(VERSION 3.11)

project(snake C CXX)

enable_testing()
include(CTest)

set(CMAKE_MODULE_PATH "${PROJECT_SOURCE_DIR}/cmake")

if( DEFINED ENV{TP_PATH} )
	set(tp_root $ENV{TP_PATH})
	message(STATUS "TP Root is ${tp_root}")
else()
	message(FATAL_ERROR "No env TP_PATH")
endif()

option(TEST_COVERAGE "Enable test coverage" off)
if(TEST_COVERAGE)
	message(STATUS "Enable Test Coverage")
endif()

include(build_env)
include(tp)
include(functions)


set(PUBLIC_HEADER_FOLDER ${PROJECT_SOURCE_DIR}/include)
set(INSTALL_HEADER_FOLDER ${CMAKE_INSTALL_PREFIX}/include)

add_subdirectory(lib/core)
add_subdirectory(lib/message)
add_subdirectory(lib/concurrency)
add_subdirectory(lib/util)
add_subdirectory(lib/db)

add_subdirectory(app)

install(DIRECTORY ${PUBLIC_HEADER_FOLDER}/ DESTINATION ${INSTALL_HEADER_FOLDER})
