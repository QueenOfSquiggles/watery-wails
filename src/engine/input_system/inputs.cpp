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
		if (!value.is_table())
			continue;
		mappings[key] = std::vector<InputMapping>();

		auto mapping_table = value.as_table();

		// prefer override over default
		auto search = mapping_table.find("override");
		if (search == mapping_table.end())
			search = mapping_table.find("default");
		if (search == mapping_table.end())
		{
			std::cerr << "Failed to find input mapping data for \"" << key.c_str() << "\"" << std::endl;
			continue;
		}
		std::vector<std::string> entries;

		switch (search->second.type())
		{
		case toml::value_t::array:
		{
			for (auto entry : search->second.as_array())
			{
				if (entry.is_string())
				{
					entries.push_back(entry.as_string());
				}
			}
		}
		break;
		case toml::value_t::string:
			entries.push_back(search->second.as_string());
			break;
		default:
			break;
		}
		if (entries.empty())
		{
			continue;
		}
		for (auto code : entries)
		{
			auto mapping = load_mapping(code);
			if (mapping.has_value())
			{
				mappings[key].push_back(mapping.value());
				// std::cout << "Mapping \"" << key.c_str() << "\" => (" << mapping->code << ", " << mapping->mode << ", " << mapping->action << ")" << std::endl;
			}
		}
	}
	register_actions(mappings);
}

void Input::poll_input_events(GLFWwindow *window)
{
	GLFWgamepadstate gamepad;
	glfwGetGamepadState(GLFW_JOYSTICK_1, &gamepad);

	for (auto action : mappings)
	{
		bool state = false;
		for (auto map : action.second)
		{
			bool mapping_state = false;
			switch (map.mode)
			{
			case InputMode::Key:
				mapping_state = glfwGetKey(window, map.code) == map.action;
				break;
			case InputMode::MouseButton:
				mapping_state = glfwGetMouseButton(window, map.code) == map.action;
				break;
			case InputMode::GamepadButton:
				if (map.code <= GLFW_GAMEPAD_BUTTON_LAST)
				{
					mapping_state = gamepad.buttons[map.code] == map.action;
				}
				break;
			case InputMode::GamepadAxis:
				if (map.code <= GLFW_GAMEPAD_AXIS_LAST)
				{
					mapping_state = gamepad.axes[map.code] >= gamepad_threshold;
				}
				break;
			}
			if (mapping_state)
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
