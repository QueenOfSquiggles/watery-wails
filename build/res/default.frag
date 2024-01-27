#version 460 core

out vec4 FragColor;

in vec3 colour;
in vec2 uv;

uniform sampler2D albedo;
uniform sampler2D normal;
uniform sampler2D orm;
uniform float time;

const float SPEED = 1.0;

void main()
{
	vec4 al = texture(albedo, uv);
	vec4 n = texture(normal, uv);
	float factor = (cos(time * SPEED) * 0.5 + 0.5);
	FragColor = vec4(mix(al.rgb, n.rgb, factor), 1.0);
}