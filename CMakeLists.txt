cmake_minimum_required(VERSION 3.5)

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

include(build_env)
include(tp)

add_subdirectory(lib/core/src)

add_subdirectory(lib/core/test)
