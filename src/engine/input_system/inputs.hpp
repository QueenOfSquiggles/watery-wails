#pragma once

#include "input_map_conversion.hpp"

#include <string>
#include <vector>
#include <map>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <memory>
#include <filesystem>
#include <iostream>
#include <toml11/toml.hpp>

namespace input
{
	struct InputState
	{
		bool is_pressed;
		bool just_pressed;
	};

	class Input
	{
		std::map<std::string, std::vector<InputMapping>> mappings;
		std::map<std::string, InputState> states;

	public:
		/// This is the threshold for the gamepad axis to be recognized. Inputs are exclusively binary at this moment
		float gamepad_threshold = 0.125f;
		Input();
		void register_from_toml(std::filesystem::path toml_file);
		void register_action(std::string action, InputMapping mapping);
		void register_actions(std::map<std::string, std::vector<InputMapping>> mappings);
		void poll_input_events(GLFWwindow *window);

		bool is_action_pressed(std::string action);
		bool is_action_just_pressed(std::string action);
		float get_action_axis(std::string action_negative, std::string action_positive);
		glm::vec2 get_action_vector(std::string action_negative_x, std::string action_positive_x, std::string action_negative_y, std::string action_positive_y);
	};
}