#include "render/render.hpp"
#include <iostream>

void Renderer::register_batch(std::string batch_name, ShaderProgram *program)
{
	if (!program)
	{
		return;
	}
	this->batches[batch_name] = {
		program,
		std::vector<Renderable *>()};
}
void Renderer::register_game_object(std::string batch_name, Renderable *obj)
{
	if (auto batch = this->batches.find(batch_name); batch != this->batches.end())
	{
		batch->second.objects.push_back(obj);
	}
}

void Renderer::render()
{
	for (auto itr = this->batches.begin(); itr != this->batches.end(); itr++)
	{
		auto batch = itr.operator->();
		batch->second.program->enable();
		RenderContext ctx{
			batch->second.program,
			batch->first};
		for (auto o : batch->second.objects)
		{
			o->render(ctx);
		}
		batch->second.program->disable();
	}
}
