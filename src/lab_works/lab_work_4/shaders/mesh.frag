#version 450

layout( location = 0 ) out vec4 fragColor;

in vec3 normal, fragPos, viewDir;

uniform vec3 ambient, diffuse, specular;
uniform float shininess;

void main()
{
	// ViewDir c pareil
	vec3 lightDir  = normalize(-fragPos);
	vec3 N = normalize(normal);
	vec3 reflectDir = reflect(-lightDir, N);

	vec3 diffuse = diffuse * max(dot(N, lightDir), 0.f);
	vec3 specular = specular * pow(max(dot(lightDir, reflectDir), 0.f), shininess);

	fragColor = vec4( ambient + diffuse + specular  , 1.f );
}
