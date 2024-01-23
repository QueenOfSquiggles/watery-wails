#pragma once
#include <string>
#include <glad/glad.h>

enum TextureChannels
{
	RGB = GL_RGB,
	RGBA = GL_RGBA,
};

class Texture
{
	unsigned int id;
	int width, height, channels;

public:
	Texture(std::string file, TextureChannels channels);
	~Texture();

	inline int get_width() { return width; }
	inline int get_height() { return height; }
	inline int get_channels() { return channels; }

	void bind();
	void bind_slot(unsigned int slot);
	void unbind();
	void unbind_slot(unsigned int slot);
};