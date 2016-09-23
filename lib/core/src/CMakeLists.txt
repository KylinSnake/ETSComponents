FOREACH (folder thread log util)
	FILE(GLOB folder_cpp ${folder}/*.cpp)
	FILE(GLOB folder_h ${folder}/*.h)
	LIST(APPEND sources ${folder_cpp})
	LIST(APPEND headers ${folder_h})
	INCLUDE_DIRECTORIES(${folder})
ENDFOREACH(folder)

IF (sources)
	ADD_LIBRARY(core_static STATIC ${sources})
	ADD_LIBRARY(core_shared SHARED ${sources})
	TARGET_LINK_LIBRARIES(core_static pthread)
	TARGET_LINK_LIBRARIES(core_shared pthread)
	SET_TARGET_PROPERTIES(core_static core_shared PROPERTIES OUTPUT_NAME core)
	
	INSTALL(TARGETS core_static DESTINATION lib)
	INSTALL(TARGETS core_shared DESTINATION lib)
ENDIF(sources)

INSTALL(FILES ${headers} DESTINATION include/core)

ADD_EXECUTABLE(MyTest test.cpp)
IF (sources)
	ADD_DEPENDENCIES(MyTest core_shared)
ENDIF (sources)
TARGET_LINK_LIBRARIES(MyTest pthread)
INSTALL(TARGETS MyTest DESTINATION bin)

