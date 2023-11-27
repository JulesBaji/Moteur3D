#version 450

layout( location = 0 ) out vec4 fragColor;

uniform vec3 ambient, diffuse, shininess;

void main()
{
	fragColor = vec4( ambient, 1.0f );
}
