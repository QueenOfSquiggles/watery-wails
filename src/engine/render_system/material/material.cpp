#include "material.hpp"

Material::Material() {}

Material::Material(std::filesystem::path file)
{
	// TODO: is this constructor necessary???
}

Material::Material(std::shared_ptr<ShaderProgram> program, std::shared_ptr<Texture> p_albedo, std::shared_ptr<Texture> p_normal, std::shared_ptr<Texture> p_orm)
	: Material(p_albedo, p_normal, p_orm)
{
	program->enable();
	program->set_int("material.albedo", 0);
	program->set_int("material.normal", 1);
	program->set_int("material.orm", 2);
	program->disable();
}
Material::Material(std::shared_ptr<Texture> p_albedo, std::shared_ptr<Texture> p_normal, std::shared_ptr<Texture> p_orm)
	: albedo(p_albedo), normal(p_normal), orm(p_orm)
{
}

void Material::bind(RenderContext ctx)
{
	ctx.program->set_int("material.albedo", 0);
	ctx.program->set_int("material.normal", 1);
	ctx.program->set_int("material.orm", 2);
	if (albedo)
		albedo->bind_slot(0);
	if (normal)
		normal->bind_slot(1);
	if (orm)
		orm->bind_slot(2);
}

void Material::unbind()
{
	if (albedo)
		albedo->unbind_slot(0);
	if (normal)
		normal->unbind_slot(1);
	if (orm)
		orm->unbind_slot(2);
}