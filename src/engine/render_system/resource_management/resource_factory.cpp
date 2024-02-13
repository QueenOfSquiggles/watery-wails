#include "resource_factory.hpp"
std::unique_ptr<ResourceFactory> ResourceFactory::instance = nullptr;

ResourceFactory::ResourceFactory()
{
	instance = std::unique_ptr<ResourceFactory>(this);
}

void ResourceFactory::ensure_instance()
{
	if (instance)
		return;
	new ResourceFactory();
}

std::shared_ptr<Mesh> ResourceFactory::load_mesh(std::filesystem::path file)
{
	ensure_instance();
	if (auto search = instance->meshes.find(file); search != instance->meshes.end())
	{
		return search->second;
	}
	auto mesh = std::shared_ptr<Mesh>(new Mesh(file));
	instance->meshes[file] = mesh;
	return mesh;
}
std::shared_ptr<Material> ResourceFactory::load_material(std::filesystem::path file)
{
	ensure_instance();
	if (auto search = instance->materials.find(file); search != instance->materials.end())
	{
		return search->second;
	}
	auto material = std::shared_ptr<Material>(new Material(file));
	instance->materials[file] = material;
	return material;
}
std::shared_ptr<Texture> ResourceFactory::load_texture(std::filesystem::path file)
{
	ensure_instance();
	if (auto search = instance->textures.find(file); search != instance->textures.end())
	{
		return search->second;
	}
	auto texture = std::shared_ptr<Texture>(new Texture(file));
	instance->textures[file] = texture;
	return texture;
}
std::shared_ptr<ShaderProgram> ResourceFactory::load_shader(std::filesystem::path file, bool preprocess)
{
	ensure_instance();
	if (auto search = instance->shaders.find(file); search != instance->shaders.end())
	{
		return search->second;
	}

	auto shader = std::shared_ptr<ShaderProgram>(new ShaderProgram{
		file.stem().string() + ".vert",
		file.stem().string() + ".frag",
		preprocess,
	});
	instance->shaders[file] = shader;
	return shader;
}

std::shared_ptr<AudioData> ResourceFactory::load_audio(std::filesystem::path file, bool load_data_now)
{
	ensure_instance();
	if (auto search = instance->audio.find(file); search != instance->audio.end())
	{
		return search->second;
	}

	auto data = std::shared_ptr<AudioData>(new AudioData(file));
	instance->audio[file] = data;
	if (load_data_now)
	{
		data->load_audio_data();
	}
	return data;
}
