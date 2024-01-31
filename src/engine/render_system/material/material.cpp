#include "material.hpp"

Material::Material(std::shared_ptr<ShaderProgram> program, std::shared_ptr<Texture> p_albedo, std::shared_ptr<Texture> p_normal, std::shared_ptr<Texture> p_orm) : albedo(p_albedo), normal(p_normal), orm(p_orm)
{
	program->enable();
	program->set_int("material.albedo", 0);
	program->set_int("material.normal", 1);
	program->set_int("material.orm", 2);
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