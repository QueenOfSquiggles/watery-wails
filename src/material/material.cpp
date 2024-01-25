#include "material.hpp"

Material::Material(ShaderProgram *program, Texture *p_albedo, Texture *p_normal, Texture *p_orm) : albedo(p_albedo), normal(p_normal), orm(p_orm)
{
	program->enable();
	program->set_int("albedo", 0);
	program->set_int("normal", 1);
	program->set_int("orm", 2);
	program->disable();
}

void Material::bind()
{
	albedo->bind_slot(0);
	normal->bind_slot(1);
	orm->bind_slot(2);
}

void Material::unbind()
{
	albedo->unbind_slot(0);
	normal->unbind_slot(1);
	orm->unbind_slot(2);
}