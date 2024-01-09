#version 450

layout( location = 0 ) out vec4 fragColor;

in vec3 normal, lightPos, fragPos;

uniform vec3 ambient, specular, diffuse;
uniform float shininess;

void main()
{
	vec3 lightDir  = normalize(lightPos - fragPos);
	vec3 viewDir = normalize(-fragPos);
	vec3 N = normalize(normal);	
	if(dot(N, lightDir) < 0)
        N = -N;

	vec3 reflectDir = reflect(-lightDir, N);
	vec3 diffuseColor = diffuse * max(dot(N, lightDir), 0.f);
	vec3 specularColor = specular * pow(max(dot(viewDir, reflectDir), 0.f), shininess);

	fragColor = vec4( ambient + diffuseColor + specularColor , 1.f );
}
