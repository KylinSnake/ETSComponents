add_executable(message_generator
	src/generator/MessageGenerator.cpp
	src/generator/Type.cpp
	src/generator/OutputFile.cpp
)

target_include_directories(message_generator
	PRIVATE
		${CMAKE_CURRENT_SOURCE_DIR}/src/generator
)
target_link_libraries(message_generator util Threads::Threads tp::protobuf ${PROTOBUF_LIBRARY_DIRS}/libprotoc.so)

set(SRC 
	base 
	message_type
	message_factory
)


set(PDIR ${CMAKE_CURRENT_SOURCE_DIR}/src/proto)
set(CDIR ${CMAKE_CURRENT_BINARY_DIR}/)
foreach(V ${SRC})
	set (PSRC "${PDIR}/${V}.proto" ${PSRC})
	set (CSRC "${CDIR}/${V}.pb.cc" ${CSRC})
endforeach(V)

add_custom_command(
	OUTPUT ${CDIR}/generated.cpp ${CSRC}
	COMMAND protoc --plugin=protoc-gen-message_generator=${CMAKE_CURRENT_BINARY_DIR}/message_generator --message_generator_out=${CDIR} --proto_path=${PDIR} ${PSRC}
	DEPENDS message_generator ${PSRC}
)

add_compile_definitions(
	MESSAGE_MAJOR_VERSION=0
	MESSAGE_MINOR_VERSION=1
	MESSAGE_MINIMAL_MAJOR_VERSION=0
	MESSAGE_MINIMAL_MINOR_VERSION=1
)
	
add_library(message ${CDIR}/generated.cpp ${CSRC})
target_include_directories(message
	PUBLIC
		$<INSTALL_INTERFACE:${INSTALL_HEADER_FOLDER}>
		$<BUILD_INTERFACE:${CMAKE_CURRENT_BINARY_DIR}/include/>
	PRIVATE
		${CDIR}/
)

target_link_libraries(message util tp::protobuf Threads::Threads)

if(TEST_COVERAGE)
	target_compile_options(message PRIVATE "--coverage")
	target_link_options(message PRIVATE "--coverage")
endif()

install(TARGETS message
		EXPORT snakes-exports
		LIBRARY DESTINATION lib
		ARCHIVE DESTINATION lib)
install(DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}/include/ DESTINATION ${INSTALL_HEADER_FOLDER}/)


add_executable(message_test
	$<TARGET_OBJECTS:test_main>
	test/base_test.cpp
)
target_include_directories(message_test PRIVATE ${CDIR}/)
target_link_libraries(message_test message tp::catch2)
add_test(NAME message_test COMMAND message_test)
if(TEST_COVERAGE)
	target_compile_options(message_test PRIVATE "--coverage")
	target_link_options(message_test PRIVATE "--coverage")
endif()
