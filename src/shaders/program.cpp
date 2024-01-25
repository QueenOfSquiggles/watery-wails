#include <fstream>
#include <iostream>
#include "program.hpp"

ShaderProgram::ShaderProgram(std::filesystem::path vert, std::filesystem::path frag)
{
	this->program = 0;
	ShaderComp vcomp = load_program(ShaderType::VERTEX, vert);
	ShaderComp fcomp = load_program(ShaderType::FRAGMENT, frag);
	if (!vcomp.succeeded)
	{
		std::cout << "Failed to load vertex shader:" << std::endl
				  << vcomp.log << std::endl;
		return;
	}
	if (!fcomp.succeeded)
	{
		std::cout << "Failed to load fragment shader:" << std::endl
				  << fcomp.log << std::endl;
		return;
	}
	this->vertex = vcomp.shader;
	this->fragment = fcomp.shader;
	this->program = glCreateProgram();
	glAttachShader(this->program, this->vertex);
	glAttachShader(this->program, this->fragment);
	glLinkProgram(this->program);
	int success;
	glGetProgramiv(this->program, GL_LINK_STATUS, &success);
	if (!success)
	{
		char log[SHADER_LOG_SIZE];
		glGetProgramInfoLog(this->program, SHADER_LOG_SIZE, NULL, log);
		std::cout << "Failed to link shader program: " << log << std::endl;
	}
	glDeleteShader(this->vertex);
	glDeleteShader(this->fragment);
}

ShaderProgram::~ShaderProgram()
{
	glDeleteProgram(this->program);
}

ShaderComp ShaderProgram::load_program(ShaderType type, std::filesystem::path file)
{
	ShaderComp comp;
	// load source data

	std::ifstream reader(file);
	if (!reader.is_open())
	{
		comp.succeeded = false;
		comp.log = "Failed to open file: " + file.string();
		return comp;
	}
	std::string code_buffer;
	std::string line;
	while (getline(reader, line))
	{
		code_buffer += line + "\n";
	}
	reader.close();

	// create GL shader
	unsigned int gl_type = GL_VERTEX_SHADER;
	switch (type)
	{
	case ShaderType::VERTEX:
		gl_type = GL_VERTEX_SHADER;
		break;
	case ShaderType::FRAGMENT:
		gl_type = GL_FRAGMENT_SHADER;
		break;
	}

	comp.shader = glCreateShader(gl_type);
	const char *shader_source_code = code_buffer.c_str();
	glShaderSource(comp.shader, 1, &shader_source_code, NULL);
	glCompileShader(comp.shader);
	int success;
	glGetShaderiv(comp.shader, GL_COMPILE_STATUS, &success);
	if (!success)
	{
		char info_log[SHADER_LOG_SIZE];
		glGetShaderInfoLog(comp.shader, SHADER_LOG_SIZE, NULL, info_log);
		comp.succeeded = false;
		comp.log = "Failed to compile shader " + file.string() + "\n\t" + info_log + "\n";
		return comp;
	}
	comp.succeeded = true;
	return comp;
}

void ShaderProgram::enable()
{
	if (this->program == 0)
	{
		return;
		// std::cerr << "ERROR!! USING UNINTIALIZED SHADER PROGRAM!" << std::endl;
	}
	glUseProgram(this->program);
}

void ShaderProgram::disable()
{
	glUseProgram(0);
}

void ShaderProgram::set_int(std::string name, int value)
{
	glUniform1i(glGetUniformLocation(this->program, name.c_str()), value);
}
void ShaderProgram::set_bool(std::string name, bool value)
{
	glUniform1i(glGetUniformLocation(this->program, name.c_str()), value);
}
void ShaderProgram::set_float(std::string name, float value)
{
	glUniform1f(glGetUniformLocation(this->program, name.c_str()), value);
}

void ShaderProgram::set_vec2(std::string name, glm::vec2 value)
{
	glUniform2f(glGetUniformLocation(this->program, name.c_str()), value.x, value.y);
}
void ShaderProgram::set_vec3(std::string name, glm::vec3 value)
{
	glUniform3f(glGetUniformLocation(this->program, name.c_str()), value.x, value.y, value.z);
}
void ShaderProgram::set_vec4(std::string name, glm::vec4 value)
{
	glUniform4f(glGetUniformLocation(this->program, name.c_str()), value.x, value.y, value.z, value.w);
}
void ShaderProgram::set_mat4(std::string name, glm::mat4 value)
{
	auto loc = glGetUniformLocation(this->program, name.c_str());
	glUniformMatrix4fv(loc, 1, GL_FALSE, glm::value_ptr(value));
}
