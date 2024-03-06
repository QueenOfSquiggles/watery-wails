#pragma once
#include <filesystem>
#include "engine/render_system/texture/texture.hpp"
#include "engine/render_system/texture/cubemap.hpp"
#include "engine/render_system/renderable.hpp"
#include "engine/render_system/mesh/primitives.hpp"

class SkyBox : public Renderable
{
	std::shared_ptr<CubeMap> cube_map;
	std::shared_ptr<PrimitiveCube> cube_mesh;

public:
	SkyBox(std::filesystem::path hdri_texture);
	SkyBox(std::shared_ptr<TextureHDRI> hdri_texture);

	inline unsigned int get() { return cube_map->get(); }

	virtual void render(RenderContext ctx) override;
};