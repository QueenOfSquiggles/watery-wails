#define NR_POINT_LIGHTS 4

#include "includes/fragment_prefix.shader"
#include "includes/lights.shader"

uniform DirectionalLight sun;
uniform PointLight lights_point[NR_POINT_LIGHTS];
uniform int lights_active = 1;

void main()
{
	vec4 surf_color = texture(material.albedo, vertex.uv);
	vec3 albedo = surf_color.rgb;
	vec3 n = normalize(texture(material.normal, vertex.uv).rgb);
	vec3 orm = texture(material.orm, vertex.uv).rgb;

	LightingContext ctx = build_context(
		vertex.position,
		normalize(vertex.normal),
		normalize(environment.camera_position - vertex.position),
		material.specular_strength,
		orm.g
	);

	vec3 lighting = environment.ambient_light;

	lighting += _get_directional_affect(sun, ctx);
	lighting += _get_point_affect(lights_point[0], ctx);

	FragColor = vec4((lighting * albedo), surf_color.a);
}