#version 450

layout( location = 0 ) out vec4 fragColor;

layout( binding = 0 ) uniform sampler2D uAmbientMap;
layout( binding = 1 ) uniform sampler2D uDiffuseMap;
layout( binding = 2 ) uniform sampler2D uShininessMap;
layout( binding = 3 ) uniform sampler2D uSpecularMap;
layout( binding = 4 ) uniform sampler2D uNormalMap;

in vec3 normal, lightPos, fragPos;
in vec2 texCoords;

uniform vec3 ambient, specular, diffuse;
uniform float shininess;
uniform bool uHasAmbientMap, uHasDiffuseMap, uHasShininesseMap, uHasSpecularMap, uHasNormalMap;

in VS_OUT {
	vec3 Ptangent;
	vec3 Ltangent;
} fs_in;

void main()
{
	vec3 ambientTexture, diffuseTexture, specularTexture, normalTexture;
	float shininessTexture, alphaDiffuseTexture;

	if (uHasDiffuseMap) 
	{
		diffuseTexture = vec3(texture(uDiffuseMap, texCoords));
		alphaDiffuseTexture = texture(uDiffuseMap, texCoords).w;
		if (alphaDiffuseTexture <= 0.5) discard;
	}
	else
	{
		diffuseTexture = diffuse;
	}

	if (uHasAmbientMap) 
	{
		ambientTexture = vec3(texture(uAmbientMap, texCoords));
	}
	else
	{
		ambientTexture = ambient;
	}

	if (uHasShininesseMap) 
	{
		shininessTexture = texture(uShininessMap, texCoords).x;
	}
	else
	{
		shininessTexture = shininess;
	}

	if (uHasSpecularMap) 
	{
		specularTexture = texture(uSpecularMap, texCoords).xxx;
	}
	else
	{
		specularTexture = specular;
	}

	vec3 lightDir, viewDir, N;
	if ( uHasNormalMap )
	{
		lightDir = normalize( - fs_in.Ptangent );
		viewDir	 = normalize( -fs_in.Ptangent );
		N		 = texture( uNormalMap, texCoords ).xyz;
		N		 = normalize( N * 2.0 - 1.0 );
	}
	else
	{
		lightDir = normalize( lightPos - fragPos );
		viewDir	 = normalize( -fragPos );
		N		 = normalize( normal );
	}

	if ( dot( N, lightDir ) < 0 )
		N = -N;

	vec3 reflectDir = reflect( -lightDir, N );
	vec3 diffuseColor = diffuseTexture * max( dot( N, lightDir ), 0.f );
	vec3 specularColor = specularTexture * pow(max(dot(viewDir, reflectDir), 0.f), shininessTexture);

	fragColor = vec4( ambientTexture + diffuseColor + specularColor, 1.0 );
}
