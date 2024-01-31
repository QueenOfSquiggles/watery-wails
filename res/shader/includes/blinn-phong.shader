vec4 blinn_phong(Environment env, Light light, Material mat) 
{
	vec4 surf_color = texture(material.albedo, vertex.uv);
	vec3 albedo = surf_color.rgb;
	vec3 n = texture(material.normal, vertex.uv).rgb;
	vec3 orm = texture(material.orm, vertex.uv).rgb;

	// diffuse
	vec3 norm = normalize(vertex.normal);
	vec3 light_dir = normalize(-light.direction);
	vec3 diffuse = light.colour * albedo * max(dot(norm, light_dir), 0.0);

	vec3 view = normalize(environment.camera_position - vertex.position);
	vec3 reflect_dir = reflect(-light_dir, norm);
	vec3 specular = light.colour * (1.0 - orm.r) * pow(max(dot(view, reflect_dir), 0.0), 32);

	return vec4(
		environment.ambient_light + diffuse + specular
		, surf_color.a
	);
}