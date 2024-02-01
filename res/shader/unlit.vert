#include "includes/vertex_prefix.shader"

void main() {
	mat4 trans =
		projection_transform * 
		view_transform * 
		model_transform;

	gl_Position = trans * vec4(in_position, 1.0);
	vertex.position = (model_transform * vec4(in_position, 1.0)).xyz;
	vertex.uv = in_uvs;
	vertex.normal = mat3(transpose(inverse(model_transform))) * in_normal;

}