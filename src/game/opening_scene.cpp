#include "opening_scene.hpp"

constexpr float SPIN_SPEED = 0.3f;

using namespace std;
using namespace input;
namespace imgui = ImGui;

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
void load_opening_scene(Engine *engine)
{

	auto scene = SceneTree("res/testing/meshes/test_scene.gltf");

	// auto test_hdr = ResourceFactory::load_texture("res/texture/rostock_laage_airport_1k.hdr");

	// auto mesh = ResourceFactory::load_mesh("res/testing/meshes/poly_haven_hatchet/hatchet_1k.gltf");
	// auto obj = std::shared_ptr<MyObj>(new MyObj(mesh));
	// obj->transform.position = {0.0f, 0.0f, 2.0f};

	// engine->load_object(std::dynamic_pointer_cast<MyObj>(obj), "default");
	// auto renderer_weak = std::weak_ptr<Renderer>(engine->window->renderer);
	// if (!renderer_weak.expired())
	// {
	// 	auto renderer = renderer_weak.lock();
	// 	renderer->set_fov(70.0f);
	// 	renderer->ambient_light = {glm::vec3(0.1f)};
	// 	renderer->specular_strength = 64.0f;
	// 	renderer->sun = {
	// 		{1, 1, 1},		// colour
	// 		{0.125, -1, 0}, // direction
	// 	};
	// 	renderer->point_lights.push_back({
	// 		{1, 1, 1}, // colour
	// 		{0, 0, 0}, // position
	// 		{0, 1, 0}, // attenuation
	// 	});
	// 	renderer->point_lights.push_back({
	// 		{1, 1, 1}, // colour
	// 		{2, 0, 3}, // position
	// 		{0, 1, 0}, // attenuation
	// 	});
	// }

	// auto audio_file = ResourceFactory::load_audio("res/testing/sfx/test-audio.wav");
	// auto sfx = std::shared_ptr<AudioSource>(new AudioSource(audio_file));
	// engine->audio->queue_audio(sfx);

	// auto hdri = ResourceFactory::load_texture_hdri("res/texture/rostock_laage_airport_1k.hdr");
	// auto skybox = ResourceFactory::load_skybox("res/texture/rostock_laage_airport_1k.hdr");
	// auto skybox_render_callback = [](RenderContext ctx)
	// {
	// 	ctx.program->set_mat4("projection", ctx.cam->get_projection_matrix());
	// 	ctx.program->set_mat4("view", ctx.cam->get_view_matrix());
	// 	ctx.program->set_int("environment", 0);
	// };
	// engine->add_render_group("skybox", "res/shader/skybox/skybox", false, skybox_render_callback);
	// engine->add_renderable("skybox", skybox);

	// auto prim_cube = std::shared_ptr<MeshSurface>(new PrimitiveCube(1.0f));
	// prim_cube->material = std::shared_ptr<Material>(new Material(hdri, hdri, hdri));
	// auto test_cube = std::shared_ptr<Mesh>{
	// 	new Mesh({
	// 		prim_cube,
	// 	}),
	// };

	// // create weak pointers to needed resources
	// auto cam_weak = std::weak_ptr<Camera>(engine->window->renderer->camera);
	// // auto sfx_weak = std::weak_ptr<AudioSource>(sfx);
	// auto input_weak = std::weak_ptr<Input>(engine->input);
	// float move_speed = 10.0f;
	// float turn_speed = glm::two_pi<float>() * 10.0f;

	auto game_loop = [=](double delta) mutable
	{
		// auto cam = cam_weak.lock();
		// auto input = input_weak.lock();
		// glm::vec2 move = input->get_action_vector("left", "right", "back", "forward");
		// if (glm::length(move) > 0.2f)
		// {
		// 	move = glm::normalize(move);
		// 	float x = move.x * delta * move_speed;
		// 	float z = move.y * delta * move_speed;
		// 	// check values?
		// 	cam->position += cam->get_front() * z;
		// 	cam->position += cam->get_right() * x;
		// }
		// float rotate = input->get_action_axis("rotate_left", "rotate_right");
		// if (abs(rotate) > 0.1f)
		// {
		// 	cam->yaw += glm::sign(rotate) * delta * turn_speed;
		// }
		// if (input->is_action_just_pressed("force_quit"))
		// {
		// 	engine->quit();
		// }
		// if (input->is_action_just_pressed("test_something"))
		// {
		// 	if (sfx->get_is_playing())
		// 	{
		// 		sfx->pause();
		// 	}
		// 	else
		// 	{
		// 		sfx->play();
		// 	}
		// }
	};

	// bool show_demo_window = true;
	auto window_flags = ImGuiWindowFlags_AlwaysUseWindowPadding;
	auto debug_draw = [=]() mutable
	{
		imgui::Begin("Debug Info", nullptr, window_flags);
		imgui::Text("FPS: %d", (int)Engine::instance->FPS);
		imgui::Text("Delta time: %f s", Engine::instance->delta);

		if (imgui::CollapsingHeader("Rendering Batches") && imgui::BeginTable("render_batches_table", 2))
		{
			imgui::TableSetupColumn("Batch");
			imgui::TableSetupColumn("Entries");
			imgui::TableHeadersRow();
			auto data = engine->window->renderer->get_batch_data();
			for (auto entry : data)
			{
				imgui::TableNextColumn();
				imgui::Text("%s", entry.first.c_str());
				imgui::TableNextColumn();
				imgui::Text("%d", entry.second);
			}
			imgui::EndTable();
		}
		imgui::End();
	};
	engine->window->debug_draw = debug_draw;
	engine->game_tick_func = game_loop;
}