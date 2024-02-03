#include <imgui.h>
#include <backends/imgui_impl_glfw.h>
#include <backends/imgui_impl_opengl3.h>

#include "engine.hpp"

using namespace std;

// "implementation" of instance variable
std::shared_ptr<Engine> Engine::instance = nullptr;

Engine::Engine()
{

	stbi_set_flip_vertically_on_load(true);
	if (!glfwInit())
	{
		cerr << "Failed to intialize GLFW" << endl;
		return;
	}

	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 6);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

	// TODO extend to allow setting the window size at initialization
	window = std::shared_ptr<GameWindow>(new GameWindow(800, 600, "Game Window"));
	if (!window->is_valid())
	{
		cerr << "Failed to initialize window!" << endl;
		return;
	}

	if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
	{
		cerr << "Failed to initialize GLAD GL Loader" << endl;
		glfwTerminate();
		return;
	}
	add_render_group("default");
	// window->renderer->register_batch("default", std::shared_ptr<ShaderProgram>(new ShaderProgram("./res/default.vert", "./res/default.frag")));
	glfwSetFramebufferSizeCallback(window->get(), window_resize_callback);
	Engine::instance = std::shared_ptr<Engine>(this);
	input = std::shared_ptr<input::Input>(new input::Input());
	glfwSetErrorCallback(glfw_error_callback);
	// gl modes
	glEnable(GL_DEPTH_TEST);
	glEnable(GL_CULL_FACE);
	glCullFace(GL_BACK);

	window_resize_callback(window->get(), 800, 600);

	// Create IMGUI
	IMGUI_CHECKVERSION();
	ImGui::CreateContext();
	auto io = ImGui::GetIO();
	// (void)io; // from the example?? Why???
	io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;
	io.ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;
	ImGui::StyleColorsDark();

	ImGui_ImplGlfw_InitForOpenGL(window->get(), true);
	ImGui_ImplOpenGL3_Init("#version 460 core");
}

void Engine::load_object(std::shared_ptr<GameObject> obj, std::string render_group)
{
	window->renderer->register_game_object(render_group, obj);
}

void Engine::add_render_group(std::string name)
{
	window->renderer->register_batch(name, std::shared_ptr<ShaderProgram>(new ShaderProgram("res/shader/" + name + ".vert", "res/shader/" + name + ".frag")));
}

void Engine::start(std::function<void(double)> game_tick_func)
{
	double time = glfwGetTime();
	delta = 0;
	while (window->is_running())
	{
		current_time = glfwGetTime();
		delta = current_time - time;
		time = current_time;
		input->poll_input_events(window->get());
		window->game_tick(delta);
		if (game_tick_func)
			game_tick_func(delta);
		else
			std::cout << "INVALID GAME LOOP!!!!!" << std::endl;
	}
	// // Cleanup
	// std::cout << "Cleaning up IMGUI" << std::endl;
	ImGui_ImplOpenGL3_Shutdown();
	ImGui_ImplGlfw_Shutdown();
	ImGui::DestroyContext();
}

void Engine::quit()
{
	window->close();
}

void window_resize_callback(GLFWwindow *window, int width, int height)
{
	std::cout << "Window resized: " << width << "x" << height << std::endl;
	if (Engine::instance == nullptr)
	{
		std::cerr << "Failed to handle window resize callback!!!!" << std::endl;
		return;
	}
	Engine::instance->window->on_resize(width, height);
}

void glfw_error_callback(int error, const char *description)
{
	std::cout << "GLFW Error: #" << error << ":\r" << description << std::endl;
}