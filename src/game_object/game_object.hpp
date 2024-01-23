#pragma once

#include <vector>
#include <glm/glm.hpp>

#include "mesh/mesh.hpp"
class GameObject
{
	Mesh mesh;

public:
	glm::vec3 position;
	glm::vec3 rotation;
	glm::vec3 scale;

	GameObject(Mesh p_mesh) : mesh(p_mesh) {}
};