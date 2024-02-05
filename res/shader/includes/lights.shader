#include "components/light_structs.shader"

struct LightingContext {
	vec3 vertex;
	vec3 albedo;
	vec3 normal;
	vec3 view_dir;
	vec3 orm;
	float shiny_factor;
};

vec3 _get_point_affect(PointLight light, LightingContext ctx)
{
	vec3 light_dir = normalize(light.position - ctx.vertex);
	float diffuse = max(dot(ctx.normal, light_dir), 0.0);
	vec3 reflect_dir = reflect(-light_dir, ctx.normal);
	float spec = pow(max(dot(ctx.view_dir, reflect_dir), 0.0), ctx.shiny_factor);
	float dist = length(light.position - ctx.vertex);
	float atten = 1.0 / 
		(light.attenuation_factors.x) +
		(light.attenuation_factors.y * dist) +
		(light.attenuation_factors.z * dist * dist);
	atten = clamp(atten, 0.0, 1.0);

	vec3 diffuse_colour = ctx.albedo * light.colour * diffuse * atten;
	vec3 specular_colour = ctx.albedo * light.colour * spec * atten * (1.0 - ctx.orm.y);

	return diffuse_colour + specular_colour;
}

vec3 _get_directional_affect(DirectionalLight light, LightingContext ctx) 
{
	vec3 light_dir = normalize(-light.direction);
	float diffuse = max(dot(ctx.normal, light_dir), 0.0);
	vec3 reflect_dir = reflect(-light_dir, ctx.normal);
	float spec = pow(max(dot(ctx.view_dir, reflect_dir), 0.0), ctx.shiny_factor);
	vec3 diffuse_colour = ctx.albedo * light.colour * diffuse;
	vec3 specular_colour = ctx.albedo * light.colour * spec * (1.0 - ctx.orm.y);
	return diffuse_colour + specular_colour;
}

LightingContext build_context(vec3 vertex, vec3 normal, vec3 view_dir, float shiny_factor, vec3 orm, vec3 albedo)
{
	LightingContext ctx;
	ctx.vertex = vertex;
	ctx.normal = normal;
	ctx.view_dir = view_dir;
	ctx.shiny_factor = shiny_factor;
	ctx.orm = orm;
	ctx.albedo = albedo
	return ctx;
}

#ifndef NR_POINT_LIGHTS
#define NR_POINT_LIGHTS 4
#endif

vec3 lighting_pass(LightingContext ctx, DirectionalLight sun, PointLight lights[NR_POINT_LIGHTS], int active_lights)
{
	vec3 sum = vec3(0.0);
	sum += max(_get_directional_affect(sun, ctx), vec3(0.0));
	for (int i = 0; i < active_lights; i++) {
		sum += max(_get_point_affect(lights[i], ctx), vec3(0.0));
	}
	return sum;
}