#pragma once

// TODO refactor these resource types into a submodule
#include "engine/render_system/mesh/mesh.hpp"
#include "engine/render_system/material/material.hpp"
#include "engine/render_system/texture/texture.hpp"
#include "engine/render_system/shaders/program.hpp"
#include "engine/render_system/sky/skybox.hpp"
#include "engine/render_system/texture/texture_hdri.hpp"
#include "engine/audio_system/audio.hpp"

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
	_ResMap<TextureHDRI> textures_hdri;
	_ResMap<SkyBox> skyboxes;
	_ResMap<ShaderProgram> shaders;
	_ResMap<AudioData> audio;
	static std::unique_ptr<ResourceFactory> instance;
	static void ensure_instance();

public:
	ResourceFactory(const ResourceFactory &factory_copy) = delete;

	ResourceFactory();
	static std::shared_ptr<Mesh> load_mesh(std::filesystem::path file);
	static std::shared_ptr<Material> load_material(std::filesystem::path file);
	static std::shared_ptr<Texture> load_texture(std::filesystem::path file);
	static std::shared_ptr<TextureHDRI> load_texture_hdri(std::filesystem::path file);
	static std::shared_ptr<SkyBox> load_skybox(std::filesystem::path file);
	static std::shared_ptr<ShaderProgram> load_shader(std::filesystem::path file, bool preprocess = true);
	static std::shared_ptr<AudioData> load_audio(std::filesystem::path file, bool load_data_now = true);
};