// This prefix is required for all vertex shader programs that are intended to be loaded for rendering

// Data passed from the vertex pass to the fragment pass
struct VertexData {
	vec3 position;
	vec3 normal;
	vec2 uv;
	mat3 TBN;
};

layout (location=0) in vec3 in_position;
layout (location=1) in vec3 in_normal;
layout (location=2) in vec2 in_uvs;
layout (location=3) in vec3 in_tangent;
layout (location=4) in vec3 in_bitangent;

uniform mat4 model_transform = mat4(1.0);
uniform mat4 view_transform = mat4(1.0);
uniform mat4 projection_transform = mat4(1.0);

out VertexData vertex;
