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
	std::cout << "Deleting shader program: " << program << std::endl;
	glDeleteProgram(this->program);
}

const std::string SHADER_PREFIX = R"(
// built-in versioning

#version 460 core

// begin your code
// - - - - - - - -
)";

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
	unsigned int line_num;
	while (getline(reader, line))
	{
		code_buffer += this->preprocess_shader_code(line, file, ++line_num) + "\n";
	}
	reader.close();
	code_buffer = SHADER_PREFIX + code_buffer;

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

std::string ShaderProgram::preprocess_shader_code(std::string in_code, std::filesystem::path file, unsigned int line_number, unsigned int recursion_depth)
{ // preprocess on a line by line basis
	if (recursion_depth > SHADER_PREPROCESS_RECURSION_MAX)
	{
		std::cerr << "Shader include preprocessing step reached a recursion depth greater than the maximum (" << SHADER_PREPROCESS_RECURSION_MAX << "). Either you have an incredibly complex shader program layout, or you have circular dependencies! Why don't you go on and check on that buddy boy chumn ol' pal?" << std::endl;
		return "";
	}
	if (auto idx = in_code.find("#include"); idx != std::string::npos)
	{ // parse include files
		auto include_target = in_code.substr(idx + 9);
		include_target = include_target.substr(include_target.find_first_not_of('"'), include_target.find_last_of('"') - 1);
		auto file_target = file.relative_path().parent_path();
		file_target.append(include_target);
		if (!std::filesystem::exists(file_target))
		{
			std::cerr << "PREPROCESSING ERROR [" << file.relative_path().string() << ":" << line_number << "] Included file does not exist : " << std::endl
					  << "\tFile: " << file_target << std::endl;

			return "";
		}
		std::ifstream reader;
		reader.open(file_target);
		if (!reader.is_open())
		{
			return "";
		}
		std::string buffer;
		std::string line;
		unsigned int include_line_num = 0;
		while (getline(reader, line))
		{

			buffer += preprocess_shader_code(line, file_target, ++include_line_num, recursion_depth + 1) + "\n";
		}
		reader.close();
		return buffer;
	}

	return in_code;
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
