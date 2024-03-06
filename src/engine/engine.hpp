#pragma once
#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include "window/window.hpp"
#include "game_object/game_object.hpp"
#include "render_system/render.hpp"
#include "input_system/inputs.hpp"
#include "audio_system/audio.hpp"
#include <iostream>
#include <stb_image.h>
#include <memory>
#include <functional>
#include <deque>

class Engine
{
public:
	std::shared_ptr<GameWindow> window;
	std::shared_ptr<input::Input> input;
	std::shared_ptr<AudioSystem> audio;
	static std::shared_ptr<Engine> instance;
	std::function<void(double)> game_tick_func;
	unsigned int FPS = 0;
	double delta;
	double current_time;

	Engine();
	Engine(const Engine &engine_copy) = delete;
	void start();
	void load_object(std::shared_ptr<GameObject> obj, std::string render_group = "default");
	void add_render_group(std::string name);
	void add_render_group(std::string name, std::string shader_path, bool preprocess, std::function<void(RenderContext)> setup_callback);
	void add_renderable(std::string batch, std::shared_ptr<Renderable> renderable);

	void debug_print_renderable_data();

	void quit();
};

void window_resize_callback(GLFWwindow *window, int width, int height);
void glfw_error_callback(int error, const char *description);