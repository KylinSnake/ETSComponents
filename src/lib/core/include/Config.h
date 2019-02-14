#ifndef SNAKE_CORE_CONFIG_H
#define SNAKE_CORE_CONFIG_H

#include <yaml-cpp/yaml.h>
#include <memory>
#include <string>
#include <list>

namespace snake
{
	namespace core
	{
		class ConfigNode final
		{
		public:
			ConfigNode() = default;
			~ConfigNode() = default;
			ConfigNode(const ConfigNode&) = default;
			ConfigNode& operator=(const ConfigNode&) = default;

			ConfigNode(const YAML::Node& node) noexcept;

			bool is_valid() const
			{
				return node_.IsDefined() && !node_.IsNull();
			}

			const std::string& name() const;

			std::string get_string_value(const std::string& child) const;
			int get_integer_value(const std::string& child) const;
			double get_float_value(const std::string& child) const;
			bool get_boolean_value(const std::string& child) const;

			std::string get_string_with_default(const std::string& child, const std::string&) const;
			int get_integer_with_default(const std::string& child, int) const;
			double get_float_with_default(const std::string& child, double) const;
			bool get_boolean_with_default(const std::string& child, bool) const;

			std::list<ConfigNode> get_node_list_of_children() const;
			std::list<std::string> get_string_list_of_children() const;
			std::list<int> get_integer_list_of_children() const;
			std::list<double> get_float_list_of_children() const;
			std::list<bool> get_boolean_list_of_children() const;

			bool has_child(const std::string& name) const;
			ConfigNode get_child(const std::string& name) const;
			
		private:
			YAML::Node node_{};
			friend class Config;

			template<class ostream>
			friend ostream& operator << (ostream&, const ConfigNode& node);
		};

		class Config final
		{
		public:
			Config() = default;
			~Config() = default;
			void init_from_file(const std::string& file_path);
			void init_from_string(const char* string);
			void init_from_string(const std::string& string);
			ConfigNode root() const;
		private:
			YAML::Node root_node_;
		};

		template<class ostream>
		ostream& operator << (ostream& os, const ConfigNode& node)
		{
			os << YAML::Dump(node.node_);
			return os;
		}
	}
}

#endif
