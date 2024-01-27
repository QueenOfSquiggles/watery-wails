#include "camera.hpp"

Camera::Camera()
	: projection(1.0f)
{
}

void Camera::create_perspective(float view_angle_degrees, float frame_ratio, float near, float far)
{
	this->projection = glm::perspective(view_angle_degrees, frame_ratio, near, far);
}

glm::mat4 Camera::get_view_matrix()
{
	return transform.as_mat4_inverted();
}
glm::mat4 Camera::get_projection_matrix()
{
	return projection;
}
