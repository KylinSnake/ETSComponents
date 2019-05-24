#include <cassert>

#include <google/protobuf/io/zero_copy_stream.h>

#include <OutputFile.hpp>
#include <Type.hpp>

#include <snake/util/String.h>

namespace snake 
{ 
	namespace __protobuf__
	{
		std::string OutputFile::tabs(size_t num)
		{
			static const char* tab = "\t";
			static std::vector<std::string> vec {""};
			for(size_t i = 1; i <= num; ++i)
			{
				vec.push_back(vec.back() + tab);
			}
			return vec[num];
		}

		OutputFile::OutputFile()
		{
		}

		OutputFile::~OutputFile()
		{
		}

		void OutputFile::set_file_name(const std::string& name)
		{
			file_name_ = name;
		}

		void OutputFile::set_path(const std::string& path)
		{
			path_ = path;
		}

		void OutputFile::add_type(const std::shared_ptr<Type>& t)
		{
			auto name = t->name();
			if (type_ptrs_.count(name) == 0)
			{
				type_ptrs_[name] = t;
			}
		}

		void OutputFile::add_header(const std::string& header)
		{
			headers_.emplace(header);
		}

		bool OutputFile::output(GeneratorContext * generator_context)
		{
			auto file = file_name_;
			if (path_.size() > 0)
			{
				file = path_ + "/" + file;
			}
			auto f = generator_context->Open(file);
			if (f == nullptr)
			{
				return false;
			}

			stream_ = f;
			output_header_comment();
			output_pre_headers();
			output_headers();
			output_after_headers();
			output_begin_namespace();

			increment_tab();
			output_type();
			decrement_tab();

			output_end_namespace();
			output_after_end_namespace();

			return true;
		}

		void OutputFile::print_line(const std::string& line, size_t tab_num)
		{
			auto s = tabs(tab_num + current_tab_num_) + line  + "\n";
			const char* str = s.c_str();
			int str_size = static_cast<int>(s.size());
			char* buf = nullptr;
			int buf_size = 0;
			while(stream_->Next((void**)&buf, &buf_size))
			{
				if(buf_size > 0)
				{
					auto size = buf_size >= str_size ? str_size : buf_size;
					memcpy(buf, str, size);
					str += size;
					buf_size -= size;
					str_size -= size;

					if(str_size == 0)
					{
						if(buf_size > 0)
						{
							stream_->BackUp(buf_size);
						}
						break;
					}
				}
			}
		}

		void OutputFile::output_header_comment()
		{
			print_line("// This file is generated by the automation tools from protobuf.");
			print_line("// Do not change this file directly, instead, change the protobuf file");
			print_line("// to update the message definition.");
			print_line("");
		}

		void OutputFile::output_headers()
		{
			for(auto& s : headers_)
			{
				print_line("#include<" + s + ">");
			}
			print_line("");
		}

		void OutputFile::output_begin_namespace()
		{
			print_line("namespace snake {\n");
		}

		void OutputFile::output_end_namespace()
		{
			print_line("\n}");
		}

		void OutputFile::output_pre_headers()
		{
		}

		void OutputFile::output_after_headers()
		{
		}

		void OutputFile::output_after_end_namespace()
		{
		}

		void OutputFile::add_global_function_decl(const std::string& s)
		{
			global_functions_.emplace(s);
		}

		OutputHpp::OutputHpp()
		: OutputFile()
		{
		}

		OutputHpp::~OutputHpp()
		{
		}

		void OutputHpp::output_pre_headers()
		{
			auto s = snake::split(snake::to_upper(file_name()), ".")[0];
			print_line("#ifndef SNAKE_MESSAGE_" + s + "_H");
			print_line("#define SNAKE_MESSAGE_" + s + "_H");
			print_line("");
		}

		void OutputHpp::output_after_end_namespace()
		{
			print_line("#endif");
		}

		void OutputHpp::output_type()
		{
			for(auto& s : type_ptrs_)
			{
				s.second->output(*this);
			}
			print_line();
			for(auto& s : global_functions_)
			{
				print_line(s);
			}
		}

		OutputCpp::OutputCpp()
		: OutputFile()
		{
		}

		OutputCpp::~OutputCpp()
		{
		}

		void OutputCpp::output_type()
		{
			for(auto& s : global_functions_)
			{
				print_line(s);
			}
			print_line();
			for(auto& s : type_ptrs_)
			{
				print_line("// Output Type: " + s.first);
				s.second->output(*this);
			}
		}
	}
}
