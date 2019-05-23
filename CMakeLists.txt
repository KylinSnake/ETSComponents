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

include(build_env)
include(tp)

set(PUBLIC_HEADER_FOLDER ${PROJECT_SOURCE_DIR}/include)
set(INSTALL_HEADER_FOLDER ${CMAKE_INSTALL_PREFIX}/include)

add_subdirectory(src/lib/core)
add_subdirectory(src/lib/message)
add_subdirectory(src/lib/concurrency)
add_subdirectory(src/lib/util)

add_subdirectory(src/app)

install(DIRECTORY ${PUBLIC_HEADER_FOLDER}/ DESTINATION ${INSTALL_HEADER_FOLDER})
