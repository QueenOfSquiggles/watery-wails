// This prefix is required for all fragment shader programs that are intended to be loaded for rendering

// Data for the material of the rendered surface
struct Material {
	sampler2D albedo;
	sampler2D normal;
	sampler2D orm;
	float specular_strength;
};
// The environmental data
struct Environment {
	vec3 ambient_light;
	vec3 camera_position;
	float time;
};
// Lighting information
struct Light {
	vec3 colour;
	vec3 direction;
};

// Data passed from the vertex pass to the fragment pass
struct VertexData {
	vec3 position;
	vec3 normal;
	vec2 uv;
};


in VertexData vertex;

uniform Environment environment;
uniform Light light;
uniform Material material;

out vec4 FragColor;
