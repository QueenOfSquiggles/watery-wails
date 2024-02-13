#pragma once

#include <memory>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include "texture.hpp"
#include "texture_hdri.hpp"
#include "engine/render_system/shaders/program.hpp"
#include "engine/render_system/mesh/mesh.hpp"

constexpr unsigned int CUBE_MAP_FACE_SIZE = 512;

class CubeMap : public Texture
{
	unsigned int cubemap; // hard-coded 6 bc cube has 6 faces

public:
	CubeMap(std::weak_ptr<TextureHDRI> hdri_equirectangular);

	void bind() override;
	void bind_slot(unsigned int slot) override;
	void unbind() override;
	void unbind_slot(unsigned int slot) override;
};