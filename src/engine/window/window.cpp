#include <imgui.h>
#include <backends/imgui_impl_glfw.h>
#include <backends/imgui_impl_opengl3.h>
#include "window.hpp"

using namespace std;

GameWindow::GameWindow(int width, int height, std::string title)
	: width(width), height(height), renderer(new Renderer())
{

	window = glfwCreateWindow(width, height, title.c_str(), NULL, NULL);
	if (!window)
	{
		cerr << "Failed to initialize GLFWwindow*" << endl;
		glfwTerminate();
		return;
	}
	glfwMakeContextCurrent(window);
}

GameWindow::~GameWindow()
{
	glfwDestroyWindow(window);
	glfwTerminate();
	// delete window;
}

void GameWindow::on_resize(int width, int height)
{
	this->width = width;
	this->height = height;
	this->renderer->on_window_resized(width, height);
	glViewport(0, 0, this->width, this->height);
}

void GameWindow::game_tick(double delta)
{
	bool draw_imgui = (bool)debug_draw;
	glfwPollEvents();
	if (draw_imgui)
	{
		ImGui_ImplOpenGL3_NewFrame();
		ImGui_ImplGlfw_NewFrame();
		ImGui::NewFrame();
		debug_draw();
		ImGui::Render();
	}

	glClearColor(0, 0, 0, 1);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	this->renderer->render(delta, glfwGetTime());

	if (draw_imgui)
	{
		ImGui_ImplOpenGL3_RenderDrawData((ImDrawData *)ImGui::GetDrawData());
	}
	glfwSwapBuffers(window);
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
	return !glfwWindowShouldClose(this->window);
}

GLFWwindow *GameWindow::get() { return window; }

void GameWindow::close()
{
	glfwSetWindowShouldClose(window, GLFW_TRUE);
}
