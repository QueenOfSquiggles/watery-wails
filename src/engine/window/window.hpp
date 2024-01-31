#pragma once

// #include "render_system/render.hpp"
#include "../render_system/render.hpp"

#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <string>
#include <vector>
#include <iostream>
#include <memory>

class GameWindow
{
	GLFWwindow *window;

public:
	int width, height;
	std::shared_ptr<Renderer> renderer;
	GameWindow(int width, int height, std::string title);
	~GameWindow();

	void game_tick(double delta);
	bool is_valid();
	bool is_running();

	/// A fully blocking function that just uses the public API to run the game loop to completion
	void run_game_loop_synchronous();

	// An internal callback
	void on_resize(int width, int height);
	GLFWwindow *get();

	void close();
};