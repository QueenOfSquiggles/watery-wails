#include "skybox.hpp"

SkyBox::SkyBox(std::filesystem::path hdri_texture) : Renderable()
{
	auto tex = std::shared_ptr<TextureHDRI>(new TextureHDRI(hdri_texture));
	SkyBox(std::weak_ptr(tex));
}

SkyBox::SkyBox(std::weak_ptr<TextureHDRI> hdri_texture) : Renderable()
{
	cube_map = std::shared_ptr<CubeMap>(new CubeMap(hdri_texture));
	cube_mesh = std::shared_ptr<Mesh>(new Mesh("res/model/internal/simple_cube.gltf"));
}

void SkyBox::render(RenderContext ctx)
{
	if (!cube_map || !cube_mesh)
	{
		std::cerr << "Something went wrong! null cube data in skybox!!" << std::endl;
		return;
	}
	cube_map->bind();
	cube_mesh->render(ctx);
	cube_map->unbind();
}
