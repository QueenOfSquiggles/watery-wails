#pragma once

#include "../texture/texture.hpp"
#include "../shaders/program.hpp"
#include "../renderable.hpp"

#include <memory>
#include <filesystem>

struct Material
{
	std::shared_ptr<Texture> albedo;
	std::shared_ptr<Texture> normal;
	std::shared_ptr<Texture> orm;

public:
	Material();
	Material(std::shared_ptr<ShaderProgram> p_program, std::shared_ptr<Texture> p_albedo, std::shared_ptr<Texture> p_normal, std::shared_ptr<Texture> p_orm);
	Material(std::shared_ptr<Texture> p_albedo, std::shared_ptr<Texture> p_normal, std::shared_ptr<Texture> p_orm);
	Material(std::filesystem::path file);
	void bind(RenderContext ctx);
	void unbind();
};