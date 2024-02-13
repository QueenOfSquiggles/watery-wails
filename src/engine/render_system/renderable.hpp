#pragma once
#include "shaders/program.hpp"
#include <string>
#include <memory>

struct RenderContext
{
	std::shared_ptr<ShaderProgram> program;
	std::string batch_name;
	double delta;
};

class Renderable
{
public:
	virtual void render(RenderContext ctx) = 0;
};
