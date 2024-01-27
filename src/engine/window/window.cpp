#include "window.hpp"

using namespace std;

GameWindow::GameWindow(int width, int height, std::string title)
	: width(width), height(height), renderer(new Renderer())
{

	window = std::shared_ptr<GLFWwindow>(glfwCreateWindow(width, height, title.c_str(), NULL, NULL));
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
	glfwMakeContextCurrent(window.get());
	glfwSetFramebufferSizeCallback(window.get(), callback);
}
void GameWindow::on_resize(int width, int height)
{
	this->width = width;
	this->height = height;
	this->renderer->on_window_resized(width, height);
}

void GameWindow::game_tick(double delta)
{
	glClearColor(0, 0, 0, 1);
	glClear(GL_COLOR_BUFFER_BIT);

	this->renderer->render(delta, glfwGetTime());

	glfwSwapBuffers(window.get());
	glfwPollEvents();
}

void GameWindow::run_game_loop_synchronous()
{
	if (!this->is_valid())
	{
		return;
	}
	glViewport(0, 0, this->width, this->height);
	double last = glfwGetTime();
	double delta = 0;
	while (this->is_running())
	{
		double now = glfwGetTime();
		delta = now - last;
		last = now;
		this->game_tick(delta);
	}
}

bool GameWindow::is_valid()
{
	return window != NULL;
}
bool GameWindow::is_running()
{
	return !glfwWindowShouldClose(this->window.get());
}

GLFWwindow *GameWindow::get() { return window.get(); }

void GameWindow::close()
{
	glfwSetWindowShouldClose(window.get(), GLFW_TRUE);
}
