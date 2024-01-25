#pragma once

#include "texture/texture.hpp"
#include "shaders/program.hpp"
#include <memory>

struct Material
{
	Texture *albedo;
	Texture *normal;
	Texture *orm;

public:
	Material(ShaderProgram *p_program, Texture *p_albedo, Texture *p_normal, Texture *p_orm);
	void bind();
	void unbind();
};