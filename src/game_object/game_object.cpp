#include "game_object.hpp"
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

GameObject::GameObject(Mesh *p_mesh)
{
	this->mesh = p_mesh;
}

void GameObject::render(RenderContext ctx)
{
	glm::mat4 trans = glm::mat4(1.0f);
	trans = glm::translate(trans, this->position);
	trans = glm::rotate(trans, this->rotation.x, glm::vec3(1, 0, 0));
	trans = glm::rotate(trans, this->rotation.y, glm::vec3(0, 1, 0));
	trans = glm::rotate(trans, this->rotation.z, glm::vec3(0, 0, 1));
	trans = glm::scale(trans, this->scale);
	ctx.program->set_mat4("model_transform", trans);
	mesh->render(ctx);
}
