#include "mesh.hpp"
#include "../resource_management/resource_factory.hpp"

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
MeshSurface::~MeshSurface()
{
	// std::cout << "Deleting mesh surface:" << std::endl
	// 		  << "   VBO=" << VBO << std::endl
	// 		  << "   EBO=" << EBO << std::endl
	// 		  << "   VAO=" << VAO << std::endl;
	glDeleteBuffers(1, &VBO);
	glDeleteBuffers(1, &EBO);
	glDeleteVertexArrays(1, &VAO);
}

void MeshSurface::render(RenderContext ctx)
{
	material->bind(ctx);
	glBindVertexArray(VAO);
	glDrawElements(GL_TRIANGLES, index_count, GL_UNSIGNED_INT, 0);
	material->unbind();
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

Mesh::Mesh(std::filesystem::path file)
{
	auto abs_path = std::filesystem::absolute(file);
	// std::cout << "Loading model file: " << abs_path << std::endl;
	if (!std::filesystem::exists(file))
	{
		std::cout << "Does not exist!!! (relative?)" << file << std::endl;
		return;
	}
	Assimp::Importer importer;
	auto scene = importer.ReadFile(
		abs_path.c_str(), aiProcessPreset_TargetRealtime_Fast | aiProcess_CalcTangentSpace);
	// aiProcess_Triangulate);

	auto err_string = std::string(importer.GetErrorString());
	if (!err_string.empty())
	{
		std::cout << "Error! Failed to load model file" << std::endl
				  << "\tFile: " << abs_path << std::endl
				  << "\tError:" << err_string << std::endl;
		return;
	}
	process_assimp_node(scene->mRootNode, scene, file);
}

void Mesh::process_assimp_node(aiNode *node, const aiScene *scene, std::filesystem::path file)
{
	// std::cout << "Loading model node: " << node->mName.C_Str() << std::endl;
	for (unsigned int i = 0; i < node->mNumMeshes; i++)
	{
		aiMesh *mesh = scene->mMeshes[node->mMeshes[i]];
		auto data = load_surf_data_container(mesh, scene, file);
		auto surf = std::shared_ptr<MeshSurface>(new MeshSurface(data.vertices, data.indices, data.attributes, data.material));
		this->surfaces.push_back(surf);
	}
	for (unsigned int i = 0; i < node->mNumChildren; i++)
	{
		process_assimp_node(node->mChildren[i], scene, file);
	}
}

MeshSurfaceDataContainer Mesh::load_surf_data_container(aiMesh *mesh, const aiScene *scene, std::filesystem::path file)
{
	// std::cout << "Loading model mesh: " << mesh->mName.C_Str() << std::endl;
	MeshSurfaceDataContainer data;
	for (unsigned int i = 0; i < mesh->mNumVertices; i++)
	{
		Vertex vertex;
		vertex.position = {
			mesh->mVertices[i].x,
			mesh->mVertices[i].y,
			mesh->mVertices[i].z,
		};
		vertex.normal = {
			mesh->mNormals[i].x,
			mesh->mNormals[i].y,
			mesh->mNormals[i].z,
		};
		if (mesh->mTextureCoords[0])
		{
			vertex.uv = {
				mesh->mTextureCoords[0][i].x,
				mesh->mTextureCoords[0][i].y,
			};
		}
		else
		{
			vertex.uv = {0, 0};
		}
		if (mesh->HasTangentsAndBitangents())
		{
			// collect tan & bitan
			vertex.tangent = {
				mesh->mTangents[i].x,
				mesh->mTangents[i].y,
				mesh->mTangents[i].z,
			};
			vertex.bitangent = {
				mesh->mBitangents[i].x,
				mesh->mBitangents[i].y,
				mesh->mBitangents[i].z,
			};
		}
		else
		{
			// this really shouldn't happen but just in case....
			vertex.tangent = {1, 0, 0};
			vertex.bitangent = {0, 1, 0};
		}

		std::vector<float> vertex_floats = {
			vertex.position.x,
			vertex.position.y,
			vertex.position.z,
			vertex.normal.x,
			vertex.normal.y,
			vertex.normal.z,
			vertex.uv.x,
			vertex.uv.y,
			vertex.tangent.x,
			vertex.tangent.y,
			vertex.tangent.z,
			vertex.bitangent.x,
			vertex.bitangent.y,
			vertex.bitangent.z,
		};

		// append vertex_floats (Why The Fuck isn't there an append array type function??)
		data.vertices.insert(data.vertices.end(), vertex_floats.begin(), vertex_floats.end());
	}

	for (unsigned int i = 0; i < mesh->mNumFaces; i++)
	{
		auto face = mesh->mFaces[i];
		for (unsigned int j = 0; j < face.mNumIndices; j++)
		{
			data.indices.push_back(face.mIndices[j]);
		}
	}
	if (mesh->mMaterialIndex >= 0)
	{
		data.material = load_material(scene->mMaterials[mesh->mMaterialIndex], file);
	}
	data.attributes = {
		VertexDataAttribute(VertexDataAttributeType::FLOAT, 3), // position
		VertexDataAttribute(VertexDataAttributeType::FLOAT, 3), // normal
		VertexDataAttribute(VertexDataAttributeType::FLOAT, 2), // uv
		VertexDataAttribute(VertexDataAttributeType::FLOAT, 3), // tangent
		VertexDataAttribute(VertexDataAttributeType::FLOAT, 3), // bitangent
	};

	return data;
}
std::shared_ptr<Material> Mesh::load_material(aiMaterial *mat, std::filesystem::path file)
{
	// std::cout << "Loading material: " << mat->GetName().C_Str() << std::endl;
	std::vector<std::shared_ptr<Texture>> textures;
	std::vector<aiTextureType> types = {aiTextureType_DIFFUSE, aiTextureType_NORMALS, aiTextureType_SPECULAR};
	for (auto type : types)
	{
		auto n_of_type = mat->GetTextureCount(type);
		if (n_of_type <= 0)
		{
			// load default fallback texture (going to cause troubles but shouldn't crash)
			textures.push_back(ResourceFactory::load_texture("res/testing/textures/texture_01.png"));
			continue;
		}
		aiString str;
		mat->GetTexture(aiTextureType_DIFFUSE, 0, &str);
		auto path = file.parent_path();
		// std::cout << "Loading material texture: " << path << std::endl;
		path.append(str.C_Str());
		textures.push_back(ResourceFactory::load_texture(path));
	}

	return std::shared_ptr<Material>(new Material(textures[0], textures[1], textures[2]));
}
