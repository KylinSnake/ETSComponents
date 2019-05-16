#include <MessageGenerator.hpp>

#include <google/protobuf/compiler/plugin.h>
#include <google/protobuf/descriptor.h>

#include <map>
#include <string>
#include <vector>

#include <fstream>

#include <String.h>
#include <Type.hpp>
#include <OutputFile.hpp>

using namespace google::protobuf::compiler;

namespace snake
{
	namespace message
	{
		namespace __protobuf__
		{
			std::map<std::string, std::string> Types = {
				{"double", "double"},
				{"float", "float"},
				{"int32", "std::int32_t"},
				{"int64", "std::int64_t"},
				{"uint32", "std::uint32_t"},
				{"uint64", "std::uint64_t"},
				{"sint32", "std::int32_t"},
				{"sint64", "std::int64_t"},
				{"fixed32", "std::uint32_t"},
				{"fixed64", "std::uint64_t"},
				{"sfixed32", "std::int32_t"},
				{"sfixed64", "std::int64_t"},
				{"bool", "bool"},
				{"string", "std::string"},
				{"byte", "std::string"},
				{"google.protobuf.Timestamp", "DateTime"}
			};

			struct enum_type
			{
				std::string name {};
				std::map<int, std::string> fields{};
				std::vector<std::string> parent_chain{};
				std::string file_name {};
				std::string package {};

				std::string to_cpp(const std::string& tab = "\t", int tab_num = 0, bool include_namespace = true) const
				{
					std::string ret = "";
					std::string prefix = "";
					for(int i = 0; i < tab_num; ++i)
					{
						prefix += tab;
					}
					auto dp = prefix;
					std::string proto_namespace = "";
					
					if(include_namespace)
					{
						auto n = snake::core::split(file_name, "/.");
						ret += "#include<" + name +".h>\n#include<" + n[n.size() - 2] + ".pb.h>\n\n";
						if(package.size() > 0)
						{
							proto_namespace = snake::core::join(snake::core::split(package,"."), "::") + "::";
						}

						ret += "namespace snake { namespace message {\n\n";
						dp += tab;
						tab_num +=1;
					}

					std::string parent_class = "";
					if (parent_chain.size() > 0)
					{
						parent_class = snake::core::join(parent_chain, "::") + "::";
					}
					auto class_name = parent_class + name;

					ret += dp + "std::string " + parent_class + "enum_to_string(" + class_name + " e)\n";
					ret += dp + "{\n";
					ret += dp + dp + "return " + proto_namespace + name + "_name(int(e));\n"; 
					ret += dp + "}\n\n";
					ret += dp + "bool " + parent_class + "parse_enum(const std::string& s, " + class_name +"& e)\n";
					ret += dp + "{\n";
					ret += dp + dp + proto_namespace + class_name + " v{};\n";
					ret += dp + dp + "if(" + proto_namespace + name +"_parse(s, &v))\n";
					ret += dp + dp + "{\n";
					ret += dp + dp + dp + "e = static_cast<" + class_name + ">(int(v));\n";
					ret += dp + dp + dp + "return true;\n";
					ret += dp + dp + "}\n";
					ret += dp + dp + "return false;\n";
					ret += dp + "}\n";

					if(include_namespace)
					{
						ret += "} }";
					}

					return ret;
				}

				std::string to_header(const std::string& tab = "\t", int tab_num = 0, bool include_namespace = true) const
				{
					std::string ret = "";
					std::string prefix = "";
					for(int i = 0; i < tab_num; ++i)
					{
						prefix += tab;
					}
					auto dp = prefix;
					auto upper_name = snake::core::to_upper(name);

					if(include_namespace)
					{
						ret +="#ifndef SNAKE_MESSAGE_" + upper_name + "_H\n#define SNAKE_MESSAGE_" + upper_name+ "_H\n\n";
						ret += "namespace snake { namespace message {\n\n";
						dp += tab;
						tab_num +=1;
					}

					ret += dp + "enum " + name + "\n" + dp + "{\n";
					int i = 0;
					int size = fields.size();
					for (auto& p : fields)
					{
						ret += dp + dp + p.second + " = " + std::to_string(p.first);
						++i;
						if(i < size)
						{
							ret += ",\n";
						}
						else
						{
							ret += "\n";
						}
					}

					ret += dp + "};\n\n";

					auto deco = parent_chain.size() > 0 ? "static " : "";
					ret += dp + deco + "std::string enum_to_string(" + name + ");\n";
					ret += dp + deco + "bool parse_enum(const std::string&, " + name + "&);\n";
					ret += "\n";
					if(include_namespace)
					{
						ret +="} }\n#endif\n";
					}
					return ret;
				}
			};

			enum_type extract_enum_type(const EnumDescriptor* edsp, const std::string& file_name, const std::string& package, const std::vector<std::string>& parent_chain)
			{
				enum_type ret;
				ret.file_name = file_name;
				ret.package = package;
				ret.parent_chain = parent_chain;
				ret.name = edsp->name();
				int size = edsp->value_count();
				for(int i = 0; i < size; ++i)
				{
					auto vdsp = edsp->value(i);
					ret.fields[vdsp->number()] = vdsp->name();
				}
				return ret;
			}

			struct field_type
			{
				std::string name{};
				std::string type {};
				bool is_list { false };
				std::string to_header(const std::string& prefix= "\t") const
				{
					if(is_list)
					{
						return prefix + "std::vector<" + type + "> " + name +"{};\n";
					}
					return prefix + type + " " + name + "{};\n";
				}
			};

			struct message_type
			{
				std::string name{};
				std::string base{};
				std::string base_name {};
				std::vector<field_type> fields;
				std::string package{};
				std::string file_name {};
				std::vector<message_type> nested_types{};
				std::vector<std::string> parent_chain{};

				std::string to_header(const std::set<std::string>& headers, const std::string& tab = "\t", int tab_num = 0, bool include_namespace = true) const
				{
					auto upper_name = snake::core::to_upper(name);
					std::string ret = "";
					std::string prefix = "";
					for (int i = 0; i < tab_num; ++i)
					{
						prefix += tab;
					}
					auto dp = prefix;
					
					if(include_namespace)
					{
						ret +="#ifndef SNAKE_MESSAGE_" + upper_name + "_H\n#define SNAKE_MESSAGE_" + upper_name+ "_H\n\n";
						for(auto& s : headers)
						{
							ret +="#include <" + s + ">\n";
						}

						ret += "\nnamespace snake { namespace message { \n\n";
						dp += tab;
						tab_num +=1;
					}

					ret += dp + "struct " + name;
					if (base_name.size() > 0)
					{
						ret += " : public " + base_name + "\n" + dp + "{\n";
					}
					else
					{
						ret += "\n" + dp + "{\n";
					}
					tab_num +=1;
					ret += dp + tab + name +"() = default;\n" + dp + tab + "~" + name + "() = default;\n\n";

					std::set<std::string> dummpy;
					for(auto& e : nested_types)
					{
						ret += e.to_header(dummpy, dp + tab, tab_num, false);
					}

					if(nested_types.size() > 0)
					{
						ret += "\n";
					}

					for(auto& f : fields)
					{
						ret += f.to_header(dp + tab);
					}
					ret += dp + "};\n";

					if(include_namespace)
					{
						ret += "} }\n#endif\n";
					}
					return ret;
				}
			};

			message_type extract_message_type(const Descriptor* dsp, const std::string& file_name, const std::string& package, std::set<std::string>& external_headers)
			{
				message_type msg_t;
				msg_t.file_name = file_name;
				msg_t.package= package;
				msg_t.name = dsp->name();
				int field_count = dsp->field_count();
				for(int i = 0; i < field_count; ++i)
				{
					auto field = dsp->field(i);
					auto name = field->name();
					std::string type = field->type_name();
					if (field->type() == FieldDescriptor::Type::TYPE_MESSAGE)
					{
						type = field->message_type()->full_name();
					}
					if(Types.count(type) > 0)
					{
						type = Types[type];
					}


					if (name == "base")
					{
						msg_t.base = type;
						auto a = snake::core::split(msg_t.base, ".");
						msg_t.base_name = a[a.size()-1];
						type = msg_t.base_name;
					}
					else
					{
						field_type f;
						f.name = name;
						f.type = type;
						f.is_list = field->is_repeated();
						msg_t.fields.push_back(f);
					}

					if(type == "std::string")
					{
						external_headers.emplace("string");
					}
					else if(type == "DateTime")
					{
						external_headers.emplace("DateTime.h");
					}
					else if(field->type() == FieldDescriptor::Type::TYPE_MESSAGE)
					{
						external_headers.emplace(type + ".h");
					}
				}

				field_count = dsp->nested_type_count();
				for(int i = 0; i < field_count; ++i)
				{
					auto nest_msg = extract_message_type(dsp->nested_type(i), file_name, package, external_headers);
					msg_t.nested_types.push_back(nest_msg);
				}

				return msg_t;
			}


			bool MessageGenerator::Generate(const FileDescriptor * file, const string & parameter, 
					GeneratorContext * generator_context, string * error) const 
			{
				std::vector<std::string> parent;
				std::vector<std::shared_ptr<OutputHpp>> hpps;
				
				auto cpp = std::make_shared<OutputCpp>();
				cpp->set_file_name("generated.cpp");

				// extract enum type
				int enum_count = file->enum_type_count();
				for(int i = 0 ; i < enum_count; ++i)
				{
					auto dsp = file->enum_type(i);
					auto hpp = std::make_shared<OutputHpp>();
					auto enm = EnumType::create();
					if(enm->parse(dsp, parent, *hpp, *cpp))
					{
						hpp->set_file_name(enm->name() + ".h");
						hpp->set_path("include");
						hpps.push_back(hpp);
					}
				}

				// extract class type
				int class_count = file->message_type_count();
				for(int i = 0; i < class_count; ++i)
				{
					auto dsp = file->message_type(i);
					if (dsp->name() == "MessageFactory")
					{
						continue;
					}
					auto hpp = std::make_shared<OutputHpp>();
					auto cls = ClassType::create();
					if(cls->parse(dsp, parent, *hpp, *cpp))
					{
						hpp->set_file_name(cls->name() + ".h");
						hpp->set_path("include");
						hpps.push_back(hpp);
					}
					else
					{
						throw std::runtime_error("Error on parse: " + dsp->name());
					}
				}


				// output generated files
				cpp->output(generator_context);
				for(auto& i : hpps)
				{
					i->output(generator_context);
				}

				return Base::Generate(file, parameter, generator_context, error);
/*
				// extract message_type
				int message_type_count = file->message_type_count();
				std::vector<message_type> messages;
				std::vector<std::set<std::string>> headers;
				headers.resize(message_type_count);
				for(int i = 0; i < message_type_count; ++i)
				{
					auto dsp = file->message_type(i);
					auto msg = extract_message_type(dsp, file->name(), file->package(), headers[i]);
					messages.push_back(msg);
				}

				// extract enum type
				std::vector<enum_type> enums;
				int enum_count = file->enum_type_count();
				for(int i = 0 ; i < enum_count; ++i)
				{
					auto dsp = file->enum_type(i);
					auto enm = extract_enum_type(dsp, file->name(), file->package(), empty_parent);
					enums.push_back(enm);
				}
				// Test code
				for(int i = 0; i < message_type_count; ++i)
				{
					std::ofstream f("/tmp/test/" + messages[i].name +".h");
					f<<messages[i].to_header(headers[i]);
				}

				for(int i = 0; i < enum_count; ++i)
				{
					std::ofstream f("/tmp/test/" + enums[i].name + ".h");
					f<<enums[i].to_header();
					std::ofstream f2("/tmp/test/" + enums[i].name + ".cpp");
					f2<<enums[i].to_cpp();
				}
*/
			}
		}
	}
}

int main(int argc, char* argv[])
{
	snake::message::__protobuf__::MessageGenerator generator;
	return google::protobuf::compiler::PluginMain(argc, argv, &generator);
}
