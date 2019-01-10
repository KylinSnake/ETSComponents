
## Add Yaml cpp
add_library(tp::yaml SHARED IMPORTED)
set_target_properties(tp::yaml PROPERTIES
	IMPORTED_LOCATION "${tp_root}/lib/libyaml-cpp.so"
    INTERFACE_INCLUDE_DIRECTORIES "${tp_root}/include/yaml-cpp;${tp_root}/include" 
)

## Catch2
add_library(tp::catch2 INTERFACE IMPORTED)
set_target_properties(tp::catch2 PROPERTIES
	INTERFACE_INCLUDE_DIRECTORIES "${tp_root}/include/catch2")
