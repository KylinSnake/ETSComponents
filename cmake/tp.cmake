## Add Yaml cpp
add_library(yaml SHARED IMPORTED)
set_target_properties(yaml PROPERTIES
	IMPORTED_LOCATION "${tp_root}/lib/libyaml-cpp.so"
    INTERFACE_INCLUDE_DIRECTORIES "${tp_root}/include/yaml-cpp;${tp_root}/include" 
)
