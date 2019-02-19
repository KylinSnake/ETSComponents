#include <Type.hpp>
#include <String.h>

namespace snake
{
	namespace message
	{
		namespace __protobuf__
		{
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

			bool EnumType::parse(const EnumDescriptor* dsp, std::vector<std::string>& parents, OutputHpp& hpp, OutputCpp& cpp)
			{
				parse_type(dsp->name(), dsp->file(), parents);
				int size = dsp->value_count();
				for(int i = 0; i < size; ++i)
				{
					auto v = dsp->value(i);
					fields[v->number()] = v->name();
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
				for(auto& p : fields)
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
		}
	}
}
