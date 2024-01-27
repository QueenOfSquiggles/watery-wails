#version 460 core

layout (location=0) in vec3 aPos;
layout (location=1) in vec3 aColor;
layout (location=2) in vec2 aTexCoord;

uniform mat4 model_transform;
uniform mat4 view_transform = mat4(1.0);
uniform mat4 projection_transform;


out vec3 colour;
out vec2 uv;

void main() {
	gl_Position = projection_transform * view_transform * model_transform * vec4(aPos, 1.0);
	colour = aColor;
	uv = aTexCoord;
}