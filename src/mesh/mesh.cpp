#include "mesh.hpp"
#include <glad/glad.h>

MeshSurface::MeshSurface(glm::vec3 data[], int indices[], std::vector<VertexDataAttribute> attributes, Material p_material) : material(p_material)
{
	this->attributes = attributes.size();
	this->index_count = sizeof(indices) / sizeof(int);
	glGenBuffers(1, &VBO);
	glGenVertexArrays(1, &VAO);
	glGenBuffers(1, &EBO);

	glBindVertexArray(VAO);
	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(data), data, GL_STATIC_DRAW);

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

		index++;
		offset += attrib.get_data_size();
	}
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindVertexArray(0);
}
MeshSurface::~MeshSurface()
{
	glDeleteVertexArrays(1, &VAO);
	glDeleteBuffers(1, &VBO);
	glDeleteBuffers(1, &EBO);
}

void MeshSurface::render()
{
	glBindVertexArray(VAO);
	for (int i = 0; i < attributes; i++)
	{
		glEnableVertexAttribArray(i);
	}

	material.bind();
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
	glDrawElements(GL_TRIANGLES, index_count, GL_UNSIGNED_INT, 0);
	material.unbind();

	for (int i = 0; i < attributes; i++)
	{
		glDisableVertexAttribArray(i);
	}
	glBindVertexArray(0);
}
