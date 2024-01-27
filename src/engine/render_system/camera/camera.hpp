#pragma once

#include "engine/math/math.hpp"
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <iostream>
struct Camera
{
private:
	glm::mat4 projection;

public:
	Transform transform;

	Camera();
	void create_perspective(float view_angle_degrees, float frame_ratio, float near, float far);
	glm::mat4 get_view_matrix();
	glm::mat4 get_projection_matrix();
};