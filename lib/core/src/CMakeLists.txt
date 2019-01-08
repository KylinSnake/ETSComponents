foreach (folder thread log util services)
	file(GLOB folder_cpp ${folder}/*.cpp)
	file(GLOB folder_h ${folder}/*.h)
	list(APPEND sources ${folder_cpp})
	list(APPEND headers ${folder_h})
	list(APPEND folders ${CMAKE_CURRENT_SOURCE_DIR}/${folder})
endforeach(folder)

if (sources)
	include_directories(${folders})
	add_library(core SHARED ${sources})
	target_link_libraries(core pthread dl yaml)

	set_property(TARGET core PROPERTY "include_path" ${folders})
	
	install(TARGETS core DESTINATION lib)

endif(sources)

install(FILES ${headers} DESTINATION include/core)
