#include "skybox.hpp"

SkyBox::SkyBox(std::filesystem::path hdri_texture) : SkyBox(std::shared_ptr<TextureHDRI>(new TextureHDRI(hdri_texture)))
{
}

SkyBox::SkyBox(std::shared_ptr<TextureHDRI> hdri_texture) : Renderable()
{
	cube_map = std::shared_ptr<CubeMap>(new CubeMap(hdri_texture));
	cube_mesh = std::shared_ptr<PrimitiveCube>(new PrimitiveCube(1.0f));
}

void SkyBox::render(RenderContext ctx)
{
	if (!cube_map || !cube_mesh)
	{
		std::cerr << "Something went wrong! null cube data in skybox!!" << std::endl;
		return;
	}
	cube_map->bind_slot(0);
	cube_mesh->render(ctx);
	cube_map->unbind_slot(0);
}
