#include "engine/engine.hpp"
#include <iostream>
#include <memory>
using namespace std;
using namespace input;
class MyObj : public GameObject
{
public:
	MyObj(shared_ptr<Mesh> mesh) : GameObject(mesh) {}
	void render(RenderContext ctx) override
	{
		transform.rotation.z += glm::radians(45.0) * ctx.delta;
		GameObject::render(ctx);
	}
};

int main()
{
	Engine *engine = new Engine();

	map<string, vector<InputMapping>> mappings = {
		{
			"forward",
			{
				{Key::W, Mode::PRESSED},
				{Key::UP, Mode::PRESSED},
			},
		},
		{
			"back",
			{
				{Key::S, Mode::PRESSED},
				{Key::DOWN, Mode::PRESSED},
			},
		},
		{
			"left",
			{
				{Key::A, Mode::PRESSED},
				{Key::LEFT, Mode::PRESSED},
			},
		},
		{
			"right",
			{
				{Key::D, Mode::PRESSED},
				{Key::RIGHT, Mode::PRESSED},
			},
		},
		{
			"force_quit",
			{
				{Key::F8, Mode::PRESSED},
			},
		},
		{
			"rotate_left",
			{
				{Key::Q, Mode::PRESSED},
			},
		},
		{
			"rotate_right",
			{
				{Key::E, Mode::PRESSED},
			},
		},
	};

	engine->input->register_actions(mappings);
	//
	//	CONVERT TO MODEL LOADING CODE
	// + + + + + + + + + + + + + + + +
	std::vector<float> vertices{
		0.5f, 0.5f, 0.0f, 1.0f, 0.0f, 0.0f, 1.0f, 1.0f, 0.5f, -0.5f, 0.0f,
		0.0f, 1.0f, 0.0f, 1.0f, 0.0f, -0.5f, -0.5f, 0.0f, 0.0f, 0.0f, 1.0f,
		0.0f, 0.0f, -0.5f, 0.5f, 0.0f, 1.0f, 1.0f, 0.0f, 0.0f, 1.0f};
	std::vector<unsigned int> indices{0, 1, 3, 1, 2, 3};

	auto albedo = shared_ptr<Texture>(new Texture("./res/texture/wall.jpg"));
	auto normal = shared_ptr<Texture>(new Texture("./res/texture/awesomeface.png"));
	auto orm = shared_ptr<Texture>(new Texture("./res/texture/wall.jpg"));
	auto program = engine->window->renderer->get_program_for("default");

	if (program == nullptr)
	{
		cerr << "Failed to load default shader program!" << endl;
	}
	auto mat = shared_ptr<Material>(new Material(program, albedo, normal, orm));

	std::vector<VertexDataAttribute> attribs{

		{VertexDataAttributeType::FLOAT, 3}, // position
		{VertexDataAttributeType::FLOAT, 3}, // colour
		{VertexDataAttributeType::FLOAT, 2}, // uvs
	};

	auto surf = shared_ptr<MeshSurface>(new MeshSurface(vertices, indices, attribs, mat));

	auto mesh = shared_ptr<Mesh>(new Mesh(
		std::vector<shared_ptr<MeshSurface>>{
			surf,
		}));
	// + + + + + + + + + + + + + + + +
	// /CONVERT TO MODEL LOADING CODE
	//

	auto obj = std::shared_ptr<MyObj>(new MyObj(mesh));
	obj->transform.position = {0, 0, -1};
	obj->transform.rotation = {0, 0, glm::radians(45.0f)};
	engine->load_object(std::dynamic_pointer_cast<MyObj>(obj));

	// create pointers to needed resources
	shared_ptr<Camera> cam = engine->window->renderer->camera;
	shared_ptr<Input> input = engine->input;
	float move_speed = 10.0f;
	float turn_speed = glm::radians(90.0);
	auto game_loop = [=](double delta)
	{
		glm::vec2 move = input->get_action_vector("left", "right", "forward", "back");

		if (glm::length(move) > 0.2f)
		{
			move = glm::normalize(move);
			float x = move.x * delta * move_speed;
			float z = move.y * delta * move_speed;
			cam->transform.position += cam->transform.direction_forward() * z;
			cam->transform.position += cam->transform.direction_right() * x;
		}
		float rotate = -input->get_action_axis("rotate_left", "rotate_right");
		if (abs(rotate) > 0.1f)
		{
			cam->transform.rotation.y += rotate * delta * turn_speed;
		}

		if (input->is_action_pressed("force_quit"))
		{
			engine->quit();
		}
	};
	engine->start(game_loop);
	return 0;
}