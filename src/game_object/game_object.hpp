#pragma once

#include "render/render.hpp"
#include "mesh/mesh.hpp"
#include <vector>
#include <glm/glm.hpp>

class GameObject : public Renderable
{
	Mesh *mesh;

public:
	glm::vec3 position;
	glm::vec3 rotation;
	glm::vec3 scale;

	GameObject(Mesh *p_mesh);

	void render(RenderContext ctx) override;
};