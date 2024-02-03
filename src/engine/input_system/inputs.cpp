#include "inputs.hpp"

using namespace input;

Input::Input()
{
}

void Input::register_action(std::string action, InputMapping mapping)
{
	if (auto search = this->mappings.find(action); search != this->mappings.end())
	{
		search->second.push_back(mapping);
	}
	else
	{
		this->mappings[action] = {
			mapping,
		};
	}
	this->states[action] = InputState();
}

void Input::register_actions(std::map<std::string, std::vector<InputMapping>> mappings)
{
	for (auto entry : mappings)
	{
		for (auto map : entry.second)
		{
			register_action(entry.first, map);
		}
	}
}

void Input::register_from_toml(std::filesystem::path toml_file)
{
	// reference: https://github.com/ToruNiina/toml11
	auto data = toml::parse(toml_file.c_str());
	auto table = data.as_table();
	std::map<std::string, std::vector<InputMapping>> mappings;
	for (auto entry : table)
	{
		auto key = entry.first;
		auto value = entry.second;
		// if (value.is_table())
		// {
		// 	std::cout << "Found TOML table\t" << value.as_string() << std::endl;
		// }
	}
	register_actions(mappings);
}

void Input::poll_input_events(GLFWwindow *window)
{
	for (auto action : mappings)
	{
		bool state = false;
		for (auto map : action.second)
		{
			if (glfwGetKey(window, map.key) == map.mode)
			{
				// early exit
				state = true;
				break;
			}
		}
		this->states[action.first].is_pressed = state;
	}
}

bool Input::is_action_pressed(std::string action)
{
	return this->states[action].is_pressed;
}

float Input::get_action_axis(std::string action_negative, std::string action_positive)
{
	float result = 0.0f;
	if (is_action_pressed(action_negative))
		result -= 1.0f;
	if (is_action_pressed(action_positive))
		result += 1.0f;
	return result;
}
glm::vec2 Input::get_action_vector(std::string action_negative_x, std::string action_positive_x, std::string action_negative_y, std::string action_positive_y)
{
	return {
		get_action_axis(action_negative_x, action_positive_x),
		get_action_axis(action_negative_y, action_positive_y),
	};
}
