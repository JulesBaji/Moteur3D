#version 450

layout( location = 0 ) out vec4 fragColor;

in vec3 normal, L;

uniform vec3 ambient, diffuse, shininess, directionLum;

void main()
{

	float theta = acos(dot(normal, L));

    vec3 couleurDiffuse = diffuse * cos(theta);

	fragColor = vec4( ambient + couleurDiffuse, 1.0f );
}
