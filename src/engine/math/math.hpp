#pragma once
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

struct Transform
{
	glm::vec3 position;
	glm::vec3 rotation;
	glm::vec3 scale;
	Transform();
	glm::mat4 as_mat4();
	glm::mat4 as_mat4_inverted();
};