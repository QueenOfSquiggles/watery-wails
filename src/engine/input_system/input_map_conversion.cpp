#include "input_map_conversion.hpp"

namespace input
{

	std::optional<InputMapping> load_mapping(std::string key)
	{
		auto idx = key.find(DELIM_MODE_PREFIX);
		auto type = key.substr(0, idx);
		auto value = key;
		InputMapping map;
		// get mode
		if (type.compare(PREFIX_KEY) == 0)
		{
			map.mode = InputMode::Key;
		}
		else if (type.compare(PREFIX_MOUSE_BUTTON) == 0)
		{
			map.mode = InputMode::MouseButton;
		}
		else if (type.compare(PREFIX_GAMEPAD_BUTTON) == 0)
		{
			map.mode = InputMode::GamepadButton;
		}
		else if (type.compare(PREFIX_GAMEPAD_AXIS) == 0)
		{
			map.mode = InputMode::GamepadAxis;
		}
		else
		{
			std::cerr << "Unrecognized input mode for \"" << type << "\" = " << value << std::endl;
			return std::nullopt;
		}
		idx = value.find(DELIM_INPUT_CODE);
		std::string code;
		std::string action;
		if (idx != std::string::npos)
		{
			code = value.substr(0, idx);
			action = value.substr(idx + 1);
		}
		else
		{
			code = value;
		}
		map.code = get_code_from(code);
		map.action = action.empty() ? GLFW_PRESS : (action.compare("press") == 0 ? GLFW_PRESS : GLFW_RELEASE);
		return map;
	}

	std::string get_string_from(InputMapping mapping)
	{
		std::string buffer;
		std::string mapping_name = "";
		for (auto entry : INPUT_IDS)
		{
			if (entry.second == mapping.code)
			{
				mapping_name = entry.first;
				break;
			}
		}
		if (mapping_name.empty())
		{
			std::cerr << "failed to find corresponding input string for id " << mapping.code << std::endl;
			return "";
		}
		buffer += mapping_name + std::string(DELIM_INPUT_CODE);
		buffer += mapping.action == GLFW_PRESS ? "press" : "release";
		return buffer;
	}

	unsigned int get_code_from(std::string name)
	{
		populate_input_ids();

		auto lower = [](unsigned char c)
		{ return std::tolower(c); };
		std::string name_lower = name;
		std::transform(name.begin(), name.end(), name_lower.begin(), lower);
		if (auto search = INPUT_IDS.find(name_lower); search != INPUT_IDS.end())
		{
			return search->second;
		}
		std::cerr << "Failed to find matching input id for \"" << name << "\" (lowered: \"" << name_lower << "\")" << std::endl;

		for (unsigned int i = 0; i < SCANCODE_SEARCH_RANGE; i++)
		{
			auto value = glfwGetKeyName(GLFW_KEY_UNKNOWN, i);
			if (value == NULL)
				continue;
			auto str_value = std::string(value);
			std::string str_value_lower = "";
			std::transform(str_value.begin(), str_value.end(), str_value_lower.begin(), lower);

			if (name_lower.compare(str_value_lower) == 0)
				return i;
		}
		// search by name (slow at best T-T )

		std::cerr << "Failed to convert \"" << name << "\" to a proper key code" << std::endl;

		return 0;
	}

	void populate_input_ids()
	{
		if (!INPUT_IDS.empty())
		{
			// std::cout << "Skipping input id allocation. " << INPUT_IDS.size() << " ids already registered" << std::endl;
			return;
		}

		INPUT_IDS["key_space"] = GLFW_KEY_SPACE;
		INPUT_IDS["key_apostrophe"] = GLFW_KEY_APOSTROPHE;
		INPUT_IDS["key_comma"] = GLFW_KEY_COMMA;
		INPUT_IDS["key_minus"] = GLFW_KEY_MINUS;
		INPUT_IDS["key_period"] = GLFW_KEY_PERIOD;
		INPUT_IDS["key_slash"] = GLFW_KEY_SLASH;
		INPUT_IDS["key_0"] = GLFW_KEY_0;
		INPUT_IDS["key_1"] = GLFW_KEY_1;
		INPUT_IDS["key_2"] = GLFW_KEY_2;
		INPUT_IDS["key_3"] = GLFW_KEY_3;
		INPUT_IDS["key_4"] = GLFW_KEY_4;
		INPUT_IDS["key_5"] = GLFW_KEY_5;
		INPUT_IDS["key_6"] = GLFW_KEY_6;
		INPUT_IDS["key_7"] = GLFW_KEY_7;
		INPUT_IDS["key_8"] = GLFW_KEY_8;
		INPUT_IDS["key_9"] = GLFW_KEY_9;
		INPUT_IDS["key_semicolon"] = GLFW_KEY_SEMICOLON;
		INPUT_IDS["key_equal"] = GLFW_KEY_EQUAL;
		INPUT_IDS["key_a"] = GLFW_KEY_A;
		INPUT_IDS["key_b"] = GLFW_KEY_B;
		INPUT_IDS["key_c"] = GLFW_KEY_C;
		INPUT_IDS["key_d"] = GLFW_KEY_D;
		INPUT_IDS["key_e"] = GLFW_KEY_E;
		INPUT_IDS["key_f"] = GLFW_KEY_F;
		INPUT_IDS["key_g"] = GLFW_KEY_G;
		INPUT_IDS["key_h"] = GLFW_KEY_H;
		INPUT_IDS["key_i"] = GLFW_KEY_I;
		INPUT_IDS["key_j"] = GLFW_KEY_J;
		INPUT_IDS["key_k"] = GLFW_KEY_K;
		INPUT_IDS["key_l"] = GLFW_KEY_L;
		INPUT_IDS["key_m"] = GLFW_KEY_M;
		INPUT_IDS["key_n"] = GLFW_KEY_N;
		INPUT_IDS["key_o"] = GLFW_KEY_O;
		INPUT_IDS["key_p"] = GLFW_KEY_P;
		INPUT_IDS["key_q"] = GLFW_KEY_Q;
		INPUT_IDS["key_r"] = GLFW_KEY_R;
		INPUT_IDS["key_s"] = GLFW_KEY_S;
		INPUT_IDS["key_t"] = GLFW_KEY_T;
		INPUT_IDS["key_u"] = GLFW_KEY_U;
		INPUT_IDS["key_v"] = GLFW_KEY_V;
		INPUT_IDS["key_w"] = GLFW_KEY_W;
		INPUT_IDS["key_x"] = GLFW_KEY_X;
		INPUT_IDS["key_y"] = GLFW_KEY_Y;
		INPUT_IDS["key_z"] = GLFW_KEY_Z;
		INPUT_IDS["key_leftbracket"] = GLFW_KEY_LEFT_BRACKET;
		INPUT_IDS["key_backslash"] = GLFW_KEY_BACKSLASH;
		INPUT_IDS["key_rightbracket"] = GLFW_KEY_RIGHT_BRACKET;
		INPUT_IDS["key_grave"] = GLFW_KEY_GRAVE_ACCENT;
		INPUT_IDS["key_world1"] = GLFW_KEY_WORLD_1;
		INPUT_IDS["key_world2"] = GLFW_KEY_WORLD_2;
		INPUT_IDS["key_enter"] = GLFW_KEY_ENTER;
		INPUT_IDS["key_tab"] = GLFW_KEY_TAB;
		INPUT_IDS["key_backspace"] = GLFW_KEY_BACKSPACE;
		INPUT_IDS["key_insert"] = GLFW_KEY_INSERT;
		INPUT_IDS["key_delete"] = GLFW_KEY_DELETE;
		INPUT_IDS["key_right"] = GLFW_KEY_RIGHT;
		INPUT_IDS["key_left"] = GLFW_KEY_LEFT;
		INPUT_IDS["key_down"] = GLFW_KEY_DOWN;
		INPUT_IDS["key_up"] = GLFW_KEY_UP;
		INPUT_IDS["key_pageup"] = GLFW_KEY_PAGE_UP;
		INPUT_IDS["key_pagedown"] = GLFW_KEY_PAGE_DOWN;
		INPUT_IDS["key_home"] = GLFW_KEY_HOME;
		INPUT_IDS["key_end"] = GLFW_KEY_END;
		INPUT_IDS["key_capslock"] = GLFW_KEY_CAPS_LOCK;
		INPUT_IDS["key_scrolllock"] = GLFW_KEY_SCROLL_LOCK;
		INPUT_IDS["key_numlock"] = GLFW_KEY_NUM_LOCK;
		INPUT_IDS["key_printscreen"] = GLFW_KEY_PRINT_SCREEN;
		INPUT_IDS["key_pause"] = GLFW_KEY_PAUSE;
		INPUT_IDS["key_f1"] = GLFW_KEY_F1;
		INPUT_IDS["key_f2"] = GLFW_KEY_F2;
		INPUT_IDS["key_f3"] = GLFW_KEY_F3;
		INPUT_IDS["key_f4"] = GLFW_KEY_F4;
		INPUT_IDS["key_f5"] = GLFW_KEY_F5;
		INPUT_IDS["key_f6"] = GLFW_KEY_F6;
		INPUT_IDS["key_f7"] = GLFW_KEY_F7;
		INPUT_IDS["key_f8"] = GLFW_KEY_F8;
		INPUT_IDS["key_f9"] = GLFW_KEY_F9;
		INPUT_IDS["key_f10"] = GLFW_KEY_F10;
		INPUT_IDS["key_f11"] = GLFW_KEY_F11;
		INPUT_IDS["key_f12"] = GLFW_KEY_F12;
		INPUT_IDS["key_f13"] = GLFW_KEY_F13;
		INPUT_IDS["key_f14"] = GLFW_KEY_F14;
		INPUT_IDS["key_f15"] = GLFW_KEY_F15;
		INPUT_IDS["key_f16"] = GLFW_KEY_F16;
		INPUT_IDS["key_f17"] = GLFW_KEY_F17;
		INPUT_IDS["key_f18"] = GLFW_KEY_F18;
		INPUT_IDS["key_f19"] = GLFW_KEY_F19;
		INPUT_IDS["key_f20"] = GLFW_KEY_F20;
		INPUT_IDS["key_f21"] = GLFW_KEY_F21;
		INPUT_IDS["key_f22"] = GLFW_KEY_F22;
		INPUT_IDS["key_f23"] = GLFW_KEY_F23;
		INPUT_IDS["key_f24"] = GLFW_KEY_F24;
		INPUT_IDS["key_f25"] = GLFW_KEY_F25;
		INPUT_IDS["key_kp0"] = GLFW_KEY_KP_0;
		INPUT_IDS["key_kp1"] = GLFW_KEY_KP_1;
		INPUT_IDS["key_kp2"] = GLFW_KEY_KP_2;
		INPUT_IDS["key_kp3"] = GLFW_KEY_KP_3;
		INPUT_IDS["key_kp4"] = GLFW_KEY_KP_4;
		INPUT_IDS["key_kp5"] = GLFW_KEY_KP_5;
		INPUT_IDS["key_kp6"] = GLFW_KEY_KP_6;
		INPUT_IDS["key_kp7"] = GLFW_KEY_KP_7;
		INPUT_IDS["key_kp8"] = GLFW_KEY_KP_8;
		INPUT_IDS["key_kp9"] = GLFW_KEY_KP_9;
		INPUT_IDS["key_kpdecimal"] = GLFW_KEY_KP_DECIMAL;
		INPUT_IDS["key_kpdivide"] = GLFW_KEY_KP_DIVIDE;
		INPUT_IDS["key_kpmultiply"] = GLFW_KEY_KP_MULTIPLY;
		INPUT_IDS["key_kpsubtract"] = GLFW_KEY_KP_SUBTRACT;
		INPUT_IDS["key_kpadd"] = GLFW_KEY_KP_ADD;
		INPUT_IDS["key_kpenter"] = GLFW_KEY_KP_ENTER;
		INPUT_IDS["key_kpequal"] = GLFW_KEY_KP_EQUAL;
		INPUT_IDS["key_leftshift"] = GLFW_KEY_LEFT_SHIFT;
		INPUT_IDS["key_leftcontrol"] = GLFW_KEY_LEFT_CONTROL;
		INPUT_IDS["key_leftalt"] = GLFW_KEY_LEFT_ALT;
		INPUT_IDS["key_leftsuper"] = GLFW_KEY_LEFT_SUPER;
		INPUT_IDS["key_rightshift"] = GLFW_KEY_RIGHT_SHIFT;
		INPUT_IDS["key_rightcontrol"] = GLFW_KEY_RIGHT_CONTROL;
		INPUT_IDS["key_rightalt"] = GLFW_KEY_RIGHT_ALT;
		INPUT_IDS["key_rightsuper"] = GLFW_KEY_RIGHT_SUPER;
		INPUT_IDS["key_menu"] = GLFW_KEY_MENU;

		INPUT_IDS["joy_btn_a"] = GLFW_GAMEPAD_BUTTON_A;
		INPUT_IDS["joy_btn_b"] = GLFW_GAMEPAD_BUTTON_B;
		INPUT_IDS["joy_btn_x"] = GLFW_GAMEPAD_BUTTON_X;
		INPUT_IDS["joy_btn_y"] = GLFW_GAMEPAD_BUTTON_Y;
		INPUT_IDS["joy_btn_leftbumper"] = GLFW_GAMEPAD_BUTTON_LEFT_BUMPER;
		INPUT_IDS["joy_btn_rightbumper"] = GLFW_GAMEPAD_BUTTON_RIGHT_BUMPER;
		INPUT_IDS["joy_btn_back"] = GLFW_GAMEPAD_BUTTON_BACK;
		INPUT_IDS["joy_btn_start"] = GLFW_GAMEPAD_BUTTON_START;
		INPUT_IDS["joy_btn_guide"] = GLFW_GAMEPAD_BUTTON_GUIDE;
		INPUT_IDS["joy_btn_leftthumb"] = GLFW_GAMEPAD_BUTTON_LEFT_THUMB;
		INPUT_IDS["joy_btn_rightthumb"] = GLFW_GAMEPAD_BUTTON_RIGHT_THUMB;
		INPUT_IDS["joy_btn_dpadup"] = GLFW_GAMEPAD_BUTTON_DPAD_UP;
		INPUT_IDS["joy_btn_dpadright"] = GLFW_GAMEPAD_BUTTON_DPAD_RIGHT;
		INPUT_IDS["joy_btn_dpaddown"] = GLFW_GAMEPAD_BUTTON_DPAD_DOWN;
		INPUT_IDS["joy_btn_dpadleft"] = GLFW_GAMEPAD_BUTTON_DPAD_LEFT;
		INPUT_IDS["joy_btn_cross"] = GLFW_GAMEPAD_BUTTON_CROSS;
		INPUT_IDS["joy_btn_circle"] = GLFW_GAMEPAD_BUTTON_CIRCLE;
		INPUT_IDS["joy_btn_square"] = GLFW_GAMEPAD_BUTTON_SQUARE;
		INPUT_IDS["joy_btn_triangle"] = GLFW_GAMEPAD_BUTTON_TRIANGLE;
		INPUT_IDS["joy_axis_leftx"] = GLFW_GAMEPAD_AXIS_LEFT_X;
		INPUT_IDS["joy_axis_lefty"] = GLFW_GAMEPAD_AXIS_LEFT_Y;
		INPUT_IDS["joy_axis_rightx"] = GLFW_GAMEPAD_AXIS_RIGHT_X;
		INPUT_IDS["joy_axis_righty"] = GLFW_GAMEPAD_AXIS_RIGHT_Y;
		INPUT_IDS["joy_axis_lefttrigger"] = GLFW_GAMEPAD_AXIS_LEFT_TRIGGER;
		INPUT_IDS["joy_axis_righttrigger"] = GLFW_GAMEPAD_AXIS_RIGHT_TRIGGER;

		INPUT_IDS["m_1"] = GLFW_MOUSE_BUTTON_1;
		INPUT_IDS["m_2"] = GLFW_MOUSE_BUTTON_2;
		INPUT_IDS["m_3"] = GLFW_MOUSE_BUTTON_3;
		INPUT_IDS["m_4"] = GLFW_MOUSE_BUTTON_4;
		INPUT_IDS["m_5"] = GLFW_MOUSE_BUTTON_5;
		INPUT_IDS["m_6"] = GLFW_MOUSE_BUTTON_6;
		INPUT_IDS["m_7"] = GLFW_MOUSE_BUTTON_7;
		INPUT_IDS["m_8"] = GLFW_MOUSE_BUTTON_8;
		INPUT_IDS["m_left"] = GLFW_MOUSE_BUTTON_LEFT;
		INPUT_IDS["m_right"] = GLFW_MOUSE_BUTTON_RIGHT;
		INPUT_IDS["m_middle"] = GLFW_MOUSE_BUTTON_MIDDLE;
	}
}