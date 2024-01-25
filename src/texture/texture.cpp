#include "texture.hpp"
#include <stb_image.h>
#include <iostream>

Texture::Texture(std::string file)
{

	unsigned char *data;
	data = stbi_load(file.c_str(), &this->width, &this->height, &this->channels, 0);
	if (!data)
	{
		std::cerr << "Failed to load image file from: " << file << std::endl;
		return;
	}
	TextureChannels tex_channels = TextureChannels::RGB;
	switch (this->channels)
	{
	case 0:
		std::cerr << "Texture loaded with 0 channels from: " << file << std::endl;
		return;
	case 1:
		tex_channels = TextureChannels::R;
		break;
	case 2:
		tex_channels = TextureChannels::RG;
		break;
	case 3:
		tex_channels = TextureChannels::RGB;
		break;
	case 4:
		tex_channels = TextureChannels::RGBA;
		break;
	}

	glGenTextures(1, &this->id);
	glBindTexture(GL_TEXTURE_2D, this->id);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, this->width, this->height, 0, tex_channels, GL_UNSIGNED_BYTE, data);
	glGenerateMipmap(GL_TEXTURE_2D);
	stbi_image_free(data);
}
Texture::~Texture()
{
}

void Texture::bind()
{
	glBindTexture(GL_TEXTURE_2D, this->id);
}

void Texture::bind_slot(unsigned int slot)
{
	if (slot > 31)
	{
		std::cerr << "Texture slot index is above slot cap! Max value is " << (GL_TEXTURE31 - GL_TEXTURE0) << "!" << std::endl;
		return;
	}
	glActiveTexture(GL_TEXTURE0 + slot);
	glBindTexture(GL_TEXTURE_2D, this->id);
}

void Texture::unbind()
{
	glBindTexture(GL_TEXTURE_2D, 0);
}

void Texture::unbind_slot(unsigned int slot)
{
	glActiveTexture(GL_TEXTURE0 + slot);
	glBindTexture(GL_TEXTURE_2D, 0);
}
