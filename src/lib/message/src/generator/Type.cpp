#include <Type.hpp>
#include <String.h>

namespace snake
{
	namespace message
	{
		namespace __protobuf__
		{
			// Type

			std::string Type::full_proto_ns() const
			{
				if (namespaces_.size() > 0)
					return "::" + snake::core::join(namespaces_,"::");
				return "";
			}

			std::string Type::full_name() const
			{
				std::string p = "";
				if (parents_.size()>0)
				{
					p = snake::core::join(parents_, "::") + "::";
				}
				return p + name_;
			}

			void Type::parse_type(const std::string& name, const FileDescriptor* file, const std::vector<std::string>& parent)
			{
				name_ = name;
				auto n = snake::core::split(file->name(), "/.");
				proto_source_name_ = n[n.size()-2];
				namespaces_ = snake::core::split(file->package(), ".");
				parents_ = parent;
			}

			// Enum

			bool EnumType::parse(const EnumDescriptor* dsp, std::vector<std::string>& parents, OutputHpp& hpp, OutputCpp& cpp)
			{
				parse_type(dsp->name(), dsp->file(), parents);
				int size = dsp->value_count();
				for(int i = 0; i < size; ++i)
				{
					auto v = dsp->value(i);
					fields_[v->number()] = v->name();
				}
				auto fn = full_name();
				hpp.add_global_function_decl("std::string enum_to_string(" + fn +" e);");
				hpp.add_global_function_decl("bool parse_enum(const std::string&, " + fn +"& e);");

				hpp.add_type(This());
				cpp.add_type(This());
				cpp.add_header(name_ + ".h");
				cpp.add_header(proto_source_name_ + ".pb.h");

				return true;
			}

			void EnumType::output(OutputHpp& hpp)
			{
				hpp.print_line("enum " + name_);
				hpp.print_line("{");
				hpp.increment_tab();
				for(auto& p : fields_)
				{
					hpp.print_line(p.second + " = " + std::to_string(p.first) + ",");
				}
				hpp.decrement_tab();
				hpp.print_line("};");
			}

			void EnumType::output(OutputCpp& cpp)
			{
				auto fn = full_name();
				auto pb = full_name_with_proto_ns();

				cpp.print_line("std::string enum_to_string(" + fn + " e)");
				cpp.print_line("{");
				cpp.increment_tab();
				cpp.print_line("return " + pb + "_Name(static_cast<" + pb +">(int(e)));");
				cpp.decrement_tab();
				cpp.print_line("}\n");

				cpp.print_line("bool parse_enum(const std::string& s, " + fn + "& e)");
				cpp.print_line("{");
				cpp.increment_tab();
				cpp.print_line(pb + " v{};");
				cpp.print_line("if(" + pb + "_Parse(s, &v))");
				cpp.print_line("{");
				cpp.increment_tab();
				cpp.print_line("e = static_cast<" + fn + ">(int(v));");
				cpp.print_line("return true;");
				cpp.decrement_tab();
				cpp.print_line("}");
				cpp.print_line("return false;");
				cpp.decrement_tab();
				cpp.print_line("}\n");
			}

			// FieldType

			FieldType::Ptr FieldType::create(const FieldDescriptor* dsp, OutputHpp& hpp, OutputCpp& cpp)
			{
				auto p = std::make_shared<FieldType>();

				p->name_ = dsp->name();
				p->cpp_type_ = dsp->cpp_type_name();
				p->dsp_cpp_type_enum_ = dsp->cpp_type();

				if (p->dsp_cpp_type_enum_ == FieldDescriptor::CppType::CPPTYPE_ENUM)
				{
					hpp.add_header(dsp->enum_type()->name() + ".h");
					p->cpp_type_ = dsp->enum_type()->name();
				}
				else if (p->dsp_cpp_type_enum_ == FieldDescriptor::CppType::CPPTYPE_INT32
					|| p->dsp_cpp_type_enum_ == FieldDescriptor::CppType::CPPTYPE_INT64
					|| p->dsp_cpp_type_enum_ == FieldDescriptor::CppType::CPPTYPE_UINT32
					|| p->dsp_cpp_type_enum_ == FieldDescriptor::CppType::CPPTYPE_UINT64)
				{
					p->cpp_type_ = "std::" + p->cpp_type_ + "_t";
				}
				else if(p->dsp_cpp_type_enum_ == FieldDescriptor::CppType::CPPTYPE_MESSAGE)
				{
					if (dsp->message_type()->full_name() == "google.protobuf.Timestamp")
					{
						hpp.add_header("DateTime.h");
						cpp.add_header("google/protobuf/util/time_util.h");
						p->cpp_type_ = "snake::core::DateTime";
					}
					else
					{
						hpp.add_header(dsp->message_type()->name() + ".h");
						p->cpp_type_ = dsp->message_type()->name();
					}
				}
				else if(p->dsp_cpp_type_enum_ == FieldDescriptor::CppType::CPPTYPE_STRING)
				{
					hpp.add_header("string");
					p->cpp_type_ = "std::string";
				}

				if (dsp->is_repeated())
				{
					p->is_list_ = true;
					hpp.add_header("vector");
				}
				return p;
			}

			void FieldType::output_declaration(OutputHpp& hpp)
			{
				if (is_list_)
				{
					hpp.print_line("std::vector<" + cpp_type_ + "> " + name_ + "{};");
				}
				else
				{
					hpp.print_line(cpp_type_ + " " + name_ + "{};");
				}
			}

			void FieldType::output_deserialize(OutputCpp& cpp, const std::string& proto_var, const std::string& local_var, const std::string& ret_bool)
			{
				if(is_list_)
				{
					output_deserialize_list(cpp, proto_var, local_var, ret_bool);
				}
				else
				{
					output_deserialize_item(cpp, proto_var, local_var, ret_bool);
				}
			}

			void FieldType::output_deserialize_list(OutputCpp& cpp, const std::string& proto_var, const std::string& local_var, const std::string& ret_bool)
			{
				cpp.print_line("for(int i = 0; i < " + proto_var + "." + name_ + "_size(); ++i)");
				cpp.print_line("{");
				cpp.increment_tab();
				if(cpp_type_ == "snake::core::DateTime")
				{
					cpp.print_line(local_var + "." + name_ + ".push_back(Clock::from_time_t(google::protobuf::util::TimeUtil::TimestampToTimeT(" + proto_var + "." + name_ + "(i))));");
				}
				else if(dsp_cpp_type_enum_ == FieldDescriptor::CppType::CPPTYPE_MESSAGE)
				{
					cpp.print_line(cpp_type_ + " t{};");
					cpp.print_line(ret_bool + " = " + ret_bool + " && protobuf_to_" + cpp_type_ + "(" + proto_var + "." + name_ + "(i), t);");
					cpp.print_line(local_var + "." + name_ + ".push_back(t);");
				}
				else if(dsp_cpp_type_enum_ == FieldDescriptor::CppType::CPPTYPE_ENUM)
				{
					cpp.print_line(local_var + "." + name_ + ".push_back(static_cast<" + cpp_type_ + ">(int(" + proto_var + "." + name_ + "(i))));");
				}
				else
				{
					cpp.print_line(local_var + "." + name_ + ".push_back(" + proto_var + "." + name_ + "(i));");
				}
				cpp.decrement_tab();
				cpp.print_line("}");
			}

			void FieldType::output_deserialize_item(OutputCpp& cpp, const std::string& proto_var, const std::string& local_var, const std::string& ret_bool)
			{
				if(cpp_type_ == "snake::core::DateTime")
				{
					cpp.print_line(local_var + "." + name_ + " = Clock::from_time_t(google::protobuf::util::TimeUtil::TimestampToTimeT(" + proto_var + "." + name_ + "()));");
				}
				else if(dsp_cpp_type_enum_ == FieldDescriptor::CppType::CPPTYPE_MESSAGE)
				{
					cpp.print_line(ret_bool + " = " + ret_bool + " && protobuf_to_" + cpp_type_ + "(" + proto_var + "." + name_ + "(), " + local_var + "." + name_ + ");");
				}
				else if(dsp_cpp_type_enum_ == FieldDescriptor::CppType::CPPTYPE_ENUM)
				{
					cpp.print_line(local_var + "." + name_ + " = static_cast<" + cpp_type_ + ">(int(" + proto_var + "." + name_ + "()));");
				}
				else
				{
					cpp.print_line(local_var + "." + name_ + " = " + proto_var + "." + name_ + "();");
				}
			}

			void FieldType::output_serialize(OutputCpp& cpp, const std::string& local_var, const std::string& proto_var, const std::string& ret_bool)
			{
				if(is_list_)
				{
					output_serialize_list(cpp, local_var, proto_var, ret_bool);
				}
				else
				{
					output_serialize_item(cpp, local_var, proto_var, ret_bool);
				}
			}

			void FieldType::output_serialize_list(OutputCpp& cpp, const std::string& local_var, const std::string& proto_var, const std::string& ret_bool)
			{
				cpp.print_line("for(auto& p : " + local_var + "." + name_ + ")");
				cpp.print_line("{");
				cpp.increment_tab();

				if(cpp_type_ == "snake::core::DateTime")
				{
					cpp.print_line("*" + proto_var + ".add_" + name_ + "() = google::protobuf::util::TimeUtil::TimeTToTimestamp(Clock::to_time_t(TimePoint(p)));");
				}
				else if (dsp_cpp_type_enum_ == FieldDescriptor::CppType::CPPTYPE_MESSAGE)
				{
					cpp.print_line(ret_bool + " = " + ret_bool + " && " + cpp_type_ + "_to_protobuf(p, *" + proto_var + ".add_" + name_ +"());");
				}
				else if (dsp_cpp_type_enum_ == FieldDescriptor::CppType::CPPTYPE_ENUM)
				{
					cpp.print_line(proto_var + ".add_" + name_ + "(static_cast<decltype(" + proto_var + "." + name_ + "(0))>(int(p)));");
				}
				else
				{
					cpp.print_line(proto_var + ".add_" + name_ + "(p);");
				}

				cpp.decrement_tab();
				cpp.print_line("}");
			}

			void FieldType::output_serialize_item(OutputCpp& cpp, const std::string& local_var, const std::string& proto_var, const std::string& ret_bool)
			{
				if(cpp_type_ == "snake::core::DateTime")
				{
					cpp.print_line("*" + proto_var + ".mutable_" + name_ + "() = google::protobuf::util::TimeUtil::TimeTToTimestamp(Clock::to_time_t(TimePoint(" + local_var + "." + name_ + ")));");
				}
				else if(dsp_cpp_type_enum_ == FieldDescriptor::CppType::CPPTYPE_MESSAGE)
				{
					cpp.print_line(ret_bool + " = " + ret_bool + " && " + cpp_type_ + "_to_protobuf(" + local_var + "." + name_ + ", *" + proto_var + ".mutable_" + name_ +"());");
				}
				else if (dsp_cpp_type_enum_ == FieldDescriptor::CppType::CPPTYPE_ENUM)
				{
					cpp.print_line(proto_var + ".set_" + name_ + "(static_cast<decltype(" + proto_var + "." + name_ + "())>(int(" + local_var + "." + name_ + ")));");
				}
				else
				{
					cpp.print_line(proto_var + ".set_" + name_ + "(" + local_var + "." + name_ + ");");
				}
			}

			// ClassType

			bool ClassType::parse(const Descriptor* dsp, std::vector<std::string>& parents, OutputHpp& hpp, OutputCpp& cpp)
			{
				parse_type(dsp->name(), dsp->file(), parents);
				int size = dsp->field_count();
				for(int i = 0; i < size; ++i)
				{
					auto v = dsp->field(i);
					if (v->number() == 1 && v->name() == "base")
					{
						base_type_ = v->message_type()->name();
						hpp.add_header(base_type_ + ".h");
						continue;
					}
					auto p = FieldType::create(v, hpp, cpp);
					if (p == nullptr)
					{
						return false;
					}
					fields_[v->number()] = p;
				}
				
				hpp.add_type(This());
				cpp.add_type(This());
				cpp.add_header(name_ + ".h");
				if (base_type_.empty())
				{
					hpp.add_header("MessageType.h");
				}
				cpp.add_global_function_decl("bool " + name_ + "_to_protobuf(const " + name_ + "&," + full_proto_ns() + "::" + name_ + "&);");
				cpp.add_global_function_decl("bool protobuf_to_" + name_ + "(const " + full_proto_ns() + "::" + name_ + "&, " + name_ + "&);");
				cpp.add_header(proto_source_name_ + ".pb.h");

				return true;
			}

			void ClassType::output(OutputHpp& hpp)
			{
				if (base_type_.empty())
				{
					hpp.print_line("struct " + name_);
				}
				else
				{
					hpp.print_line("struct " + name_ + " : public " + base_type_);
				}
				hpp.print_line("{");
				hpp.increment_tab();

				// output default ctor and destructor
				hpp.print_line(name_ + "() = default;");
				hpp.print_line("~" + name_ + "() = default;");
				hpp.print_line();
				
				// output member
				for(auto& p : fields_)
				{
					p.second->output_declaration(hpp);
				}

				// output functions

				if(base_type_.empty())
				{
					hpp.print_line("virtual MessageType type() const;");
				}
				else
				{
					hpp.print_line("virtual MessageType type() const override;");
				}
				hpp.decrement_tab();
				hpp.print_line("};");
			}

			void ClassType::output(OutputCpp& cpp)
			{
				// bool protobuf_to_class()
				cpp.print_line("bool protobuf_to_" + name_ + "(const " + full_proto_ns() + "::" + name_ + "& proto_var, " + name_ + "& local_var)");
				cpp.print_line("{");
				cpp.increment_tab();
				cpp.print_line("bool ret {true};");

				if(!base_type_.empty())
				{
					cpp.print_line("ret = protobuf_to_" + base_type_ + "(proto_var.base(), local_var);");
				}

				for(auto& p : fields_)
				{
					p.second->output_deserialize(cpp, "proto_var", "local_var", "ret");

				}
				
				cpp.print_line("return ret;");
				cpp.decrement_tab();
				cpp.print_line("}");
				cpp.print_line();

				// bool class_to_protobuf()
				cpp.print_line("bool " + name_ + "_to_protobuf(const " + name_ + "& local_var, " + full_proto_ns() + "::" + name_ + "& proto_var)");
				cpp.print_line("{");
				cpp.increment_tab();
				cpp.print_line("bool ret {true};");

				if(!base_type_.empty())
				{
					cpp.print_line("ret = " + base_type_ + "_to_protobuf(local_var, *proto_var.mutable_base());");
				}

				for(auto& p : fields_)
				{
					p.second->output_serialize(cpp, "local_var", "proto_var", "ret");
				}
				
				cpp.print_line("return ret;");
				cpp.decrement_tab();
				cpp.print_line("}");
				cpp.print_line();

				// following are class member functions

				// virtual MessageType type() const
				cpp.print_line("MessageType " + name_ + "::type() const");
				cpp.print_line("{");
				cpp.increment_tab();
				cpp.print_line("return MessageType::" + (base_type_.empty() ? "UNKNOWN" : snake::core::to_upper(snake::core::camel_to_lower_case(name_))) + ";");
				cpp.decrement_tab();
				cpp.print_line("}");
				cpp.print_line();


			}
		}
	}
}
