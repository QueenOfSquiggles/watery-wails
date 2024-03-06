#version 460 core
out vec4 FragColor;
in vec3 position;

uniform samplerCube environment;

void main()
{
	vec3 env_color = texture(environment, position).rgb;
	env_color = env_color / (env_color + vec3(1.0));
	env_color = pow(env_color, vec3(1.0 / 2.2));
	FragColor = vec4(env_color, 1.0);
	//
	FragColor = vec4(1.0);
}