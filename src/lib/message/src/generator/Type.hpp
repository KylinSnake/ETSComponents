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
				std::map<int, std::string> fields_;
			};

			class FieldType
			{
			public:
				FieldType() = default;
				virtual ~FieldType() = default;

				using Ptr = std::shared_ptr<FieldType>;

				void output_declaration(OutputHpp& hpp);
				void output_serialize(OutputCpp& cpp, const std::string& local_var, const std::string& proto_var, const std::string& ret_bool);
				void output_deserialize(OutputCpp& cpp, const std::string& proto_var, const std::string& local_var, const std::string& ret_bool);

				static Ptr create(const FieldDescriptor* dsp, OutputHpp& hpp, OutputCpp& cpp);
			private:
				void output_serialize_item(OutputCpp& cpp, const std::string& local_var, const std::string& proto_var, const std::string& ret_bool);
				void output_serialize_list(OutputCpp& cpp, const std::string& local_var, const std::string& proto_var, const std::string& ret_bool);
				void output_deserialize_item(OutputCpp& cpp, const std::string& proto_var, const std::string& local_var, const std::string& ret_bool);
				void output_deserialize_list(OutputCpp& cpp, const std::string& proto_var, const std::string& local_var, const std::string& ret_bool);
				
				std::string name_ {};
				std::string cpp_type_ {};
				FieldDescriptor::CppType dsp_cpp_type_enum_ {};
				bool is_list_ {false};
				bool is_map_ {false};
			};

			class ClassType : public TypeT<ClassType>
			{
			public:
				ClassType() = default;
				virtual ~ClassType() = default;

				virtual void output(OutputHpp& hpp) override;
				virtual void output(OutputCpp& cpp) override;

				bool parse(const Descriptor* dsp, std::vector<std::string>& parent_chain, OutputHpp& hpp, OutputCpp& cpp);
			protected:
				std::map<int, FieldType::Ptr> fields_;
				std::string base_type_{};
			};

			class FactoryType : public TypeT<FactoryType>
			{
			public:
				FactoryType() = default;
				virtual ~FactoryType() = default;

				virtual void output(OutputHpp& hpp) override;
				virtual void output(OutputCpp& cpp) override;

				bool parse(const Descriptor* dsp, std::vector<std::string>& parent_chain, OutputHpp& hpp, OutputCpp& cpp);
			protected:
				std::map<std::string, std::string> fields_;
				std::map<int, std::string> types_;
			};
		}
	}
}



#endif
