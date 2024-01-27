#pragma once

#include "../renderable.hpp"
#include "../material/material.hpp"

#include <vector>
#include <glad/glad.h>
#include <iostream>

enum VertexDataAttributeType
{
	FLOAT = GL_FLOAT,
	INT = GL_INT,
};
struct VertexDataAttribute
{
	VertexDataAttributeType type;
	unsigned int size;
	unsigned int get_data_size();

	VertexDataAttribute(VertexDataAttributeType p_type, unsigned int p_size);
};

class MeshSurface
{
	std::shared_ptr<Material> material;
	unsigned int VAO, VBO, EBO, attributes, index_count;

public:
	MeshSurface(std::vector<float> data, std::vector<unsigned int> indices, 
	std::vector<VertexDataAttribute>attributes, std::shared_ptr<Material> p_material);
	~MeshSurface();
	void render(RenderContext ctx);
};

class Mesh
{
	std::vector<std::shared_ptr<MeshSurface>> surfaces;

public:
	Mesh(std::vector<std::shared_ptr<MeshSurface>> p_surfaces);
	void render(RenderContext ctx);
};