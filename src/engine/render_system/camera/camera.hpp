#pragma once

#include "engine/math/math.hpp"
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <iostream>

#ifndef CONST_WORLD_UP
#define CONST_WORLD_UP
constexpr glm::vec3 world_up = glm::vec3(0, 1, 0);
#endif

struct Camera
{
private:
	glm::mat4 projection;
	glm::mat4 view;

	glm::vec3 front;
	glm::vec3 up;
	glm::vec3 right;

public:
	glm::vec3 position;
	float yaw, pitch;

	Camera();
	void create_perspective(float view_angle_degrees, float frame_ratio, float near, float far);
	void update_transform();
	glm::mat4 get_view_matrix();
	glm::mat4 get_projection_matrix();
	glm::vec3 get_front();
	glm::vec3 get_up();
	glm::vec3 get_right();
};