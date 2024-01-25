#pragma once
#include "shaders/program.hpp"
#include <map>
#include <string>
#include <vector>

struct RenderContext
{
	ShaderProgram *program;
	std::string batch_name;
};

class Renderable
{
public:
	virtual void render(RenderContext ctx) = 0;
};

struct BatchEntry
{
	ShaderProgram *program;
	std::vector<Renderable *> objects;
};

class Renderer
{
	std::map<std::string, BatchEntry> batches;

public:
	void register_batch(std::string, ShaderProgram *program);
	void register_game_object(std::string batch_name, Renderable *obj);

	void render();
};