#pragma once
#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include "window/window.hpp"
#include "game_object/game_object.hpp"
#include "render_system/render.hpp"
#include "input_system/inputs.hpp"
#include <iostream>
#include <stb_image.h>
#include <memory>
#include <functional>

class Engine
{
public:
	std::shared_ptr<GameWindow> window;
	std::shared_ptr<input::Input> input;

	static std::shared_ptr<Engine> instance;
	double delta;
	double current_time;

	Engine();
	Engine(const Engine &engine_copy) = delete;
	void start(std::function<void(double)> game_tick_func);
	void load_object(std::shared_ptr<GameObject> obj, std::string render_group = "default");
	void add_render_group(std::string name);
	void quit();
};

void window_resize_callback(GLFWwindow *window, int width, int height);