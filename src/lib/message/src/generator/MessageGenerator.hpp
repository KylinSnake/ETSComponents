#ifndef SNAKE_MESSAGE_PROTOBUF_MESSAGE_GENERATOR_H
#define SNAKE_MESSAGE_PROTOBUF_MESSAGE_GENERATOR_H

#include <google/protobuf/compiler/cpp/cpp_generator.h>

using namespace google::protobuf::compiler::cpp;
using namespace google::protobuf::compiler;
using namespace google::protobuf;

namespace snake
{
	namespace message
	{
		namespace __protobuf__
		{
			class MessageGenerator : public CppGenerator
			{
			public:
				using Base=CppGenerator;
				MessageGenerator() = default;
				virtual ~MessageGenerator() = default;
				
				virtual bool Generate(const FileDescriptor * file,
					const string & parameter, GeneratorContext * generator_context, 
					string * error) const override;

				virtual bool GenerateAll(
					const std::vector< const FileDescriptor * > & files,
					const string & parameter,
					GeneratorContext * generator_context,
					string * error) const override
					{
						proto_file_number = files.size();
						return Base::GenerateAll(files, parameter, generator_context, error);
					}
			private:
				mutable size_t proto_file_number = 0;
				mutable size_t current_file_number = 0;
			};
		}
	}
}

#endif
