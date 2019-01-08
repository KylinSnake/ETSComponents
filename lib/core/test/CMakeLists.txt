file(GLOB sources 
	main.cpp
	unit_test.cpp
	services_test.cpp
)

get_target_property(folders core "include_path")
include_directories(${folders})
if (sources)
	add_library(test_service SHARED fake_service.cpp)
	target_link_libraries(test_service core catch2)

	add_executable(core_test ${sources})
	add_dependencies(core_test core)
	target_link_libraries(core_test catch2 core)
	add_test(NAME CoreTest COMMAND core_test)

endif(sources)

