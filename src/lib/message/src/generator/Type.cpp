#include <Type.hpp>
#include <snake/util/String.h>

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
					return "::" + snake::util::join(namespaces_,"::");
				return "";
			}

			std::string Type::full_name() const
			{
				std::string p = "";
				if (parents_.size()>0)
				{
					p = snake::util::join(parents_, "::") + "::";
				}
				return p + name_;
			}

			void Type::parse_type(const std::string& name, const FileDescriptor* file, const std::vector<std::string>& parent)
			{
				name_ = name;
				auto n = snake::util::split(file->name(), "/.");
				proto_source_name_ = n[n.size()-2];
				namespaces_ = snake::util::split(file->package(), ".");
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

				hpp.add_type(This());
				cpp.add_type(This());
				cpp.add_header("snake/message/" + name_ + ".h");
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
				auto fn = full_name();
				hpp.print_line("std::string enum_to_string(" + fn +" e);");
				hpp.print_line("bool parse_enum(const std::string&, " + fn +"& e);");
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

				if(dsp->is_map())
				{
					p->is_map_ = true;
					hpp.add_header("map");
					cpp.add_header("google/protobuf/map.h");
					auto kv_dsp = dsp->message_type();
					auto k_dsp = kv_dsp->FindFieldByName("key");
					auto v_dsp = kv_dsp->FindFieldByName("value");
					p->key_field_ = create(k_dsp, hpp, cpp);
					p->value_field_ = create(v_dsp, hpp, cpp);
				}
				else if (p->dsp_cpp_type_enum_ == FieldDescriptor::CppType::CPPTYPE_ENUM)
				{
					hpp.add_header("snake/message/" + dsp->enum_type()->name() + ".h");
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
						hpp.add_header("snake/util/DateTime.h");
						cpp.add_header("google/protobuf/util/time_util.h");
						p->cpp_type_ = "snake::util::DateTime";
					}
					else
					{
						hpp.add_header("snake/message/" + dsp->message_type()->name() + ".h");
						p->cpp_type_ = dsp->message_type()->name();
					}
				}
				else if(p->dsp_cpp_type_enum_ == FieldDescriptor::CppType::CPPTYPE_STRING)
				{
					hpp.add_header("string");
					p->cpp_type_ = "std::string";
				}

				if (dsp->is_repeated() && !p->is_map_)
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
				else if (is_map_)
				{
					hpp.print_line("std::map<" + key_field_->cpp_type_ + ", " + value_field_->cpp_type_ + "> " + name_ + "{};");
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
				else if(is_map_)
				{
					output_deserialize_map(cpp, proto_var, local_var, ret_bool);
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
				if(cpp_type_ == "snake::util::DateTime")
				{
					cpp.print_line(local_var + "." + name_ + ".push_back(google::protobuf::util::TimeUtil::TimestampToNanoseconds(" + proto_var + "." + name_ + "(i)));");
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

			void FieldType::output_deserialize_map(OutputCpp& cpp, const std::string& proto_var, const std::string& local_var, const std::string& ret_bool)
			{
				auto e= [](OutputCpp& cpp, const std::string& proto, const std::string& local, const std::string& cpp_type, FieldDescriptor::CppType type_enum, const std::string& ret_bool)
				{
					if(cpp_type == "snake::util::DateTime")
					{
						cpp.print_line(local + " = google::protobuf::util::TimeUtil::TimestampToNanoseconds(" + proto + ");");
					}
					else if(type_enum == FieldDescriptor::CppType::CPPTYPE_MESSAGE)
					{
						cpp.print_line(ret_bool + " = " + ret_bool + " && protobuf_to_" + cpp_type + "(" + proto + "," + local + ");");
					}
					else if (type_enum == FieldDescriptor::CppType::CPPTYPE_ENUM)
					{
						cpp.print_line(local + " = static_cast<" + cpp_type + ">(int(" + proto + "));");
					}
					else
					{
						cpp.print_line(local + " = " + proto + ";");
					}
				};

				cpp.print_line("for(auto& p : " + proto_var + "." + name_ + "())");
				cpp.print_line("{");
				cpp.increment_tab();
				cpp.print_line(key_field_->cpp_type_ + " key{};");
				e(cpp, "p.first", "key", key_field_->cpp_type_, key_field_->dsp_cpp_type_enum_, ret_bool);
				cpp.print_line(value_field_->cpp_type_ + " value{};");
				e(cpp, "p.second", "value", value_field_->cpp_type_, value_field_->dsp_cpp_type_enum_, ret_bool);
				cpp.print_line(local_var + "." + name_ + "[key] = value;");
				cpp.decrement_tab();
				cpp.print_line("}");
			}

			void FieldType::output_deserialize_item(OutputCpp& cpp, const std::string& proto_var, const std::string& local_var, const std::string& ret_bool)
			{
				if(cpp_type_ == "snake::util::DateTime")
				{
					cpp.print_line(local_var + "." + name_ + " = google::protobuf::util::TimeUtil::TimestampToNanoseconds(" + proto_var + "." + name_ + "());");
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
				else if(is_map_)
				{
					output_serialize_map(cpp, local_var, proto_var, ret_bool);
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

				if(cpp_type_ == "snake::util::DateTime")
				{
					cpp.print_line("*" + proto_var + ".add_" + name_ + "() = google::protobuf::util::TimeUtil::NanosecondsToTimestamp(std::int64_t(p));");
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

			void FieldType::output_serialize_map(OutputCpp& cpp, const std::string& local_var, const std::string& proto_var, const std::string& ret_bool)
			{
				auto e= [](OutputCpp& cpp, const std::string& local, const std::string& proto, const std::string& cpp_type, FieldDescriptor::CppType type_enum, const std::string& ret_bool)
				{
					if(cpp_type == "snake::util::DateTime")
					{
						cpp.print_line(proto + " = google::protobuf::util::TimeUtil::NanosecondsToTimestamp(std::int64_t (" + local + "));");
					}
					else if(type_enum == FieldDescriptor::CppType::CPPTYPE_MESSAGE)
					{
						cpp.print_line(ret_bool + " = " + ret_bool + " && " + cpp_type + "_to_protobuf(" + local + "," + proto + ");");
					}
					else if (type_enum == FieldDescriptor::CppType::CPPTYPE_ENUM)
					{
						cpp.print_line(proto + " = static_cast<decltype(" + proto + ")>(int(" + local + "));");
					}
					else
					{
						cpp.print_line(proto + " = " + local + ";");
					}
				};

				cpp.print_line("for(auto& p : " +local_var + "." + name_ + ")");
				cpp.print_line("{");
				cpp.increment_tab();
				cpp.print_line("using ProtoMap= std::decay<decltype( " + proto_var + "." + name_ + "())>::type;");
				cpp.print_line("ProtoMap::key_type key{};");
				e(cpp, "p.first", "key", key_field_->cpp_type_, key_field_->dsp_cpp_type_enum_, ret_bool);
				cpp.print_line("ProtoMap::mapped_type value{};");
				e(cpp, "p.second", "value", value_field_->cpp_type_, value_field_->dsp_cpp_type_enum_, ret_bool);
				cpp.print_line("(*" + proto_var + ".mutable_" + name_ + "())[key] = value;");
				cpp.decrement_tab();
				cpp.print_line("}");
			}

			void FieldType::output_serialize_item(OutputCpp& cpp, const std::string& local_var, const std::string& proto_var, const std::string& ret_bool)
			{
				if(cpp_type_ == "snake::util::DateTime")
				{
					cpp.print_line("*" + proto_var + ".mutable_" + name_ + "() = google::protobuf::util::TimeUtil::NanosecondsToTimestamp(std::int64_t(" + local_var + "." + name_ + "));");
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
						hpp.add_header("snake/message/" + base_type_ + ".h");
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
				cpp.add_header("snake/message/" + name_ + ".h");
				if (base_type_.empty())
				{
					hpp.add_header("snake/message/MessageType.h");
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

				if(base_type_.empty())
				{
					hpp.print_line(name_ + "(): major_version{MESSAGE_MAJOR_VERSION}, minor_version{MESSAGE_MINOR_VERSION}");
					hpp.print_line("{ last_update = snake::util::DateTime::now(); }");
					hpp.print_line("~" + name_ + "() = default;");
				}
				else if(name_ == "MessageVersion")
				{
					hpp.print_line("MessageVersion() : " + base_type_ + "(), minimal_major_version(MESSAGE_MINIMAL_MAJOR_VERSION), minimal_minor_version(MESSAGE_MINIMAL_MINOR_VERSION) {}");
					hpp.print_line("virtual ~MessageVersion() = default;");
				}
				else
				{
					hpp.print_line(name_ + "() = default;");
					hpp.print_line("virtual ~" + name_ + "() = default;");
				}
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
					hpp.print_line("virtual bool serialize(std::string&) const = 0;");
					hpp.print_line("virtual bool deserialize(const std::string&) = 0;");
					hpp.print_line("void set_last_update() { last_update = snake::util::DateTime::now(); }");
				}
				else
				{
					hpp.print_line("virtual MessageType type() const override;");
					hpp.print_line("virtual bool serialize(std::string&) const override;");
					hpp.print_line("virtual bool deserialize(const std::string&) override;");
				}
				hpp.print_line("bool operator==(const " + name_ + "&) const;");
				hpp.decrement_tab();
				hpp.print_line("};");
			}

			void ClassType::output(OutputCpp& cpp)
			{
				auto lower_name = snake::util::to_lower(name_);
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
				cpp.print_line("return MessageType::" + (base_type_.empty() ? "UNKNOWN" : snake::util::to_upper(snake::util::camel_to_lower_case(name_))) + ";");
				cpp.decrement_tab();
				cpp.print_line("}");
				cpp.print_line();

				// bool operator==(const MsgType&) const

				cpp.print_line("bool " + name_ + "::operator==(const " + name_ + "& rhs) const");
				cpp.print_line("{");
				cpp.increment_tab();
				std::string line{};
				for(auto& p : fields_)
				{
					auto name = p.second->name();
					line += " && (" + name + " == rhs." + name + ")";
				}
				if (base_type_.empty())
				{
					line = "true " + line;
				}
				else
				{
					line = base_type_ + "::operator==(rhs)" + line;
				}
				cpp.print_line(" return " + line + ";");
				cpp.decrement_tab();
				cpp.print_line("}");
				cpp.print_line();

				if(!base_type_.empty())
				{
					// virtual bool serialize(std::string&) const override
					cpp.print_line("bool " + name_ + "::serialize(std::string& out) const");
					cpp.print_line("{");
					cpp.increment_tab();
					cpp.print_line("proto::MessageFactory f{};");
					cpp.print_line("auto p = f.add_item()->mutable_" + lower_name + "();");
					cpp.print_line("auto ret = " + name_ + "_to_protobuf(*this, *p);");
					cpp.print_line("return ret && f.SerializeToString(&out);");
					cpp.decrement_tab();
					cpp.print_line("}");
					cpp.print_line();

					// virtual bool deserialize(const std::string&) override
					cpp.print_line("bool " + name_ + "::deserialize(const std::string& out)");
					cpp.print_line("{");
					cpp.increment_tab();
					cpp.print_line("proto::MessageFactory f{};");
					cpp.print_line("auto ret = f.ParseFromString(out);");
					cpp.print_line("if(ret && f.item_size() == 1 && f.item(0).has_" + lower_name + "())");
					cpp.print_line("{");
					cpp.increment_tab();
					cpp.print_line("return protobuf_to_" + name_ + "(f.item(0)." + lower_name + "(), *this);");
					cpp.decrement_tab();
					cpp.print_line("}");
					cpp.print_line("return false;");
					cpp.decrement_tab();
					cpp.print_line("}");
					cpp.print_line();
				}

			}

			// FactoryType

			bool FactoryType::parse(const Descriptor* dsp, std::vector<std::string>& parents, OutputHpp& hpp, OutputCpp& cpp)
			{
				parse_type(dsp->name(), dsp->file(), parents);

				auto nest_dsp = dsp->FindNestedTypeByName("Item");

				if (nest_dsp == nullptr)
				{
					return false;
				}

				auto oneof_dsp = nest_dsp->FindOneofByName("tag");

				if (oneof_dsp == nullptr)
				{
					return false;
				}

				auto size = oneof_dsp->field_count();
				for(int i = 0; i < size; ++i)
				{
					auto d = oneof_dsp->field(i);
					auto name = d->name();
					auto type = d->message_type()->name();
					auto index = d->number();
					fields_[name] = type;
					types_[index] = type;
				}

				hpp.add_header("memory");
				hpp.add_header("functional");
				hpp.add_header("string");
				cpp.add_header(proto_source_name_ + ".pb.h");
				cpp.add_header("snake/message/" + name_ + ".h");

				hpp.add_type(This());
				cpp.add_type(This());

				cpp.add_global_function_decl("bool handle_message_from_item(const ::snake::message::proto::MessageFactory::Item&, const std::function<void(const MessageBase*)>&);");

				return true;
			}

			void FactoryType::output(OutputHpp& hpp)
			{
				hpp.print_line("class MessageBase;");
				hpp.print_line("bool serialize_to_string(const std::function<const MessageBase*()>& next, std::string& ret);");
				hpp.print_line();

				hpp.print_line("template<typename iteratorT>");
				hpp.print_line("inline bool serialize_to_string(iteratorT begin, iteratorT end, std::string& ret)");

				hpp.print_line("{");
				hpp.increment_tab();
				hpp.print_line("return serialize_to_string([&begin, &end](){ return begin == end ? nullptr : &(*begin++);}, ret);");
				hpp.decrement_tab();
				hpp.print_line("}");
				hpp.print_line();

				hpp.print_line("size_t parse_from_string(const std::string& str, const std::function<void(const MessageBase*)>& handler);");
			}

			void FactoryType::output(OutputCpp& cpp)
			{
				cpp.print_line("bool handle_message_from_item(const ::snake::message::proto::MessageFactory::Item& item, const std::function<void(const MessageBase*)>& func)");
				cpp.print_line("{");
				cpp.increment_tab();
				for (auto& p : fields_)
				{
					auto name = snake::util::to_lower(p.first);
					auto type = p.second;

					cpp.print_line("if (item.has_"+name+"())");
					cpp.print_line("{");
					cpp.increment_tab();
					cpp.print_line(type + " t{};");
					cpp.print_line("if(protobuf_to_"+ type +"(item."+name+"(), t))");
					cpp.print_line("{");
					cpp.increment_tab();
					cpp.print_line("func(&t);");
					cpp.print_line("return true;");
					cpp.decrement_tab();
					cpp.print_line("}");
					cpp.print_line("else return false;");
					cpp.decrement_tab();
					cpp.print_line("}");
				}
				cpp.print_line("return false;");
				cpp.decrement_tab();
				cpp.print_line("}");
				cpp.print_line();
				
				cpp.print_line("size_t parse_from_string(const std::string& str, const std::function<void(const MessageBase*)>& handler)");
				cpp.print_line("{");
				cpp.increment_tab();
				cpp.print_line("size_t ret {0};");
				cpp.print_line("proto::MessageFactory f;");
				cpp.print_line("if(f.ParseFromString(str))");
				cpp.print_line("{");
				cpp.increment_tab();
				cpp.print_line("for(int i = 0; i < f.item_size(); ++i)");
				cpp.print_line("{");
				cpp.increment_tab();
				cpp.print_line("ret += handle_message_from_item(f.item(i), handler) ? 1 : 0;");
				cpp.decrement_tab();
				cpp.print_line("}");
				cpp.decrement_tab();
				cpp.print_line("}");
				cpp.print_line("return ret;");
				cpp.decrement_tab();
				cpp.print_line("}");
				cpp.print_line();

				cpp.print_line("bool serialize_to_string(const std::function<const MessageBase*()>& next, std::string& ret)");
				cpp.print_line("{");
				cpp.increment_tab();
				cpp.print_line("proto::MessageFactory f;");
				cpp.print_line("for(auto ptr = next(); ptr!= nullptr; ptr = next())");
				cpp.print_line("{");
				cpp.increment_tab();
				cpp.print_line("auto type = ptr->type();");
				for(auto& p : fields_)
				{
					cpp.print_line("if (type == MessageType::" + snake::util::to_upper(snake::util::camel_to_lower_case(p.second)) + ")");
					cpp.print_line("{");
					cpp.increment_tab();
					cpp.print_line("auto msg = dynamic_cast<const " + p.second + "*>(ptr);");
					cpp.print_line("assert(msg != nullptr);");
					cpp.print_line("proto::" + p.second + " t{};");
					cpp.print_line("if(!" + p.second + "_to_protobuf(*msg, t)) return false;");
					cpp.print_line("*(f.add_item()->mutable_" + snake::util::to_lower(p.first) + "()) = t;");
					cpp.print_line("continue;");
					cpp.decrement_tab();
					cpp.print_line("}");
				}
				cpp.print_line("return false;");
				cpp.decrement_tab();
				cpp.print_line("}");
				cpp.print_line("return f.SerializeToString(&ret);");
				cpp.decrement_tab();
				cpp.print_line("}");
				cpp.print_line();

			}
		}
	}
}
