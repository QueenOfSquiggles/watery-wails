#include "game_object.hpp"

GameObject::GameObject(std::shared_ptr<Mesh> p_mesh) : mesh(p_mesh)
{
	this->position = {0, 0, 0};
	this->rotation = {0, 0, 0};
	this->scale = {1, 1, 1};
}

void GameObject::render(RenderContext ctx)
{
	glm::mat4 trans = glm::mat4(1.0f);
	trans = glm::rotate(trans, this->rotation.x, glm::vec3(1, 0, 0));
	trans = glm::rotate(trans, this->rotation.y, glm::vec3(0, 1, 0));
	trans = glm::rotate(trans, this->rotation.z, glm::vec3(0, 0, 1));
	trans = glm::scale(trans, this->scale);
	trans = glm::translate(trans, this->position);
	ctx.program->set_mat4("model_transform", trans);
	mesh->render(ctx);
}
