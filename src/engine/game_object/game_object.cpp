#include "game_object.hpp"

GameObject::GameObject(std::shared_ptr<Mesh> p_mesh) : mesh(p_mesh)
{
}

void GameObject::render(RenderContext ctx)
{
	ctx.program->set_mat4("model_transform", transform.as_mat4());
	mesh->render(ctx);
}
