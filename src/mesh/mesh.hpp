#pragma once

#include <vector>
#include <glm/glm.hpp>
#include <glad/glad.h>
#include "material/material.hpp"

enum VertexDataAttributeType
{
	FLOAT = GL_FLOAT,
	INT = GL_INT,
};
class VertexDataAttribute
{
	friend MeshSurface;

	VertexDataAttributeType type;
	unsigned int size;
	unsigned int get_data_size();

public:
	VertexDataAttribute(VertexDataAttributeType p_type, unsigned int p_size);
};

class MeshSurface
{
	Material material;
	unsigned int VAO, VBO, EBO, attributes, index_count;

public:
	MeshSurface(glm::vec3 data[], int indices[], std::vector<VertexDataAttribute> attributes, Material p_material);
	~MeshSurface();
	void render();
};

class Mesh
{
	std::vector<MeshSurface> surfaces;

public:
	Mesh(std::vector<MeshSurface> p_surfaces) : surfaces(p_surfaces) {}
};