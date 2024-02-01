#include "mesh.hpp"
VertexDataAttribute::VertexDataAttribute(VertexDataAttributeType p_type, unsigned int p_size) : type(p_type), size(p_size) {}
unsigned int VertexDataAttribute::get_data_size()
{
	unsigned int type_size = 0;
	switch (this->type)
	{
	case VertexDataAttributeType::FLOAT:
		type_size = sizeof(float);
		break;
	case VertexDataAttributeType::INT:
		type_size = sizeof(int);
		break;
	}
	return this->size * type_size;
}

MeshSurface::MeshSurface(std::vector<float> vertex_data, std::vector<unsigned int> indices, std::vector<VertexDataAttribute> attributes, std::shared_ptr<Material> p_material) : material(p_material)
{
	this->attributes = attributes.size();
	this->index_count = indices.size();
	glGenVertexArrays(1, &VAO);
	glGenBuffers(1, &VBO);
	glGenBuffers(1, &EBO);

	glBindVertexArray(VAO);

	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glBufferData(GL_ARRAY_BUFFER, vertex_data.size() * sizeof(float), vertex_data.data(), GL_STATIC_DRAW);

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(unsigned int), indices.data(), GL_STATIC_DRAW);

	unsigned int stride = 0;
	for (auto attrib : attributes)
	{
		stride += attrib.get_data_size();
	}
	unsigned int index = 0;
	unsigned int offset = 0;
	for (auto attrib : attributes)
	{

		glVertexAttribPointer(index, attrib.size, attrib.type, GL_FALSE, stride, (void *)offset);
		glEnableVertexAttribArray(index);
		index++;
		offset += attrib.get_data_size();
	}
}
Mesh::Mesh(std::filesystem::path file)
{
	Assimp::Importer importer;
	auto scene = importer.ReadFile(file.c_str(),
								   aiProcess_Triangulate |
									   aiProcess_FlipUVs |
									   aiProcess_OptimizeMeshes |
									   aiProcess_GenNormals);
	if (!scene || scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE || !scene->mRootNode)
	{
		std::cout << "Error! Failed to load model file from: " << file << std::endl
				  << "\tError:" << importer.GetErrorString() << std::endl;
		return;
	}
	//
}

MeshSurface::~MeshSurface()
{
	std::cout << "Deleting mesh surface:" << std::endl
			  << "   VBO=" << VBO << std::endl
			  << "   EBO=" << EBO << std::endl
			  << "   VAO=" << VAO << std::endl;
	glDeleteBuffers(1, &VBO);
	glDeleteBuffers(1, &EBO);
	glDeleteVertexArrays(1, &VAO);
}

void MeshSurface::render(RenderContext ctx)
{
	// for (unsigned int i = 0; i < attributes; i++)
	// {
	// 	glEnableVertexAttribArray(i);
	// }

	material->bind(ctx);
	glBindVertexArray(VAO);
	glDrawElements(GL_TRIANGLES, index_count, GL_UNSIGNED_INT, 0);
	material->unbind();

	// for (unsigned int i = 0; i < attributes; i++)
	// {
	// 	glDisableVertexAttribArray(i);
	// }
	glBindVertexArray(0);
}
Mesh::Mesh(std::vector<std::shared_ptr<MeshSurface>> p_surfaces) : surfaces(p_surfaces)
{
}

void Mesh::render(RenderContext ctx)
{
	for (auto surf : surfaces)
	{
		surf->render(ctx);
	}
}