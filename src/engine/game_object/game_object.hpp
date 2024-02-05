#pragma once

#include "../render_system/renderable.hpp"
#include "../render_system/mesh/mesh.hpp"
#include "../math/math.hpp"

#include <vector>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <memory>

class GameObject : public Renderable
{
	std::shared_ptr<Mesh> mesh;

public:
	unsigned int id = -1;
	Transform transform;

	GameObject(std::shared_ptr<Mesh> p_mesh);

	virtual void render(RenderContext ctx) override;
};