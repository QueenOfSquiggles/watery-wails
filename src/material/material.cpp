#include "material.hpp"

void Material::bind()
{
	program.enable();
	albedo.bind_slot(0);
	normal.bind_slot(1);
	orm.bind_slot(2);
	program.set_int("albedo", 0);
	program.set_int("normal", 1);
	program.set_int("orm", 2);
}

void Material::unbind()
{
	albedo.unbind_slot(0);
	normal.unbind_slot(1);
	orm.unbind_slot(2);
	program.disable();
}