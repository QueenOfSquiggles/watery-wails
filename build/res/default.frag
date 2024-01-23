#version 460 core

out vec4 FragColor;

in vec3 colour;
in vec2 uv;

uniform sampler2D wall_tex;
uniform sampler2D face_tex;
uniform float gradient;

void main()
{
	vec4 wall = texture(wall_tex, uv);
	vec4 face = texture(face_tex, uv);
	FragColor = vec4(mix(wall.rgb, face.rgb, gradient) * colour, 1.0);
}