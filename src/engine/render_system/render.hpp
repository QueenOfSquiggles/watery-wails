#pragma once
#include "renderable.hpp"
#include "shaders/program.hpp"
#include "camera/camera.hpp"
#include <map>
#include <string>
#include <vector>
#include <iostream>
#include <memory>
struct BatchEntry
{
	std::shared_ptr<ShaderProgram> program;
	std::vector<std::shared_ptr<Renderable>> objects;
};

class Renderer
{
	std::map<std::string, BatchEntry> batches;
	float field_of_view = 90.0f;
	float view_ratio = 6.0f / 8.0f;

public:
	std::shared_ptr<Camera> camera;
	Renderer();
	std::shared_ptr<ShaderProgram> get_program_for(std::string batch_name);
	void register_batch(std::string, std::shared_ptr<ShaderProgram> program);
	void register_game_object(std::string batch_name, std::shared_ptr<Renderable> obj);

	void render(double delta, double currtime);

	void set_fov(float n_fov);
	void on_window_resized(int n_width, int n_height);
};