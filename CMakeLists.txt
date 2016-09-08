cmake_minimum_required(VERSION 2.6)

project(snake C CXX)

enable_testing()

if(NOT CMAKE_BUILD_TYPE)
  set(CMAKE_BUILD_TYPE "Release")
endif()

if( CMAKE_SIZEOF_VOID_P EQUAL 8 )
  MESSAGE( "64 bits compiler detected" )
  SET( EX_PLATFORM 64 )
  SET( EX_PLATFORM_NAME "x64" )
else( CMAKE_SIZEOF_VOID_P EQUAL 8 ) 
  MESSAGE( "32 bits compiler detected" )
  SET( EX_PLATFORM 32 )
  SET( EX_PLATFORM_NAME "x86" )
endif( CMAKE_SIZEOF_VOID_P EQUAL 8 )


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

if(EX_PLATFORM EQUAL 32)
  list(APPEND CXX_FLAGS "-m32")
endif()

string(REPLACE ";" " " CMAKE_CXX_FLAGS "${CXX_FLAGS}")

include(CheckCXXCompilerFlag)
check_cxx_compiler_flag("-std=c++11" COMPILER_SUPPORTS_CXX11)
if(COMPILER_SUPPORTS_CXX11)
	set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11")
else()
	message(STATUS "The compiler ${CMAKE_CXX_COMPILER} has no C++11 support. Please use a different C++ compiler.")
endif()


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

set(INSTALL_DIR "install/${CMAKE_BUILD_TYPE}${EX_PLATFORM}")
if(PROJ_BUILD_DIR)
  message(STATUS "Install folder is ${PROJ_BUILD_DIR}/${INSTALL_DIR}")
  set(CMAKE_INSTALL_PREFIX "${PROJ_BUILD_DIR}/${INSTALL_DIR}")
else()
  message(WARNING "Cannot find install dir, use ./${INSTALL_DIR}")
  set(CMAKE_INSTALL_PREFIX "${PROJECT_SOURCE_DIR}/${INSTALL_DIR}")
endif()

include_directories(${BOOST_INC})
include_directories(${PROJECT_SOURCE_DIR})

link_directories($ENV{LD_LIBRARY_PATH})
link_directories(${Boost_LIBRARY_DIRS})

string(TOUPPER ${CMAKE_BUILD_TYPE} BUILD_TYPE)
message(STATUS "CXX_FLAGS = " ${CMAKE_CXX_FLAGS} " " ${CMAKE_CXX_FLAGS_${BUILD_TYPE}})

add_subdirectory(lib/core)
