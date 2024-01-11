#version 450

layout( location = 0 ) out vec4 fragColor;

layout( binding = 0 ) uniform sampler2D uPosFragsMap;
layout( binding = 1 ) uniform sampler2D uNormalMap;
layout( binding = 2 ) uniform sampler2D uAmbientMap;
layout( binding = 3 ) uniform sampler2D uDiffuseMap;
layout( binding = 4 ) uniform sampler2D uSpecularMap;
layout( binding = 5 ) uniform sampler2D uDepthMap;

// Données du pixel courant
ivec2 coord = ivec2(gl_FragCoord.xy);

// Données des textures
vec4 dataPos = texelFetch( uPosFragsMap, coord, 0 );
vec4 dataNormal = texelFetch( uNormalMap, coord, 0 );
vec4 dataAmbient = texelFetch( uAmbientMap, coord, 0 );
vec4 dataDiffuse = texelFetch( uDiffuseMap, coord, 0 );
vec4 dataSpecular = texelFetch( uSpecularMap, coord, 0 );
vec4 dataDepth = texelFetch( uDepthMap, coord, 0 );

uniform vec3 ambient, specular, diffuse;
uniform float shininess;
uniform bool uHasPosFragMap, uHasNormalMap, uHasAmbientMap, uHasDiffuseMap, uHasSpecularMap, uHasDepthMap;

void main()
{
	vec3 posFragsTexture, normalTexture, ambientTexture, diffuseTexture, specularTexture, depthTexture;

	if ( uHasPosFragMap ) posFragsTexture = dataPos;
	
	if ( uHasNormalMap ) normalTexture = dataNormal;
	
	if ( uHasAmbientMap ) ambientTexture = dataAmbient;
	else ambientTexture = ambient;

	if ( uHasDiffuseMap ) diffuseTexture = dataDiffuse;
	else ambientTexture = diffuse;

	if ( uHasSpecularMap ) specularTexture = dataSpecular;
	else ambientTexture = specular;

	if ( uHasDepthMap ) depthTexture = dataDepth;

	fragColor = ambientTexture + diffuseTexture + specularTexture;
}