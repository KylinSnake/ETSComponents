add_executable(message_generator
	src/generator/MessageGenerator.cpp
	src/generator/Type.cpp
	src/generator/OutputFile.cpp
)

# protoc example with plugin
# protoc --plugin=protoc-gen-message_generator=/home/ec2-user/jupyter/workstation/snake/build/Debug64/src/lib/message/message_generator --message_generator_out=/tmp --proto_path=./src/proto ./src/proto/base.proto

target_include_directories(message_generator
	PRIVATE
		${CMAKE_CURRENT_SOURCE_DIR}/src/generator
)
target_link_libraries(message_generator core tp::protobuf ${PROTOBUF_LIBRARY_DIRS}/libprotoc.so)

set(SRC base
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

add_library(message ${CDIR}/generated.cpp ${CSRC})
target_include_directories(message
	PRIVATE
		${CDIR} ${CDIR}/include
)

target_link_libraries(message core tp::protobuf)
