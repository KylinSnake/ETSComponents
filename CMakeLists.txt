cmake_minimum_required(VERSION 2.6)

project(snake C CXX)

enable_testing()

if(NOT CMAKE_BUILD_TYPE)
  set(CMAKE_BUILD_TYPE "Release")
endif()

set(CXX_FLAGS
 -g
 -DCHECK_PTHREAD_RETURN_VALUE
 -D_FILE_OFFSET_BITS=64
 -Wall
 -Wextra
 -Werror
 -Wno-unused-parameter
 -Woverloaded-virtual
 -Wpointer-arith
 -Wwrite-strings
 -march=native
 -rdynamic
 )

if(CMAKE_BUILD_BITS EQUAL 32)
  list(APPEND CXX_FLAGS "-m32")
endif()

string(REPLACE ";" " " CMAKE_CXX_FLAGS "${CXX_FLAGS}")

set(CMAKE_CXX_COMPILER "g++")
set(CMAKE_CXX_FLAGS_DEBUG "-O0")
set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} -O2 -finline-limit=1000 -DNDEBUG")
set(EXECUTABLE_OUTPUT_PATH ${PROJECT_BINARY_DIR}/bin)
set(LIBRARY_OUTPUT_PATH ${PROJECT_BINARY_DIR}/lib)

find_package(Boost 1.61.0)
if(Boost_FOUND)
  include_directories()
	message(STATUS "found boost libs in TP, INC=${Boost_INCLUDE_DIRS} LIB=${Boost_LIBRARY_DIRS}")
else()
	message(FATAL_ERROR "Cannot find boost libs, exit")
endif()

if(PROJ_BUILD_DIR)
  message(STATUS "Install folder is ${PROJ_BUILD_DIR}/${PROJECT_NAME}")
  set(CMAKE_INSTALL_PREFIX "${PROJ_BUILD_DIR}/${PROJECT_NAME}")
else()
  message(WARNING "Cannot find install dir, use ./install")
  set(CMAKE_INSTALL_PREFIX "${PROJECT_SOURCE_DIR}/install")
endif()

include_directories(${BOOST_INC})
include_directories(${PROJECT_SOURCE_DIR})

link_directories($ENV{LD_LIBRARY_PATH})
link_directories(${Boost_LIBRARY_DIRS})

string(TOUPPER ${CMAKE_BUILD_TYPE} BUILD_TYPE)
message(STATUS "CXX_FLAGS = " ${CMAKE_CXX_FLAGS} " " ${CMAKE_CXX_FLAGS_${BUILD_TYPE}})

add_subdirectory(lib/core)
