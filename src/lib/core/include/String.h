#ifndef SNAKE_CORE_STRING_H
#define SNAKE_CORE_STRING_H

#include <string>
#include <cstring>
#include <cctype>

namespace snake
{
	namespace core
	{
		inline std::string& ltrim(std::string& str, const std::string& chars = "\t\n\v\f\r ")
		{
			str.erase(0, str.find_first_not_of(chars));
			return str;
		}
		 
		inline std::string& rtrim(std::string& str, const std::string& chars = "\t\n\v\f\r ")
		{
			str.erase(str.find_last_not_of(chars) + 1);
			return str;
		}
		  
		inline std::string& trim(std::string& str, const std::string& chars = "\t\n\v\f\r ")
		{
			return ltrim(rtrim(str, chars), chars);
		}

		template<class T>
		inline std::string join(const T& t, const char* join_key)
		{
			std::string ret;
			auto begin = std::begin(t);
			auto end = std::end(t);
			while(begin != end)
			{
				ret += *begin;
				++begin;
				if(begin != end)
				{
					ret += join_key;
				}
			}
			return ret;
		}



		inline std::vector<std::string> split(const char* str, const char* key)
		{
			std::vector<std::string> ret;
			auto begin = str;
			const char* pch = nullptr;
			do
			{
				pch = std::strpbrk(begin, key);
				if (pch == nullptr)
				{
					ret.push_back(std::string{begin});
				}
				else
				{
					ret.push_back(std::string{begin, pch});
					begin = pch + 1;
				}

			}while(pch != nullptr);
			return ret;
		}

		inline std::vector<std::string> split(const std::string& str, const std::string& key)
		{
			return split(str.c_str(), key.c_str());
		}

		inline std::string to_upper(std::string str)
		{
			std::transform(str.begin(), str.end(), str.begin(),
				[](unsigned char c){ return std::toupper(c); } );
			return str;
		}

		inline std::string to_lower(std::string str)
		{
			std::transform(str.begin(), str.end(), str.begin(),
				[](unsigned char c){ return std::tolower(c); } );
			return str;
		}
	}
}

#endif
