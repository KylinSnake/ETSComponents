#ifndef SNAKE_MESSAGE_PROTOBUF_TYPE_HPP
#define SNAKE_MESSAGE_PROTOBUF_TYPE_HPP

#include <vector>
#include <string>
#include <set>
#include <memory>

#include <google/protobuf/descriptor.h>

#include <OutputFile.hpp>

using namespace google::protobuf;
using namespace google::protobuf::compiler;

namespace snake
{
	namespace message
	{
		namespace __protobuf__
		{
			class Type : public std::enable_shared_from_this<Type>
			{
			public:
				Type() = default;
				virtual ~Type() = default;

				std::string name() const { return name_;}

				std::string full_name() const;
				std::string full_proto_ns() const;

				std::string full_name_with_proto_ns() const { return full_proto_ns() + "::" + full_name(); }

				virtual void output(OutputHpp& hpp) = 0;
				virtual void output(OutputCpp& cpp) = 0;

				std::shared_ptr<Type> This()
				{
					return shared_from_this();
				}

			protected:
				void parse_type(const std::string& name, const FileDescriptor* file, const std::vector<std::string>& parents);
				std::string name_{};
				std::string proto_source_name_{};
				std::vector<std::string> namespaces_{};
				std::vector<std::string> parents_{};
			};

			template<typename T>
			class TypeT : public Type
			{
			public:
				virtual ~TypeT() = default;
				static std::shared_ptr<T> create()
				{
					return std::make_shared<T>();
				}
			protected:
				TypeT() = default;
			};

			class EnumType : public TypeT<EnumType>
			{
			public:
				EnumType() = default;
				virtual ~EnumType() = default;

				virtual void output(OutputHpp& hpp) override;
				virtual void output(OutputCpp& cpp) override;

				bool parse(const EnumDescriptor* dsp, std::vector<std::string>& parent_chain, OutputHpp& hpp, OutputCpp& cpp); 
			protected:
				std::map<int, std::string> fields;
			};

			class FieldType
			{
			public:
				virtual ~FieldType() = default;

				virtual void output(OutputHpp& hpp);
				virtual void output(OutputCpp& cpp); // as field needs serialize and deserialize, it may need 2 functions here

				static std::shared_ptr<FieldType> create(const FieldDescriptor* dsp, OutputHpp& hpp, OutputCpp& cpp);
			protected:
				virtual bool parse_field(const FieldDescriptor* dsp, OutputHpp& hpp, OutputCpp& cpp) = 0;
				FieldType() = default;
			};
		}
	}
}



#endif
