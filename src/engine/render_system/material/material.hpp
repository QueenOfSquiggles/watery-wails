#pragma once

#include "../texture/texture.hpp"
#include "../shaders/program.hpp"
#include <memory>

struct Material
{
	std::shared_ptr<Texture> albedo;
	std::shared_ptr<Texture> normal;
	std::shared_ptr<Texture> orm;

public:
	Material(std::shared_ptr<ShaderProgram> p_program, std::shared_ptr<Texture> p_albedo, std::shared_ptr<Texture> p_normal, std::shared_ptr<Texture> p_orm);
	void bind();
	void unbind();
};