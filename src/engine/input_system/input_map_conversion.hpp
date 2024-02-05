#pragma once
#include <GLFW/glfw3.h>
#include <string>
#include <iostream>
#include <optional>
#include <algorithm>
#include <map>

constexpr std::string_view PREFIX_KEY = {"key"};
constexpr std::string_view PREFIX_MOUSE_BUTTON = {"mbtn"};
constexpr std::string_view PREFIX_GAMEPAD_BUTTON = {"joybtn"};
constexpr std::string_view PREFIX_GAMEPAD_AXIS = {"joyaxis"};
constexpr std::string_view DELIM_MODE_PREFIX = {"_"};
constexpr std::string_view DELIM_INPUT_CODE = {":"};
constexpr unsigned int SCANCODE_SEARCH_RANGE = 256; // TBH probably still too high

namespace input
{

	static std::map<std::string_view, unsigned int> INPUT_IDS;

	enum InputMode
	{
		Key,
		MouseButton,
		GamepadButton,
		GamepadAxis,
		// Ignoring joystick controls (for now??)
	};

	struct InputMapping
	{
		unsigned int code = GLFW_KEY_UNKNOWN;
		InputMode mode = InputMode::Key;
		unsigned int action = GLFW_PRESS;
	};
	void populate_input_ids();
	std::optional<InputMapping> load_mapping(std::string key);
	std::string get_string_from(InputMapping mapping);

	unsigned int get_code_from(std::string name);
}