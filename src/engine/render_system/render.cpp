#include "render.hpp"

Renderer::Renderer() : camera(new Camera())
{
	field_of_view = 90.0f;
	view_ratio = 6.0f / 8.0f;
	camera->create_perspective(field_of_view, 6.0f / 8.0f, camera_near, camera_far);
	batches = std::map<std::string, std::shared_ptr<BatchEntry>>();
}

void Renderer::register_batch(std::string batch_name, std::shared_ptr<ShaderProgram> program)
{
	if (!program)
	{
		return;
	}
	this->batches[batch_name] = std::unique_ptr<BatchEntry>(
		new BatchEntry{
			program,
			{},
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
	for (auto itr = this->batches.begin(); itr != this->batches.end(); itr++)
	{
		auto batch = itr->second;
		batch->program->enable();
		batch->program->set_float("environment.time", float(currtime));
		batch->program->set_mat4("view_transform", camera->get_view_matrix());
		batch->program->set_mat4("projection_transform", camera->get_projection_matrix());
		batch->program->set_vec3("environment.camera_position", camera->position);
		batch->program->set_vec3("environment.ambient_light", ambient_light.colour);
		batch->program->set_vec3("sun.direction", sun.direction);
		batch->program->set_vec3("sun.colour", sun.colour);
		batch->program->set_float("material.specular_strength", specular_strength);

		for (int i = 0; i < point_lights.size(); i++)
		{
			auto idx = std::to_string(i);
			auto light = point_lights[i];
			batch->program->set_vec3("lights_point[" + idx + "].position", light.position);
			batch->program->set_vec3("lights_point[" + idx + "].colour", light.colour);
			batch->program->set_vec3("lights_point[" + idx + "].attenuation_factors", light.falloff_components);
		}
		batch->program->set_int("lights_active", point_lights.size());

		RenderContext ctx{
			batch->program,
			itr->first,
			delta};
		for (auto o : batch->objects)
		{
			o->render(ctx);
		}
		batch->program->disable();
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
