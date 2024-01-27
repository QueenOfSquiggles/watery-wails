#include "math.hpp"

Transform::Transform() : position(0.0f), rotation(0.0f), scale(1.0f) {}

glm::mat4 Transform::as_mat4()
{
	auto trans = glm::mat4(1.0f);
	trans = glm::translate(trans, position);
	trans = glm::rotate(trans, rotation.x, glm::vec3(1, 0, 0));
	trans = glm::rotate(trans, rotation.y, glm::vec3(0, 1, 0));
	trans = glm::rotate(trans, rotation.z, glm::vec3(0, 0, 1));
	trans = glm::scale(trans, scale);
	return trans;
}

glm::mat4 Transform::as_mat4_inverted()
{
	auto trans = glm::mat4(1.0f);
	trans = glm::translate(trans, -position);
	trans = glm::rotate(trans, -rotation.x, glm::vec3(1, 0, 0));
	trans = glm::rotate(trans, -rotation.y, glm::vec3(0, 1, 0));
	trans = glm::rotate(trans, -rotation.z, glm::vec3(0, 0, 1));
	trans = glm::scale(trans, scale);
	return trans;
}
