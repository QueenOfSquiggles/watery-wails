#pragma once

#include "../renderable.hpp"
#include "../material/material.hpp"

#include <vector>
#include <glad/glad.h>
#include <iostream>
#include <memory>
#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>
#include <glm/glm.hpp>

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
struct Vertex
{
	glm::vec3 position;
	glm::vec3 normal;
	glm::vec2 uv;
	glm::vec3 tangent;
	glm::vec3 bitangent;
};

struct MeshSurfaceDataContainer
{
	std::vector<float> vertices;
	std::vector<unsigned int> indices;
	std::vector<VertexDataAttribute> attributes;
	std::shared_ptr<Material> material;
};

class MeshSurface
{
	std::shared_ptr<Material> material;
	unsigned int VAO, VBO, EBO, attributes, index_count;

public:
	MeshSurface(std::vector<float> data, std::vector<unsigned int> indices,
				std::vector<VertexDataAttribute> attributes, std::shared_ptr<Material> p_material);

	~MeshSurface();
	void render(RenderContext ctx);
};

class Mesh
{
	std::vector<std::shared_ptr<MeshSurface>> surfaces;
	void process_assimp_node(aiNode *node, const aiScene *scene, std::filesystem::path file);
	MeshSurfaceDataContainer load_surf_data_container(aiMesh *mesh, const aiScene *scene, std::filesystem::path file);
	std::shared_ptr<Material> load_material(aiMaterial *mat, std::filesystem::path file);

public:
	Mesh(std::vector<std::shared_ptr<MeshSurface>> p_surfaces);
	Mesh(std::filesystem::path file);
	void render(RenderContext ctx);
};