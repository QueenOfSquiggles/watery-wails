#include "engine/engine.hpp"
#include <iostream>
#include <memory>
#include <engine/render_system/resource_management/resource_factory.hpp>
using namespace std;
using namespace input;

constexpr float SPIN_SPEED = 0.3f;

class MyObj : public GameObject
{
public:
	MyObj(shared_ptr<Mesh> mesh) : GameObject(mesh) {}
	void render(RenderContext ctx) override
	{
		// transform.rotation.x += glm::radians(45.0f) * ctx.delta * SPIN_SPEED;
		transform.rotation.y += glm::radians(35.0f) * ctx.delta * SPIN_SPEED;
		// transform.rotation.z += glm::radians(15.0f) * ctx.delta * SPIN_SPEED;

		GameObject::render(ctx);
	}
};

int main()
{
	Engine *engine = new Engine();

	map<string, vector<InputMapping>> mappings = {
		// TODO: load this data from a file? Maybe CFG or TOML??
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
	engine->input->register_from_toml("res/actions.toml");
	auto mesh = ResourceFactory::load_mesh("res/testing/meshes/my_export/gltf/test_survival_backpack.gltf");
	auto obj = std::shared_ptr<MyObj>(new MyObj(mesh));
	obj->transform.position = {0, 0, 3};

	engine->load_object(std::dynamic_pointer_cast<MyObj>(obj), "default");

	auto program = engine->window->renderer->get_program_for("default");
	program->enable();
	program->set_vec3("environment.ambient_light", glm::vec3(0.5f));
	program->set_vec3("sun.direction", {.125, -1, 0});
	program->set_vec3("sun.colour", glm::vec3(1));
	program->set_vec3("lights_point[0].position", {0, 0, 0});
	program->set_vec3("lights_point[0].colour", glm::vec3(1.0));
	program->set_vec3("lights_point[0].attenuation_factors", {0, .2, 0});
	program->set_float("material.specular_strength", 32.0f);
	program->set_int("lights_active", 1);
	program->disable();

	// create pointers to needed resources
	shared_ptr<Camera> cam = engine->window->renderer->camera;
	shared_ptr<Input> input = engine->input;
	float move_speed = 10.0f;
	float turn_speed = glm::two_pi<float>() * 10.0f;

	auto game_loop = [=](double delta) mutable
	{
		obj->transform.scale = glm::vec3(1.0);
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
		glm::vec3 n_pos = {-1, 1, sin(engine->current_time * .25) * 5.0 + 3.0};
		program->enable();
		program->set_vec3("lights_point[0].position", n_pos);
		program->disable();
	};
	engine->start(game_loop);
	return 0;
}