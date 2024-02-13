#pragma once
#include <string>
#include <stb_image.h>
#include <glad/glad.h>
#include <iostream>

enum TextureChannels
{
	R = GL_R,
	RG = GL_RG,
	RGB = GL_RGB,
	RGBA = GL_RGBA,
};

class Texture
{
protected:
	unsigned int id;
	int width, height, channels;

	Texture() {}

public:
	Texture(std::string file);
	~Texture();
	inline unsigned int get() { return id; }
	inline int get_width() { return width; }
	inline int get_height() { return height; }
	inline int get_channels() { return channels; }

	virtual void bind();
	virtual void bind_slot(unsigned int slot);
	virtual void unbind();
	virtual void unbind_slot(unsigned int slot);
};