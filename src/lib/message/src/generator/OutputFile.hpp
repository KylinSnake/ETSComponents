#ifndef SNAKE_MESSAGE_PROTOBUF_OUTPUT_FILE_HPP
#define SNAKE_MESSAGE_PROTOBUF_OUTPUT_FILE_HPP

#include <string>
#include <set>
#include <map>
#include <memory>
#include <vector>
#include <fstream>
#include <google/protobuf/compiler/code_generator.h>

using namespace google::protobuf::compiler;
using namespace google::protobuf;

namespace snake
{
	namespace message
	{
		namespace __protobuf__
		{
			class Type;

			class OutputFile
			{
			public:
				OutputFile();
				virtual ~OutputFile();

				void add_header(const std::string& header_name);
				void add_type(const std::shared_ptr<Type>& t);
				void set_file_name(const std::string& name);
				void set_path(const std::string& path);

				bool output(GeneratorContext * generator_context);

				const std::string& file_name() const { return file_name_; }

				void print_line(const std::string& line, size_t tab_num = 0);

				void increment_tab() { ++current_tab_num_;}
				void decrement_tab() { --current_tab_num_;}

				static std::string tabs(size_t i);
			
				void add_global_function_decl(const std::string& s);
			protected:
				void output_header_comment();
				void output_headers();
				void output_begin_namespace();
				void output_end_namespace();

				virtual void output_type() = 0;

				virtual void output_pre_headers();
				virtual void output_after_headers();
				virtual void output_after_end_namespace();

				std::set<std::string> headers_ {};

				std::map<std::string, std::shared_ptr<Type>> type_ptrs_ {};
				std::string file_name_ {};
				std::string path_{};
				int current_tab_num_ {};
				std::set<std::string> global_functions_ {};

				io::ZeroCopyOutputStream * stream_ {nullptr};
			};

			class OutputHpp : public OutputFile
			{
			public:
				OutputHpp();
				virtual ~OutputHpp();

			protected:
				virtual void output_type() override;
				virtual void output_pre_headers() override;
				virtual void output_after_end_namespace() override;
			private:
				std::set<std::string> global_functions_ {};
			};

			class OutputCpp : public OutputFile
			{
			public:
				OutputCpp();
				virtual ~OutputCpp();

			protected:
				virtual void output_type() override;

			};
		}
	}
}


#endif
