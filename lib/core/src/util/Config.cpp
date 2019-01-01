#include <Config.h>
#include <Exception.h>

#include <algorithm>

namespace snake
{
	namespace core
	{
		template<typename T>
		T get_node_value(const YAML::Node& node)
		{
			try
			{
				return node.as<T>();
			}
			catch(const YAML::InvalidNode& e)
			{
				throw InvalidArgumentException(e.what());
			}
			catch(const YAML::TypedBadConversion<T>& e)
			{
				throw WrongTypeException();
			}
		}
		Config Config::create_config_from_file(const std::string& file_path)
		{
			return Config(YAML::LoadFile(file_path));
		}

		Config::Config(const YAML::Node& node)
		: root_node_(node)
		{
		}

		ConfigNode Config::root() const
		{
			return ConfigNode(root_node_);
		}

		ConfigNode::ConfigNode(const YAML::Node& node) noexcept
		: node_(node)
		{
		}

		std::string ConfigNode::get_string_value(const std::string& name) const
		{
			return get_node_value<std::string>(node_[name]);
		}

		int ConfigNode::get_integer_value(const std::string& name) const
		{
			return get_node_value<int>(node_[name]);
		}

		float ConfigNode::get_float_value(const std::string& name) const
		{
			return get_node_value<float>(node_[name]);
		}

		bool ConfigNode::get_boolean_value(const std::string& name) const
		{
			return get_node_value<bool>(node_[name]);
		}

		std::string ConfigNode::get_string_with_default(const std::string& name, const std::string& value) const
		{
			try
			{
				return get_node_value<std::string>(node_[name]);
			}
			catch(...){}
			return value;
		}

		int ConfigNode::get_integer_with_default(const std::string& name, int value) const
		{
			try
			{
				return get_node_value<int>(node_[name]);
			}
			catch(...){}
			return value;
		}

		float ConfigNode::get_float_with_default(const std::string& name, float value) const
		{
			try
			{
				return get_node_value<float>(node_[name]);
			}
			catch(...){}
			return value;
		}

		bool ConfigNode::get_boolean_with_default(const std::string& name, bool value) const
		{
			try
			{
				return get_node_value<bool>(node_[name]);
			}
			catch(...){}
			return value;
		}

		std::list<ConfigNode> ConfigNode::get_node_list_of_children() const
		{
			std::list<ConfigNode> ret;
			std::for_each(node_.begin(), node_.end(), [&ret](const YAML::Node& n){ ret.push_back(ConfigNode{n});});
			return ret;
		}

		std::list<std::string> ConfigNode::get_string_list_of_children() const
		{
			std::list<std::string> ret;
			std::for_each(node_.begin(), node_.end(), [&ret](const YAML::Node& n){ ret.push_back(get_node_value<std::string>(n));});
			return ret;
		}

		std::list<int> ConfigNode::get_integer_list_of_children() const
		{
			std::list<int> ret;
			std::for_each(node_.begin(), node_.end(), [&ret](const YAML::Node& n){ ret.push_back(get_node_value<int>(n));});
			return ret;
		}

		std::list<float> ConfigNode::get_float_list_of_children() const
		{
			std::list<float> ret;
			std::for_each(node_.begin(), node_.end(), [&ret](const YAML::Node& n){ ret.push_back(get_node_value<float>(n));});
			return ret;
		}

		std::list<bool> ConfigNode::get_bool_list_of_children() const
		{
			std::list<bool> ret;
			std::for_each(node_.begin(), node_.end(), [&ret](const YAML::Node& n){ ret.push_back(get_node_value<bool>(n));});
			return ret;
		}

		ConfigNode ConfigNode::get_child(const std::string& child) const
		{
			return ConfigNode(node_[child]);
		}

		bool ConfigNode::has_child(const std::string& child) const
		{
			return get_child(child).is_valid();
		}
	}
}
