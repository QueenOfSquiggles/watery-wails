#include "cubemap.hpp"
#include "engine/render_system/mesh/primitives.hpp"

CubeMap::CubeMap(std::weak_ptr<TextureHDRI> hdri_equirectangular) : Texture()
{

	width = height = CUBE_MAP_FACE_SIZE;
	channels = 3;
	unsigned int capture_fbo, capture_rbo;
	glGenFramebuffers(1, &capture_fbo);
	glGenRenderbuffers(1, &capture_rbo);

	glBindFramebuffer(GL_FRAMEBUFFER, capture_fbo);
	glBindRenderbuffer(GL_RENDERBUFFER, capture_rbo);
	glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT24, width, height);
	glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, capture_rbo);

	glGenTextures(1, &id);
	glBindTexture(GL_TEXTURE_CUBE_MAP, id);
	for (unsigned int i = 0; i < 6; i++)
	{
		glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, GL_RGB16F, width, height, 0, GL_RGB, GL_FLOAT, nullptr);
	}
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glm::mat4 capture_projection = glm::perspective(glm::radians(90.0f), 1.0f, 0.1f, 10.0f);
	glm::mat4 capture_view[] = {
		glm::lookAt(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(1.0f, 0.0f, 0.0f), glm::vec3(0.0f, -1.0f, 0.0f)),
		glm::lookAt(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(-1.0f, 0.0f, 0.0f), glm::vec3(0.0f, -1.0f, 0.0f)),
		glm::lookAt(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, 1.0f, 0.0f), glm::vec3(0.0f, 0.0f, 1.0f)),
		glm::lookAt(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, -1.0f, 0.0f), glm::vec3(0.0f, 0.0f, -1.0f)),
		glm::lookAt(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, 0.0f, 1.0f), glm::vec3(0.0f, -1.0f, 0.0f)),
		glm::lookAt(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, 0.0f, -1.0f), glm::vec3(0.0f, -1.0f, 0.0f)),
	};
	auto shader_convert = std::shared_ptr<ShaderProgram>(new ShaderProgram{
		"res/shader/conversion/rect2cube.vert",
		"res/shader/conversion/rect2cube.frag",
		false,
	});
	shader_convert->enable();
	shader_convert->set_int("equirectangular_map", 0);
	shader_convert->set_mat4("projection", capture_projection);
	auto hdri = hdri_equirectangular.lock();
	if (!hdri)
	{
		std::cerr << "Failed to get a lock on HDRI texture during cubemap initialization!" << std::endl;
	}
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, hdri->get());

	int previous_viewport[4];
	glGetIntegerv(GL_VIEWPORT, previous_viewport);

	glViewport(0, 0, width, height);
	glBindFramebuffer(GL_FRAMEBUFFER, capture_fbo);

	auto cube = new PrimitiveCube(1.0);
	RenderContext ctx;
	ctx.batch_name = "cubmap_conversion";
	ctx.delta = 0;
	ctx.program = shader_convert;

	for (unsigned int i = 0; i < 6; i++)
	{
		shader_convert->set_mat4("view", capture_view[i]);
		glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, id, 0);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		cube->render(ctx);
	}
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
	glViewport(previous_viewport[0], previous_viewport[1], previous_viewport[2], previous_viewport[3]);
	shader_convert->disable();
}

void CubeMap::bind() { bind_slot(0); }
void CubeMap::unbind() { unbind_slot(0); }

void CubeMap::bind_slot(unsigned int slot)
{
	if (slot > 31)
	{
		std::cerr << "Texture slot index is above slot cap! Max value is " << (GL_TEXTURE31 - GL_TEXTURE0) << "!" << std::endl;
		return;
	}
	glActiveTexture(GL_TEXTURE0 + slot);
	glBindTexture(GL_TEXTURE_CUBE_MAP, id);
}

void CubeMap::unbind_slot(unsigned int slot)
{
	if (slot > 31)
	{
		std::cerr << "Texture slot index is above slot cap! Max value is " << (GL_TEXTURE31 - GL_TEXTURE0) << "!" << std::endl;
		return;
	}
	glActiveTexture(GL_TEXTURE0 + slot);
	glBindTexture(GL_TEXTURE_CUBE_MAP, 0);
}
