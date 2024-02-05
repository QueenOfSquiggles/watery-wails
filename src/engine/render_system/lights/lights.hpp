#pragma once
#include <glm/glm.hpp>

struct AmbientLight
{
	glm::vec3 colour;
};

struct DirectionalLight
{
	glm::vec3 colour;
	glm::vec3 direction;
};

struct PointLight
{
	glm::vec3 colour;
	glm::vec3 position;
	/// quadratic fallout function components where d=distance: x + dy + ddz
	glm::vec3 falloff_components;
};