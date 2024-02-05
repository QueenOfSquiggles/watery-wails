#pragma once
#include "renderable.hpp"
#include "shaders/program.hpp"
#include "camera/camera.hpp"
#include "lights/lights.hpp"

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
	std::map<std::string, std::shared_ptr<BatchEntry>> batches;
	float field_of_view = 90.0f;
	float view_ratio = 6.0f / 8.0f;
	float camera_near = 0.1f;
	float camera_far = 1024.0f;

public:
	float specular_strength = 32.0f;
	AmbientLight ambient_light;
	DirectionalLight sun;
	std::vector<PointLight> point_lights;
	std::shared_ptr<Camera> camera;
	Renderer();
	std::shared_ptr<ShaderProgram> get_program_for(std::string batch_name);
	void register_batch(std::string, std::shared_ptr<ShaderProgram> program);
	void register_game_object(std::string batch_name, std::shared_ptr<Renderable> obj);

	void render(double delta, double currtime);

	void set_fov(float n_fov);
	void on_window_resized(int n_width, int n_height);
};