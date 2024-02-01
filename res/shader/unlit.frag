#include "includes/fragment_prefix.shader"

void main()
{
	FragColor = texture(material.albedo, vertex.uv);
}