#pragma once
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <iostream>
struct Camera
{
private:
	glm::mat4 projection;

public:
	glm::vec3 position;

	Camera();
	void create_perspective(float view_angle_degrees, float frame_ratio, float near, float far);
	glm::mat4 get_view_matrix();
	glm::mat4 get_projection_matrix();
};