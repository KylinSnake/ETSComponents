add_executable(message_generator
	src/generator/MessageGenerator.cpp
	src/generator/Type.cpp
	src/generator/OutputFile.cpp
)

target_include_directories(message_generator
	PRIVATE
		${CMAKE_CURRENT_SOURCE_DIR}/src/generator
)
target_link_libraries(message_generator core tp::protobuf ${PROTOBUF_LIBRARY_DIRS}/libprotoc.so)

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
		$<INSTALL_INTERFACE:include/message/>
		$<BUILD_INTERFACE:${CMAKE_CURRENT_BINARY_DIR}/include/>
	PRIVATE
		${CDIR}/
)

target_link_libraries(message core tp::protobuf)
install(TARGETS message
		EXPORT snakes-exports
		LIBRARY DESTINATION lib
		ARCHIVE DESTINATION lib)
install(DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}/include/ DESTINATION ${CMAKE_INSTALL_PREFIX}/include/message/)


add_executable(message_test
	test/main.cpp
	test/base_test.cpp
)
message(STATUS "${CDIR}/include/")
target_include_directories(message_test PRIVATE ${CDIR}/)
target_link_libraries(message_test message tp::catch2)
add_test(NAME MessageTest COMMAND message_test)
