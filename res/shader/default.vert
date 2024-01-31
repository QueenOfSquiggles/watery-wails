layout (location=0) in vec3 in_position;
layout (location=1) in vec3 in_normal;
layout (location=2) in vec2 in_uvs;

uniform mat4 model_transform = mat4(1.0);
uniform mat4 view_transform = mat4(1.0);
uniform mat4 projection_transform = mat4(1.0);

out VertexData vertex;

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