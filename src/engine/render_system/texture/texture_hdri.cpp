#include "texture_hdri.hpp"

TextureHDRI::TextureHDRI(std::string file) : Texture()
{
	float *data = stbi_loadf(file.c_str(), &width, &height, &channels, 0);
	if (data)
	{
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

		glGenTextures(1, &id);
		glBindTexture(GL_TEXTURE_2D, id);
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB16F, width, height, 0, GL_RGB, GL_FLOAT, data);

		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		stbi_image_free(data);
	}
	else
	{
		std::cerr << "Failed to load HDR image! " << file << std::endl;
	}
}
