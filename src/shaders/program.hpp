#pragma once

#include <string>
#include <filesystem>
#include <glad/glad.h>
#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>

#ifndef SHADER_LOG_SIZE
#define SHADER_LOG_SIZE 512
#endif
enum ShaderType
{
	VERTEX,
	FRAGMENT
	// TODO: I don't wager I need anything more than these??? WTF would I use geom for in this?
};

struct ShaderComp
{
	unsigned int shader;
	bool succeeded;
	std::string log;
};

class ShaderProgram
{

private:
	unsigned int vertex;
	unsigned int fragment;
	unsigned int program;

	ShaderComp load_program(ShaderType type, std::filesystem::path file);

public:
	ShaderProgram(std::filesystem::path vert, std::filesystem::path frag);
	~ShaderProgram();

	void enable();
	void disable();

	// uniforms
	void set_int(std::string name, int value);
	void set_bool(std::string name, bool value);
	void set_float(std::string name, float value);

	void set_vec2(std::string name, glm::vec2 value);
	void set_vec3(std::string name, glm::vec3 value);
	void set_vec4(std::string name, glm::vec4 value);
	void set_mat4(std::string name, glm::mat4 value);
};