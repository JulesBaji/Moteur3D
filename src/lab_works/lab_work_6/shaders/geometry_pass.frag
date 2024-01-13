#version 450

layout( location = 0 ) out vec4 fragPosition;
layout( location = 1 ) out vec4 fragNormal;
layout( location = 2 ) out vec4 fragAmbient;
layout( location = 3 ) out vec4 fragDiffuse;
layout( location = 4 ) out vec4 fragSpecular;

layout( binding = 0 ) uniform sampler2D uAmbientMap;
layout( binding = 1 ) uniform sampler2D uDiffuseMap;
layout( binding = 2 ) uniform sampler2D uShininessMap;
layout( binding = 3 ) uniform sampler2D uSpecularMap;
layout( binding = 4 ) uniform sampler2D uNormalMap;

in vec3 normal, fragPos;
in vec2 texCoords;

uniform vec3 ambient, specular, diffuse;
uniform float shininess;
uniform bool uHasAmbientMap, uHasDiffuseMap, uHasShininesseMap, uHasSpecularMap, uHasNormalMap;

void main()
{
	vec3 ambientTexture, diffuseTexture, specularTexture;
	float shininessTexture, alphaDiffuseTexture;

	if (uHasDiffuseMap) 
	{
		diffuseTexture = vec3(texture(uDiffuseMap, texCoords));
		alphaDiffuseTexture = texture(uDiffuseMap, texCoords).w;
		if (alphaDiffuseTexture <= 0.5) discard;
	}
	else diffuseTexture = diffuse;

	if (uHasAmbientMap) ambientTexture = texture(uAmbientMap, texCoords).xyz;
	else ambientTexture = ambient;

	if (uHasShininesseMap) shininessTexture = texture(uShininessMap, texCoords).x;
	else shininessTexture = shininess;

	if (uHasSpecularMap) specularTexture = texture(uSpecularMap, texCoords).xxx;
	else specularTexture = specular;

	fragPosition = vec4( fragPos, 1.f );
	fragNormal = vec4( normal, 1.f );
	fragAmbient = vec4( ambientTexture, 1.f );
	fragDiffuse = vec4( alphaDiffuseTexture );
	fragSpecular = vec4( specularTexture, shininessTexture );
}
