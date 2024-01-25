
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#define STB_IMAGE_IMPLEMENTATION
#include <stb_image.h>

#include <iostream>

#include "game_object/game_object.hpp"
#include "window/window.hpp"

using namespace std;
int main()
{
	stbi_set_flip_vertically_on_load(true);
	cout << "Initalizing GLFW" << endl;
	if (!glfwInit())
	{
		cerr << "Failed to intialize GLFW" << endl;
		return -1;
	}

	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 6);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

	cout << "Creating window" << endl;
	GameWindow window(800, 600, "Game Window");
	if (!window.is_valid())
	{
		cerr << "Failed to initialize window!" << endl;
		return -1;
	}

	cout << "Initalizing GLAD" << endl;
	if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
	{
		cerr << "Failed to initialize GLAD GL Loader" << endl;
		glfwTerminate();
		return -1;
	}

	cout << "Creating Game Data:" << endl;
	std::vector<float> vertices{
		0.5f, 0.5f, 0.0f, 1.0f, 0.0f, 0.0f, 1.0f, 1.0f, 0.5f, -0.5f, 0.0f,
		0.0f, 1.0f, 0.0f, 1.0f, 0.0f, -0.5f, -0.5f, 0.0f, 0.0f, 0.0f, 1.0f,
		0.0f, 0.0f, -0.5f, 0.5f, 0.0f, 1.0f, 1.0f, 0.0f, 0.0f, 1.0f};
	std::vector<unsigned int> indices{0, 1, 3, 1, 2, 3};
	cout << " - Vertex data and Index data" << endl;

	Texture albedo("./res/texture/wall.jpg");
	Texture normal("./res/texture/wall.jpg");
	Texture orm("./res/texture/wall.jpg");

	cout << " - Textures" << endl;
	ShaderProgram program("./res/default.vert", "./res/default.frag");
	cout << " - Shader Program" << endl;
	Material mat(&program, &albedo, &normal, &orm);
	cout << " - Material data" << endl;

	std::vector<VertexDataAttribute> attribs{

		{VertexDataAttributeType::FLOAT, 3}, // position
		{VertexDataAttributeType::FLOAT, 3}, // colour
		{VertexDataAttributeType::FLOAT, 2}, // uvs
	};

	MeshSurface surf(vertices, indices, attribs, &mat);
	cout << " - MeshSurface (with attribute pointers)" << endl;
	Mesh mesh(std::vector<MeshSurface *>{
		&surf,
	});
	cout << " - Mesh" << endl;
	GameObject obj(&mesh);
	cout << " - Game Object" << endl;

	window.renderer.register_batch("default", &program);
	window.renderer.register_game_object("default", &obj);
	cout << " - Registered with Window::renderer" << endl;

	cout << "Starting game loop" << endl;
	double time = glfwGetTime();
	double delta = 0;
	while (window.is_running())
	{
		double now = glfwGetTime();
		delta = now - time;
		time = now;

		obj.rotation.x += glm::radians(45.0) * delta;
		window.game_tick();
	}
	cout << "Game loop completed" << endl;
	return 0;
}