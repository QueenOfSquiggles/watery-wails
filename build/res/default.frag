#version 460 core

out vec4 FragColor;

in vec3 colour;
in vec2 uv;

uniform sampler2D albedo;
uniform sampler2D normal;
uniform sampler2D orm;

void main()
{
	FragColor = texture(albedo, uv);
}