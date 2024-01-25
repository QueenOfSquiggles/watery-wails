#include "window.hpp"
#include <iostream>
#include <functional>

using namespace std;

GameWindow::GameWindow(int width, int height, std::string title)
	: width(width), height(height)
{

	window = glfwCreateWindow(width, height, title.c_str(), NULL, NULL);
	if (window == NULL)
	{
		cerr << "Failed to initialize GLFWwindow*" << endl;
		glfwTerminate();
		return;
	}
	auto callback = [](GLFWwindow *w, int width, int height)
	{
		glViewport(0, 0, width, height);
		// this->on_resize(w, width, height);
	};
	glfwMakeContextCurrent(window);
	glfwSetFramebufferSizeCallback(window, callback);
}
void GameWindow::on_resize(GLFWwindow *window, int width, int height)
{
	this->width = width;
	this->height = height;
}

void GameWindow::game_tick()
{
	glClearColor(0, 0, 0, 1);
	glClear(GL_COLOR_BUFFER_BIT);

	this->renderer.render();

	glfwSwapBuffers(window);
	glfwPollEvents();
}

void GameWindow::run_game_loop_synchronous()
{
	if (!this->is_valid())
	{
		return;
	}
	glViewport(0, 0, this->width, this->height);

	while (this->is_running())
	{
		this->game_tick();
	}
}

GameWindow::~GameWindow()
{
}
bool GameWindow::is_valid()
{
	return window != NULL;
}
bool GameWindow::is_running()
{
	return !glfwWindowShouldClose(this->window);
}

GLFWwindow *GameWindow::get() { return window; }
