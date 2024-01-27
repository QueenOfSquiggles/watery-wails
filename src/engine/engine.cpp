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
	window->renderer->register_batch("default", std::shared_ptr<ShaderProgram>(new ShaderProgram("./res/default.vert", "./res/default.frag")));
	glfwSetFramebufferSizeCallback(window->get(), window_resize_callback);
	Engine::instance = std::shared_ptr<Engine>(this);
	input = std::shared_ptr<input::Input>(new input::Input());
}

void Engine::load_object(std::shared_ptr<GameObject> obj)
{
	window->renderer->register_game_object("default", obj);
}

void Engine::start(std::function<void(double)> game_tick_func)
{
	double time = glfwGetTime();
	delta = 0;
	while (window->is_running())
	{
		double now = glfwGetTime();
		delta = now - time;
		time = now;
		input->poll_input_events(window->get());
		window->game_tick(delta);
		if (game_tick_func)
			game_tick_func(delta);
	}
}

void window_resize_callback(GLFWwindow *window, int width, int height)
{
	if (Engine::instance == nullptr)
	{
		return;
	}
	Engine::instance->window->on_resize(width, height);
}
void Engine::quit()
{
	window->close();
}
