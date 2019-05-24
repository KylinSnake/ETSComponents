#########################################################
Instruction on how to work with message and its generator
########################################################

The library uses protobuf to define messages. However, to reduce the interface depedency on protobuf, I write a protobuf plugin to generate a set of wrapper classes working as interfaces in general. In that case, in the future, if we want to replace protobuf with other serilizator, we can keep the exposed interface to other libraries.

To add a new message, there are 3 steps,
	1. code its defnition with proto 3 syntax in .proto files. If a new .proto file is created, please add it into CMakeLists.txt in the library root folder.
	2. Add messge type in MessageType from message_type.proto
	3. Register message into MessageFactory from message_factory.proto

Then compile with cmake and install. The installed files contains header in include/message/ and one dynamic library in lib/. All wrapper class headers and functions are defined in include/message/*.h. 

Note on naming:
	1. all mesasge in protobuf should be in the camel format (e.g: TestMessage)
	2. all fields in the message except MessageFactory should be in lower case combined with underscore (e.g. minimal_message_version)
	3. all enum fields should be in upper case combined with underscore (e.g. DATA_MESSAGE)
	4. The mesasge type should come from the upper case of message name combined by underscore (e.g. "RequestMessage" => "REQUEST_MESSAGE")
	5. when register message into MessageFactory, the field name should be same as message type (e.g "ResponseMessage ResponseMessage = 6")

For naming, examples can be found in current proto files.

We use proto 3 syntax for protobuf. However, we still have following constraints
	1. Protobuf "OneOf" and "extension" are not supported
	2. Protobuf supports map, but only the integer and string can works as key, values cannot be set "repeated", so if the complicated map is required, use 2 repeated message to work around
	3. To support inheritance, all messages should have base type, and its first field should be named as "base" with type of its base one. The root base class is "MessageBase"

