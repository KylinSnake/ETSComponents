set($ENV{PKG_CONFIG_PATH} ${tp_root}/lib/pkgconfig)
find_package(PkgConfig REQUIRED)

## Add Yaml cpp
pkg_check_modules(YAML REQUIRED yaml-cpp)
add_library(tp::yaml SHARED IMPORTED)
set_target_properties(tp::yaml PROPERTIES
	IMPORTED_LOCATION ${YAML_LINK_LIBRARIES}
    INTERFACE_INCLUDE_DIRECTORIES ${YAML_INCLUDE_DIRS}
)

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

