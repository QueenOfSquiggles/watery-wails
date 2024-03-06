#include "render.hpp"

Renderer::Renderer() : camera(new Camera())
{
	field_of_view = 90.0f;
	view_ratio = 6.0f / 8.0f;
	camera->create_perspective(field_of_view, 6.0f / 8.0f, camera_near, camera_far);
	batches = std::map<std::string, std::shared_ptr<BatchEntry>>();
}

void Renderer::register_batch(std::string batch_name, std::shared_ptr<ShaderProgram> program, std::function<void(RenderContext)> setup_callback)
{
	if (!program)
	{
		return;
	}
	this->batches[batch_name] = std::unique_ptr<BatchEntry>(
		new BatchEntry{
			program,
			{},
			setup_callback,
		});
}
void Renderer::register_game_object(std::string batch_name, std::shared_ptr<Renderable> obj)
{
	if (auto batch = this->batches.find(batch_name); batch != this->batches.end())
	{
		batch->second->objects.push_back(obj);
	}
}

void Renderer::render(double delta, double currtime)
{
	this->camera->update_transform();
	RenderContext ctx{
		nullptr,
		"null",
		delta,
		currtime,
		// lights
		sun,
		point_lights,
		ambient_light,
		specular_strength,
		// camera
		camera,
	};
	for (auto itr = this->batches.begin(); itr != this->batches.end(); itr++)
	{
		ctx.program = itr->second->program;
		ctx.batch_name = itr->first;

		ctx.program->enable();
		itr->second->setup_callback(ctx);
		for (auto o : itr->second->objects)
		{
			o->render(ctx);
		}
		ctx.program->disable();
	}
}

void Renderer::on_window_resized(int n_width, int n_height)
{
	this->view_ratio = float(n_width) / float(n_height);
	camera->create_perspective(this->field_of_view, this->view_ratio, camera_near, camera_far);
}

void Renderer::set_fov(float n_fov)
{
	this->field_of_view = n_fov;
	camera->create_perspective(this->field_of_view, this->view_ratio, camera_near, camera_far);
}

std::shared_ptr<ShaderProgram> Renderer::get_program_for(std::string batch_name)
{
	if (auto batch = this->batches.find(batch_name); batch != this->batches.end())
	{
		return batch->second->program;
	}
	return nullptr;
}

std::map<std::string, unsigned int> Renderer::get_batch_data()
{
	std::map<std::string, unsigned int> data;
	for (auto entry : batches)
	{
		data[entry.first] = entry.second->objects.size();
	}
	return data;
}

void default_render_batch_setup_callback(RenderContext ctx)
{

	ctx.program->set_float("environment.time", float(ctx.currtime));
	ctx.program->set_mat4("view_transform", ctx.cam->get_view_matrix());
	ctx.program->set_mat4("projection_transform", ctx.cam->get_projection_matrix());
	ctx.program->set_vec3("environment.camera_position", ctx.cam->position);
	ctx.program->set_vec3("environment.ambient_light", ctx.ambient_light.colour);
	ctx.program->set_vec3("sun.direction", ctx.sun.direction);
	ctx.program->set_vec3("sun.colour", ctx.sun.colour);
	ctx.program->set_float("material.specular_strength", ctx.specular_strength);

	// I really wish C++'s `auto` keyword worked like in rust
	//	where it would use the context to determine the expected type
	for (std::vector<PointLight>::size_type i = 0; i < ctx.point_lights.size(); i++)
	{
		auto idx = std::to_string(i);
		auto light = ctx.point_lights[i];
		ctx.program->set_vec3("lights_point[" + idx + "].position", light.position);
		ctx.program->set_vec3("lights_point[" + idx + "].colour", light.colour);
		ctx.program->set_vec3("lights_point[" + idx + "].attenuation_factors", light.falloff_components);
	}
	ctx.program->set_int("lights_active", ctx.point_lights.size());
}