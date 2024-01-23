#pragma once

#include "texture/texture.hpp"
#include "shaders/program.hpp"
#include <memory>

struct Material
{
	Texture albedo;
	Texture normal;
	Texture orm;

	ShaderProgram program;

public:
	Material(ShaderProgram p_program, Texture p_albedo, Texture p_normal, Texture p_orm) : program(p_program), albedo(p_albedo), normal(p_normal), orm(p_orm)
	{
	}

	void bind();
	void unbind();
};