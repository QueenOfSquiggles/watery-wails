#include "camera.hpp"

Camera::Camera()
	: projection(1.0f), front(0.0f, 0.0f, -1.0f), up(0.0f, 1.0f, 0.0f), right(1.0f, 0.0f, 0.0f), position(1.0f), yaw(90.0f), pitch(0)
{
}

void Camera::create_perspective(float view_angle_degrees, float frame_ratio, float near, float far)
{
	this->projection = glm::perspective(glm::radians(view_angle_degrees), frame_ratio, near, far);
}

void Camera::update_transform()
{
	auto direction = glm::vec3({
		cos(glm::radians(yaw)) * cos(glm::radians(pitch)),
		sin(glm::radians(pitch)),
		sin(glm::radians(yaw)) * cos(glm::radians(pitch)),
	});
	front = safe_normalize(direction);
	right = safe_normalize(glm::cross(front, world_up));
	up = safe_normalize(glm::cross(right, front));
	view = glm::lookAt(position, position + front, up);
}

glm::mat4 Camera::get_view_matrix() { return view; }
glm::mat4 Camera::get_projection_matrix() { return projection; }
glm::vec3 Camera::get_front() { return front; }
glm::vec3 Camera::get_up() { return up; }
glm::vec3 Camera::get_right() { return right; }
