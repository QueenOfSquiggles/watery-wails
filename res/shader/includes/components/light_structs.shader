struct DirectionalLight {
	vec3 direction;
	vec3 colour;
};

struct PointLight {
	vec3 position;
	vec3 colour;
	vec3 attenuation_factors; // e.g. linear = vec3(0, 1, 0)
};
