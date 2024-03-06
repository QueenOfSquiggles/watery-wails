#version 460 core

out vec4 colour;
in vec3 position;

uniform sampler2D equirectangular_map;

const vec2 inv_atan = vec2(0.1591, 0.3183);

vec2 sample_sphere(vec3 v) {
	vec2 uv = vec2(atan(v.z, v.x), asin(v.y));
	uv *= inv_atan;
	uv += 0.5;
	return uv;
}

void main() {
	vec2 uv = sample_sphere(normalize(position));
	vec3 col = texture(equirectangular_map, uv).rgb;
	colour = vec4(col, 1.0);
}
