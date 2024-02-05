#define NR_POINT_LIGHTS 4

#include "includes/fragment_prefix.shader"
#include "includes/pbr_lighting.shader"

uniform DirectionalLight sun;
uniform PointLight lights_point[NR_POINT_LIGHTS];
uniform int lights_active = 1;

void main()
{
	vec4 surf_color = texture(material.albedo, vertex.uv);
	vec3 albedo = surf_color.rgb;
	vec3 normal = texture(material.normal, vertex.uv).rgb;
	normal = normalize(normal * 2.0 - 1.0);
	normal = normalize(vertex.TBN * normal);
	vec3 orm = texture(material.orm, vertex.uv).rgb;

	LightingContext ctx = build_context(
		vertex.position,
		normal,
		environment.camera_position,
		material.specular_strength,
		orm,
		albedo
	);
	// pbr lighting calc
	vec3 colour = lighting_pass(ctx, sun, lights_point, lights_active);
	
	// gamma correct
	colour = colour / (colour + vec3(1.0));
	colour = pow(colour, vec3(1.0 / 2.2));

	// force min lighting
	// color = max(color, vec3(0.2));

	FragColor = vec4(colour, 1.0);
	
	// FragColor = vec4(albedo, 1.0);
}