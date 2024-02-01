#pragma once

// TODO refactor these resource types into a submodule
#include "../mesh/mesh.hpp"
#include "../material/material.hpp"
#include "../texture/texture.hpp"
#include "../shaders/program.hpp"

#include <vector>
#include <string>
#include <filesystem>
#include <memory>
#include <map>

template <class T>
using _ResMap = std::map<std::filesystem::path, std::shared_ptr<T>>;

class ResourceFactory
{
	_ResMap<Mesh> meshes;
	_ResMap<Material> materials;
	_ResMap<Texture> textures;
	_ResMap<ShaderProgram> shaders;
	static std::unique_ptr<ResourceFactory> instance;
	static void ensure_instance();

public:
	ResourceFactory(const ResourceFactory &factory_copy) = delete;

	ResourceFactory();
	static std::shared_ptr<Mesh> load_mesh(std::filesystem::path file);
	static std::shared_ptr<Material> load_material(std::filesystem::path file);
	static std::shared_ptr<Texture> load_texture(std::filesystem::path file);
	static std::shared_ptr<ShaderProgram> load_shader(std::filesystem::path file);
};