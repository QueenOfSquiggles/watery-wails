#include "components/constants.shader"
#include "components/light_structs.shader"


struct LightingContext {
	vec3 vertex;
	vec3 albedo;
	vec3 normal;
	vec3 cam_position;
	vec3 orm;
	float shiny_factor;
};

LightingContext build_context(vec3 vertex, vec3 normal, vec3 cam_position, float shiny_factor, vec3 orm, vec3 albedo)
{
	LightingContext ctx;
	ctx.vertex = vertex;
	ctx.normal = normal;
	ctx.cam_position = cam_position;
	ctx.shiny_factor = shiny_factor;
	ctx.orm = orm;
	ctx.albedo = albedo;

	return ctx;
}

#ifndef NR_POINT_LIGHTS
#define NR_POINT_LIGHTS 4
#endif

#ifndef AMBIENT_LIGHT_VALUE
#define AMBIENT_LIGHT_VALUE 0.133
#endif

vec3 fresnel_schlick(float cos_theta, vec3 f0) {
	return f0 + (1.0 - f0) * pow(clamp(1.0 - cos_theta, 0.0, 1.0), 5.0);
}

float distribution_ggx(vec3 N, vec3 H, float roughness) {
	float a = roughness * roughness;
	float a2 = a*a;
	float ndoth = max(dot(N, H), 0.0);
	float ndoth2 = ndoth * ndoth;

	float num = a2;
	float denom = (ndoth2 * (a2 - 1.0) + 1.0);
	denom = PI * denom * denom;
	return num / denom;
}

float geometry_schlick_ggx(float ndotv, float roughness){
	float r = (roughness + 1.0);
	float k = (r*r) / 8.0;

	float num = ndotv;
	float denom = ndotv * (1.0 - k) + k;
	return num / denom;
}

float geometry_smith(vec3 N, vec3 V, vec3 L, float roughness) {
	float ndotv = max(dot(N, V), 0.0);
	float ndotl = max(dot(N, L), 0.0);
	float ggx2 = geometry_schlick_ggx(ndotv, roughness);
	float ggx1 = geometry_schlick_ggx(ndotl, roughness);
	return ggx1 * ggx2;
	
}

vec3 lighting_pass(LightingContext ctx, DirectionalLight sun, PointLight lights[NR_POINT_LIGHTS], int active_lights)
{
	float ao = ctx.orm.x;
	float roughness = ctx.orm.y;
	float metallic = ctx.orm.z;
	vec3 N = normalize(ctx.normal);
	vec3 V = normalize(ctx.cam_position);
	vec3 ambient = vec3(AMBIENT_LIGHT_VALUE) * ctx.albedo * ao;
	// direct
	vec3 Lo = vec3(0.0);
	vec3 light_pos = normalize(sun.direction) + ctx.vertex;
	vec3 light_colour = sun.colour;
	for (int i = -1; i < active_lights; i++) {
		if (i >= 0) {
			light_pos = lights[i].position;
			light_colour = lights[i].colour;
		}
		vec3 L = normalize(light_pos - ctx.vertex);
		vec3 H = normalize(V + L);

		float dist = length(light_pos - ctx.vertex);
		float attenuation = 1.0 / (dist * dist);
		vec3 radiance = light_colour * attenuation;

		vec3 f0 = mix(vec3(0.04), ctx.albedo, metallic);
		vec3 F = fresnel_schlick(max(dot(H, V), 0.0), f0);
		float NDF = distribution_ggx(N,H, roughness);
		float G = geometry_smith(N, V, L, roughness);

		vec3 numerator = NDF * G * F;
		float ndotl = max(dot(N, L), 0.0);
		float denominator = 4.0 * max(dot(N, V), 0.0) * ndotl + 0.0001;

		vec3 specular = numerator / denominator;

		vec3 ks = F;
		vec3 kd = vec3(1.0) - ks;

		kd *= 1.0 - metallic;
		Lo += (kd * ctx.albedo / PI + specular) * radiance * ndotl;
	}
	return Lo + ambient;
}