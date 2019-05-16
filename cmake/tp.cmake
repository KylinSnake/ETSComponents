set($ENV{PKG_CONFIG_PATH} ${tp_root}/lib/pkgconfig)
find_package(PkgConfig REQUIRED)

# pThread
find_package(Threads REQUIRED)
if (Threads_FOUND)
	message(STATUS "Found Threads")
else()
	message(FATAL "No Threads found")
endif()

## Add Yaml cpp
find_package(yaml-cpp REQUIRED)
if (yaml-cpp_FOUND)
	message(STATUS "Found Yaml-cpp")
else()
	message(FATAL "No Yaml-cpp found")
endif()


## Catch2
add_library(tp::catch2 INTERFACE IMPORTED)
set_target_properties(tp::catch2 PROPERTIES
	INTERFACE_INCLUDE_DIRECTORIES "${tp_root}/include/catch2")

## Protobuf
pkg_check_modules(PROTOBUF REQUIRED protobuf)
add_library(tp::protobuf SHARED IMPORTED)
set_target_properties(tp::protobuf PROPERTIES
	IMPORTED_LOCATION ${PROTOBUF_LINK_LIBRARIES}
	INTERFACE_INCLUDE_DIRECTORIES ${PROTOBUF_INCLUDE_DIRS}
)

## leveldb
find_package(leveldb REQUIRED)
if (leveldb_FOUND)
	message(STATUS "Found LevelDb")
else()
	message(FATAL "No LevelDb found")
endif()

