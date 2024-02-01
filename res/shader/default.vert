#include "includes/vertex_prefix.shader"

void main() {
	vertex.position = (model_transform * vec4(in_position, 1.0)).xyz;
	vertex.normal = normalize(mat3(transpose(inverse(model_transform))) * in_normal);
	vertex.uv = in_uvs;

	gl_Position = projection_transform * view_transform * vec4(vertex.position, 1.0);
}