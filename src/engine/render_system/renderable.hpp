#pragma once
#include "shaders/program.hpp"
#include "lights/lights.hpp"
#include "camera/camera.hpp"
#include <string>
#include <memory>
#include <vector>

struct RenderContext
{
	std::shared_ptr<ShaderProgram> program;
	std::string batch_name;
	double delta;
	double currtime;

	// lighting
	DirectionalLight sun;
	std::vector<PointLight> point_lights;
	AmbientLight ambient_light;
	float specular_strength = 32.0f;

	// camera
	std::shared_ptr<Camera> cam;
};

class Renderable
{
public:
	virtual void render(RenderContext ctx) = 0;
};
