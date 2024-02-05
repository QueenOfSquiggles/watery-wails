#include "input_map_conversion.hpp"

namespace input
{

	std::optional<InputMapping> load_mapping(std::string key)
	{
		auto idx = key.find(DELIM_MODE_PREFIX);
		auto type = key.substr(0, idx);
		auto value = key.substr(idx + 1);
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
		if (!action.empty())
		{
			map.action = action.compare("press") == 0 ? GLFW_PRESS : GLFW_RELEASE;
		}
		return map;
	}

	std::string get_string_from(InputMapping mapping)
	{
		std::string buffer;
		switch (mapping.mode)
		{
		case InputMode::Key:
			buffer += PREFIX_KEY;
			break;
		case InputMode::MouseButton:
			buffer += PREFIX_KEY;
			break;
		case InputMode::GamepadButton:
			buffer += PREFIX_KEY;
			break;
		case InputMode::GamepadAxis:
			buffer += PREFIX_KEY;
			break;
		default:
			std::cerr << "Failed to find a valid string version of InputMapping {" << mapping.code << ", " << mapping.mode << ", " << mapping.action << "}" << std::endl;
			return "";
		}
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
		buffer += std::string(DELIM_MODE_PREFIX) + std::to_string(mapping.code) + std::string(DELIM_INPUT_CODE);
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

		INPUT_IDS["space"] = GLFW_KEY_SPACE;
		INPUT_IDS["apostrophe"] = GLFW_KEY_APOSTROPHE;
		INPUT_IDS["comma"] = GLFW_KEY_COMMA;
		INPUT_IDS["minus"] = GLFW_KEY_MINUS;
		INPUT_IDS["period"] = GLFW_KEY_PERIOD;
		INPUT_IDS["slash"] = GLFW_KEY_SLASH;
		INPUT_IDS["kb0"] = GLFW_KEY_0;
		INPUT_IDS["kb1"] = GLFW_KEY_1;
		INPUT_IDS["kb2"] = GLFW_KEY_2;
		INPUT_IDS["kb3"] = GLFW_KEY_3;
		INPUT_IDS["kb4"] = GLFW_KEY_4;
		INPUT_IDS["kb5"] = GLFW_KEY_5;
		INPUT_IDS["kb6"] = GLFW_KEY_6;
		INPUT_IDS["kb7"] = GLFW_KEY_7;
		INPUT_IDS["kb8"] = GLFW_KEY_8;
		INPUT_IDS["kb9"] = GLFW_KEY_9;
		INPUT_IDS["semicolon"] = GLFW_KEY_SEMICOLON;
		INPUT_IDS["equal"] = GLFW_KEY_EQUAL;
		INPUT_IDS["a"] = GLFW_KEY_A;
		INPUT_IDS["b"] = GLFW_KEY_B;
		INPUT_IDS["c"] = GLFW_KEY_C;
		INPUT_IDS["d"] = GLFW_KEY_D;
		INPUT_IDS["e"] = GLFW_KEY_E;
		INPUT_IDS["f"] = GLFW_KEY_F;
		INPUT_IDS["g"] = GLFW_KEY_G;
		INPUT_IDS["h"] = GLFW_KEY_H;
		INPUT_IDS["i"] = GLFW_KEY_I;
		INPUT_IDS["j"] = GLFW_KEY_J;
		INPUT_IDS["k"] = GLFW_KEY_K;
		INPUT_IDS["l"] = GLFW_KEY_L;
		INPUT_IDS["m"] = GLFW_KEY_M;
		INPUT_IDS["n"] = GLFW_KEY_N;
		INPUT_IDS["o"] = GLFW_KEY_O;
		INPUT_IDS["p"] = GLFW_KEY_P;
		INPUT_IDS["q"] = GLFW_KEY_Q;
		INPUT_IDS["r"] = GLFW_KEY_R;
		INPUT_IDS["s"] = GLFW_KEY_S;
		INPUT_IDS["t"] = GLFW_KEY_T;
		INPUT_IDS["u"] = GLFW_KEY_U;
		INPUT_IDS["v"] = GLFW_KEY_V;
		INPUT_IDS["w"] = GLFW_KEY_W;
		INPUT_IDS["x"] = GLFW_KEY_X;
		INPUT_IDS["y"] = GLFW_KEY_Y;
		INPUT_IDS["z"] = GLFW_KEY_Z;
		INPUT_IDS["leftbracket"] = GLFW_KEY_LEFT_BRACKET;
		INPUT_IDS["backslash"] = GLFW_KEY_BACKSLASH;
		INPUT_IDS["rightbracket"] = GLFW_KEY_RIGHT_BRACKET;
		INPUT_IDS["grave"] = GLFW_KEY_GRAVE_ACCENT;
		INPUT_IDS["world1"] = GLFW_KEY_WORLD_1;
		INPUT_IDS["world2"] = GLFW_KEY_WORLD_2;
		INPUT_IDS["enter"] = GLFW_KEY_ENTER;
		INPUT_IDS["tab"] = GLFW_KEY_TAB;
		INPUT_IDS["backspace"] = GLFW_KEY_BACKSPACE;
		INPUT_IDS["insert"] = GLFW_KEY_INSERT;
		INPUT_IDS["delete"] = GLFW_KEY_DELETE;
		INPUT_IDS["right"] = GLFW_KEY_RIGHT;
		INPUT_IDS["left"] = GLFW_KEY_LEFT;
		INPUT_IDS["down"] = GLFW_KEY_DOWN;
		INPUT_IDS["up"] = GLFW_KEY_UP;
		INPUT_IDS["pageup"] = GLFW_KEY_PAGE_UP;
		INPUT_IDS["pagedown"] = GLFW_KEY_PAGE_DOWN;
		INPUT_IDS["home"] = GLFW_KEY_HOME;
		INPUT_IDS["end"] = GLFW_KEY_END;
		INPUT_IDS["capslock"] = GLFW_KEY_CAPS_LOCK;
		INPUT_IDS["scrolllock"] = GLFW_KEY_SCROLL_LOCK;
		INPUT_IDS["numlock"] = GLFW_KEY_NUM_LOCK;
		INPUT_IDS["printscreen"] = GLFW_KEY_PRINT_SCREEN;
		INPUT_IDS["pause"] = GLFW_KEY_PAUSE;
		INPUT_IDS["f1"] = GLFW_KEY_F1;
		INPUT_IDS["f2"] = GLFW_KEY_F2;
		INPUT_IDS["f3"] = GLFW_KEY_F3;
		INPUT_IDS["f4"] = GLFW_KEY_F4;
		INPUT_IDS["f5"] = GLFW_KEY_F5;
		INPUT_IDS["f6"] = GLFW_KEY_F6;
		INPUT_IDS["f7"] = GLFW_KEY_F7;
		INPUT_IDS["f8"] = GLFW_KEY_F8;
		INPUT_IDS["f9"] = GLFW_KEY_F9;
		INPUT_IDS["f10"] = GLFW_KEY_F10;
		INPUT_IDS["f11"] = GLFW_KEY_F11;
		INPUT_IDS["f12"] = GLFW_KEY_F12;
		INPUT_IDS["f13"] = GLFW_KEY_F13;
		INPUT_IDS["f14"] = GLFW_KEY_F14;
		INPUT_IDS["f15"] = GLFW_KEY_F15;
		INPUT_IDS["f16"] = GLFW_KEY_F16;
		INPUT_IDS["f17"] = GLFW_KEY_F17;
		INPUT_IDS["f18"] = GLFW_KEY_F18;
		INPUT_IDS["f19"] = GLFW_KEY_F19;
		INPUT_IDS["f20"] = GLFW_KEY_F20;
		INPUT_IDS["f21"] = GLFW_KEY_F21;
		INPUT_IDS["f22"] = GLFW_KEY_F22;
		INPUT_IDS["f23"] = GLFW_KEY_F23;
		INPUT_IDS["f24"] = GLFW_KEY_F24;
		INPUT_IDS["f25"] = GLFW_KEY_F25;
		INPUT_IDS["kp0"] = GLFW_KEY_KP_0;
		INPUT_IDS["kp1"] = GLFW_KEY_KP_1;
		INPUT_IDS["kp2"] = GLFW_KEY_KP_2;
		INPUT_IDS["kp3"] = GLFW_KEY_KP_3;
		INPUT_IDS["kp4"] = GLFW_KEY_KP_4;
		INPUT_IDS["kp5"] = GLFW_KEY_KP_5;
		INPUT_IDS["kp6"] = GLFW_KEY_KP_6;
		INPUT_IDS["kp7"] = GLFW_KEY_KP_7;
		INPUT_IDS["kp8"] = GLFW_KEY_KP_8;
		INPUT_IDS["kp9"] = GLFW_KEY_KP_9;
		INPUT_IDS["kpdecimal"] = GLFW_KEY_KP_DECIMAL;
		INPUT_IDS["kpdivide"] = GLFW_KEY_KP_DIVIDE;
		INPUT_IDS["kpmultiply"] = GLFW_KEY_KP_MULTIPLY;
		INPUT_IDS["kpsubtract"] = GLFW_KEY_KP_SUBTRACT;
		INPUT_IDS["kpadd"] = GLFW_KEY_KP_ADD;
		INPUT_IDS["kpenter"] = GLFW_KEY_KP_ENTER;
		INPUT_IDS["kpequal"] = GLFW_KEY_KP_EQUAL;
		INPUT_IDS["leftshift"] = GLFW_KEY_LEFT_SHIFT;
		INPUT_IDS["leftcontrol"] = GLFW_KEY_LEFT_CONTROL;
		INPUT_IDS["leftalt"] = GLFW_KEY_LEFT_ALT;
		INPUT_IDS["leftsuper"] = GLFW_KEY_LEFT_SUPER;
		INPUT_IDS["rightshift"] = GLFW_KEY_RIGHT_SHIFT;
		INPUT_IDS["rightcontrol"] = GLFW_KEY_RIGHT_CONTROL;
		INPUT_IDS["rightalt"] = GLFW_KEY_RIGHT_ALT;
		INPUT_IDS["rightsuper"] = GLFW_KEY_RIGHT_SUPER;
		INPUT_IDS["menu"] = GLFW_KEY_MENU;

		INPUT_IDS["joybtna"] = GLFW_GAMEPAD_BUTTON_A;
		INPUT_IDS["joybtnb"] = GLFW_GAMEPAD_BUTTON_B;
		INPUT_IDS["joybtnx"] = GLFW_GAMEPAD_BUTTON_X;
		INPUT_IDS["joybtny"] = GLFW_GAMEPAD_BUTTON_Y;
		INPUT_IDS["joybtnleftbumper"] = GLFW_GAMEPAD_BUTTON_LEFT_BUMPER;
		INPUT_IDS["joybtnrightbumper"] = GLFW_GAMEPAD_BUTTON_RIGHT_BUMPER;
		INPUT_IDS["joybtnback"] = GLFW_GAMEPAD_BUTTON_BACK;
		INPUT_IDS["joybtnstart"] = GLFW_GAMEPAD_BUTTON_START;
		INPUT_IDS["joybtnguide"] = GLFW_GAMEPAD_BUTTON_GUIDE;
		INPUT_IDS["joybtnleftthumb"] = GLFW_GAMEPAD_BUTTON_LEFT_THUMB;
		INPUT_IDS["joybtnrightthumb"] = GLFW_GAMEPAD_BUTTON_RIGHT_THUMB;
		INPUT_IDS["joybtndpadup"] = GLFW_GAMEPAD_BUTTON_DPAD_UP;
		INPUT_IDS["joybtndpadright"] = GLFW_GAMEPAD_BUTTON_DPAD_RIGHT;
		INPUT_IDS["joybtndpaddown"] = GLFW_GAMEPAD_BUTTON_DPAD_DOWN;
		INPUT_IDS["joybtndpadleft"] = GLFW_GAMEPAD_BUTTON_DPAD_LEFT;
		INPUT_IDS["joybtncross"] = GLFW_GAMEPAD_BUTTON_CROSS;
		INPUT_IDS["joybtncircle"] = GLFW_GAMEPAD_BUTTON_CIRCLE;
		INPUT_IDS["joybtnsquare"] = GLFW_GAMEPAD_BUTTON_SQUARE;
		INPUT_IDS["joybtntriangle"] = GLFW_GAMEPAD_BUTTON_TRIANGLE;
		INPUT_IDS["joyaxisleftx"] = GLFW_GAMEPAD_AXIS_LEFT_X;
		INPUT_IDS["joyaxislefty"] = GLFW_GAMEPAD_AXIS_LEFT_Y;
		INPUT_IDS["joyaxisrightx"] = GLFW_GAMEPAD_AXIS_RIGHT_X;
		INPUT_IDS["joyaxisrighty"] = GLFW_GAMEPAD_AXIS_RIGHT_Y;
		INPUT_IDS["joyaxislefttrigger"] = GLFW_GAMEPAD_AXIS_LEFT_TRIGGER;
		INPUT_IDS["joyaxisrighttrigger"] = GLFW_GAMEPAD_AXIS_RIGHT_TRIGGER;

		INPUT_IDS["m1"] = GLFW_MOUSE_BUTTON_1;
		INPUT_IDS["m2"] = GLFW_MOUSE_BUTTON_2;
		INPUT_IDS["m3"] = GLFW_MOUSE_BUTTON_3;
		INPUT_IDS["m4"] = GLFW_MOUSE_BUTTON_4;
		INPUT_IDS["m5"] = GLFW_MOUSE_BUTTON_5;
		INPUT_IDS["m6"] = GLFW_MOUSE_BUTTON_6;
		INPUT_IDS["m7"] = GLFW_MOUSE_BUTTON_7;
		INPUT_IDS["m8"] = GLFW_MOUSE_BUTTON_8;
		INPUT_IDS["mleft"] = GLFW_MOUSE_BUTTON_LEFT;
		INPUT_IDS["mright"] = GLFW_MOUSE_BUTTON_RIGHT;
		INPUT_IDS["mmiddle"] = GLFW_MOUSE_BUTTON_MIDDLE;
	}
}