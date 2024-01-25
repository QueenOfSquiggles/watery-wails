#pragma once

#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <string>
#include <vector>
#include "render/render.hpp"

class GameWindow
{
	GLFWwindow *window;

public:
	int width, height;
	Renderer renderer;
	GameWindow(int width, int height, std::string title);
	~GameWindow();

	void game_tick();
	bool is_valid();
	bool is_running();

	/// A fully blocking function that just uses the public API to run the game loop to completion
	void run_game_loop_synchronous();

	// An internal callback
	void on_resize(GLFWwindow *window, int width, int height);
	GLFWwindow *get();
};