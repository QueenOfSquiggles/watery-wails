#include "engine/engine.hpp"
#include <iostream>
#include <memory>
using namespace std;
using namespace input;

constexpr float SPIN_SPEED = 0.8f;

class MyObj : public GameObject
{
public:
	MyObj(shared_ptr<Mesh> mesh) : GameObject(mesh) {}
	void render(RenderContext ctx) override
	{
		transform.rotation.x += glm::radians(45.0f) * ctx.delta * SPIN_SPEED;
		transform.rotation.y += glm::radians(35.0f) * ctx.delta * SPIN_SPEED;
		transform.rotation.z += glm::radians(15.0f) * ctx.delta * SPIN_SPEED;

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

	std::vector<float> vertices{// cube
								-0.5f, -0.5f, -0.5f, 0.0f, 0.0f, -1.0f, 0.0f, 0.0f,
								0.5f, -0.5f, -0.5f, 0.0f, 0.0f, -1.0f, 1.0f, 0.0f,
								0.5f, 0.5f, -0.5f, 0.0f, 0.0f, -1.0f, 1.0f, 1.0f,
								0.5f, 0.5f, -0.5f, 0.0f, 0.0f, -1.0f, 1.0f, 1.0f,
								-0.5f, 0.5f, -0.5f, 0.0f, 0.0f, -1.0f, 0.0f, 1.0f,
								-0.5f, -0.5f, -0.5f, 0.0f, 0.0f, -1.0f, 0.0f, 0.0f,
								-0.5f, -0.5f, 0.5f, 0.0f, 0.0f, 1.0f, 0.0f, 0.0f,
								0.5f, -0.5f, 0.5f, 0.0f, 0.0f, 1.0f, 1.0f, 0.0f,
								0.5f, 0.5f, 0.5f, 0.0f, 0.0f, 1.0f, 1.0f, 1.0f,
								0.5f, 0.5f, 0.5f, 0.0f, 0.0f, 1.0f, 1.0f, 1.0f,
								-0.5f, 0.5f, 0.5f, 0.0f, 0.0f, 1.0f, 0.0f, 1.0f,
								-0.5f, -0.5f, 0.5f, 0.0f, 0.0f, 1.0f, 0.0f, 0.0f,
								-0.5f, 0.5f, 0.5f, -1.0f, 0.0f, 0.0f, 1.0f, 0.0f,
								-0.5f, 0.5f, -0.5f, -1.0f, 0.0f, 0.0f, 1.0f, 1.0f,
								-0.5f, -0.5f, -0.5f, -1.0f, 0.0f, 0.0f, 0.0f, 1.0f,
								-0.5f, -0.5f, -0.5f, -1.0f, 0.0f, 0.0f, 0.0f, 1.0f,
								-0.5f, -0.5f, 0.5f, -1.0f, 0.0f, 0.0f, 0.0f, 0.0f,
								-0.5f, 0.5f, 0.5f, -1.0f, 0.0f, 0.0f, 1.0f, 0.0f,
								0.5f, 0.5f, 0.5f, 1.0f, 0.0f, 0.0f, 1.0f, 0.0f,
								0.5f, 0.5f, -0.5f, 1.0f, 0.0f, 0.0f, 1.0f, 1.0f,
								0.5f, -0.5f, -0.5f, 1.0f, 0.0f, 0.0f, 0.0f, 1.0f,
								0.5f, -0.5f, -0.5f, 1.0f, 0.0f, 0.0f, 0.0f, 1.0f,
								0.5f, -0.5f, 0.5f, 1.0f, 0.0f, 0.0f, 0.0f, 0.0f,
								0.5f, 0.5f, 0.5f, 1.0f, 0.0f, 0.0f, 1.0f, 0.0f,
								-0.5f, -0.5f, -0.5f, 0.0f, -1.0f, 0.0f, 0.0f, 1.0f,
								0.5f, -0.5f, -0.5f, 0.0f, -1.0f, 0.0f, 1.0f, 1.0f,
								0.5f, -0.5f, 0.5f, 0.0f, -1.0f, 0.0f, 1.0f, 0.0f,
								0.5f, -0.5f, 0.5f, 0.0f, -1.0f, 0.0f, 1.0f, 0.0f,
								-0.5f, -0.5f, 0.5f, 0.0f, -1.0f, 0.0f, 0.0f, 0.0f,
								-0.5f, -0.5f, -0.5f, 0.0f, -1.0f, 0.0f, 0.0f, 1.0f,
								-0.5f, 0.5f, -0.5f, 0.0f, 1.0f, 0.0f, 0.0f, 1.0f,
								0.5f, 0.5f, -0.5f, 0.0f, 1.0f, 0.0f, 1.0f, 1.0f,
								0.5f, 0.5f, 0.5f, 0.0f, 1.0f, 0.0f, 1.0f, 0.0f,
								0.5f, 0.5f, 0.5f, 0.0f, 1.0f, 0.0f, 1.0f, 0.0f,
								-0.5f, 0.5f, 0.5f, 0.0f, 1.0f, 0.0f, 0.0f, 0.0f,
								-0.5f, 0.5f, -0.5f, 0.0f, 1.0f, 0.0f, 0.0f, 1.0f};

	std::vector<unsigned int> indices{0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16, 17, 18, 19, 20, 21, 22, 23, 24, 25, 26, 27, 28, 29, 30, 31, 32, 33, 34, 35};

	auto ptr_wall_albedo = new Texture("res/texture/stone_tiles_diff_1k.jpg");
	auto ptr_wall_orm = new Texture("res/texture/stone_tiles_arm_1k.jpg");
	auto ptr_wall_normal = new Texture("res/texture/stone_tiles_nor_gl_1k.jpg");

	auto ptr_floor = new Texture("./res/testing/textures/texture_01.png");
	auto wall_albedo = shared_ptr<Texture>(ptr_wall_albedo);
	auto wall_normal = shared_ptr<Texture>(ptr_wall_normal);
	auto wall_orm = shared_ptr<Texture>(ptr_wall_orm);
	auto floor_tex = shared_ptr<Texture>(ptr_floor);

	auto program = engine->window->renderer->get_program_for("default");

	if (program == nullptr)
	{
		cerr << "Failed to load default shader program!" << endl;
	}
	auto mat = shared_ptr<Material>(new Material(program, wall_albedo, wall_normal, wall_orm));
	auto floor_mat = shared_ptr<Material>(new Material(program, floor_tex, floor_tex, floor_tex));

	std::vector<VertexDataAttribute>
		attribs{
			{VertexDataAttributeType::FLOAT, 3}, // position
			{VertexDataAttributeType::FLOAT, 3}, // normal
			{VertexDataAttributeType::FLOAT, 2}, // uvs
		};

	auto surf = shared_ptr<MeshSurface>(new MeshSurface(vertices, indices, attribs, mat));
	auto floor_surf = shared_ptr<MeshSurface>(new MeshSurface(vertices, indices, attribs, floor_mat));

	auto mesh = shared_ptr<Mesh>(new Mesh(
		std::vector<shared_ptr<MeshSurface>>{
			surf,
		}));
	auto floor_mesh = shared_ptr<Mesh>(new Mesh(
		std::vector<shared_ptr<MeshSurface>>{
			floor_surf,
		}));
	// + + + + + + + + + + + + + + + +
	// /CONVERT TO MODEL LOADING CODE
	//

	auto obj = std::shared_ptr<MyObj>(new MyObj(mesh));
	obj->transform.position = {0, -1, 5};
	obj->transform.scale = glm::vec3(3.0f);

	auto floor = std::shared_ptr<GameObject>(new GameObject(floor_mesh));
	floor->transform.scale = glm::vec3(512.0f);

	engine->load_object(std::dynamic_pointer_cast<MyObj>(obj), "default");
	engine->add_render_group("unlit");
	engine->load_object(floor, "unlit");

	// create pointers to needed resources
	shared_ptr<Camera> cam = engine->window->renderer->camera;
	shared_ptr<Input> input = engine->input;
	float move_speed = 10.0f;
	float turn_speed = glm::two_pi<float>() * 10.0f;

	program->enable();
	program->set_vec3("environment.ambient_light", glm::vec3(0.1f));
	program->set_vec3("light.colour", {1, 1, 1});
	program->set_vec3("light.direction", {.5, -1, -.5});
	program->disable();

	auto game_loop = [=](double delta) mutable
	{
		glm::vec2 move = input->get_action_vector("left", "right", "back", "forward");

		if (glm::length(move) > 0.2f)
		{
			move = glm::normalize(move);
			float x = move.x * delta * move_speed;
			float z = move.y * delta * move_speed;
			// check values?
			cam->position += cam->get_front() * z;
			cam->position += cam->get_right() * x;
		}
		float rotate = input->get_action_axis("rotate_left", "rotate_right");
		if (abs(rotate) > 0.1f)
		{
			cam->yaw += glm::sign(rotate) * delta * turn_speed;
		}

		if (input->is_action_pressed("force_quit"))
		{

			engine->quit();
		}
	};
	engine->start(game_loop);
	return 0;
}