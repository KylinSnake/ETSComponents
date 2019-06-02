#include <MessageGenerator.hpp>

#include <google/protobuf/compiler/plugin.h>
#include <google/protobuf/descriptor.h>

#include <map>
#include <string>
#include <vector>

#include <fstream>

#include <snake/util/String.h>
#include <Type.hpp>
#include <OutputFile.hpp>

using namespace google::protobuf::compiler;

namespace snake
{
	namespace __protobuf__
	{
		OutputCpp& get_cpp_singleton()
		{
			static OutputCpp t;
			return t;
		}

		bool MessageGenerator::Generate(const FileDescriptor * file, const string & parameter, 
				GeneratorContext * generator_context, string * error) const 
		{
			std::vector<std::string> parent;
			std::vector<std::shared_ptr<OutputHpp>> hpps;
			
			auto& cpp = get_cpp_singleton();

			// extract enum type
			int enum_count = file->enum_type_count();
			for(int i = 0 ; i < enum_count; ++i)
			{
				auto dsp = file->enum_type(i);
				auto hpp = std::make_shared<OutputHpp>();
				auto enm = EnumType::create();
				if(enm->parse(dsp, parent, *hpp, cpp))
				{
					hpp->set_file_name(enm->name() + ".h");
					hpp->set_path("include/snake/message");
					hpps.push_back(hpp);
				}
			}

			// extract class type
			int class_count = file->message_type_count();
			for(int i = 0; i < class_count; ++i)
			{
				auto dsp = file->message_type(i);
				auto hpp = std::make_shared<OutputHpp>();
				auto result = false;
				std::string file_name = "";
				if (dsp->name() == "MessageFactory")
				{
					auto cls = FactoryType::create();
					result = cls->parse(dsp, parent, *hpp, cpp);
					file_name = cls->name();

				}
				else
				{
					auto cls = ClassType::create();
					result = cls->parse(dsp, parent, *hpp, cpp);
					file_name = cls->name();
				}
				if(result)
				{
					hpp->set_file_name(file_name + ".h");
					hpp->set_path("include/snake/message");
					hpps.push_back(hpp);
				}
				else
				{
					throw std::runtime_error("Error on parse: " + dsp->name());
				}
			}

			++current_file_number;

			// output generated files
			if (current_file_number == proto_file_number)
			{
				cpp.set_file_name("generated.cpp");
				cpp.output(generator_context);
			}
			for(auto& i : hpps)
			{
				i->output(generator_context);
			}

			return Base::Generate(file, parameter, generator_context, error);
		}
	}
}

int main(int argc, char* argv[])
{
	snake::__protobuf__::MessageGenerator generator;
	return google::protobuf::compiler::PluginMain(argc, argv, &generator);
}
