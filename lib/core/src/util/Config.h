#ifndef SNAKE_CORE_CONFIG_H
#define SNAKE_CORE_CONFIG_H

#include <yaml.h>
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
			ConfigNode(const YAML::Node& node) noexcept;
			~ConfigNode() = default;
			ConfigNode(const ConfigNode&) = default;
			ConfigNode& operator=(const ConfigNode&) = default;

			bool is_valid() const
			{
				return node_.IsDefined() && !node_.IsNull();
			}

			std::string get_string_value(const std::string& child) const;
			int get_integer_value(const std::string& child) const;
			float get_float_value(const std::string& child) const;
			bool get_boolean_value(const std::string& child) const;

			std::string get_string_with_default(const std::string& child, const std::string&) const;
			int get_integer_with_default(const std::string& child, int) const;
			float get_float_with_default(const std::string& child, float) const;
			bool get_boolean_with_default(const std::string& child, bool) const;

			std::list<ConfigNode> get_node_list_of_children() const;
			std::list<std::string> get_string_list_of_children() const;
			std::list<int> get_integer_list_of_children() const;
			std::list<float> get_float_list_of_children() const;
			std::list<bool> get_bool_list_of_children() const;

			bool has_child(const std::string& name) const;
			ConfigNode get_child(const std::string& name) const;
			
		private:
			YAML::Node node_;
			friend class Config;
		};

		class Config final
		{
		public:
			~Config() = default;
			Config(const Config&) = delete;
			Config& operator=(const Config&) = delete;
			static Config create_config_from_file(const std::string& file_path);
			ConfigNode root() const;
		private:
			Config(const YAML::Node& root);
			YAML::Node root_node_;
		};
	}
}

#endif
