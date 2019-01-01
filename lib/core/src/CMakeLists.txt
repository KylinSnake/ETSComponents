foreach (folder thread log util services)
	file(GLOB folder_cpp ${folder}/*.cpp)
	file(GLOB folder_h ${folder}/*.h)
	list(APPEND sources ${folder_cpp})
	list(APPEND headers ${folder_h})
	include_directories(${folder})
endforeach(folder)

if (sources)
	add_library(libcore SHARED ${sources})
	target_link_libraries(libcore PUBLIC pthread PUBLIC yaml)
	
	install(TARGETS libcore DESTINATION lib)

	add_executable(core_test test.cpp)
	target_link_libraries(core_test libcore)

endif(sources)

install(FILES ${headers} DESTINATION include/core)
