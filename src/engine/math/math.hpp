#pragma once
#define GLM_ENABLE_EXPERIMENTAL // fixes error on using string cast
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtx/string_cast.hpp>
#include <string>

glm::vec3 safe_normalize(glm::vec3 in_vec);
std::string stringify(glm::vec3 vec);

/// Coordination of position, angle, and scale for 3D objects.
struct Transform
{
	glm::vec3 position;
	glm::vec3 rotation;
	glm::vec3 scale;
	Transform();
	glm::mat4 as_mat4();

	glm::vec3 direction_forward();
	glm::vec3 direction_right();
	glm::vec3 direction_up();
};