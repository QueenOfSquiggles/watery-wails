#include "structs.shaderinclude"

in VertexData vertex;

uniform Environment environment;
uniform Light light;
uniform Material material;

out vec4 FragColor;


void main()
{
	FragColor = texture(material.albedo, vertex.uv);
}