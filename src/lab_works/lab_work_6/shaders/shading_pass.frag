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
vec4 dataNormal = texelFetch( uPosFragsMap, coord, 0 );
vec4 dataAmbient = texelFetch( uPosFragsMap, coord, 0 );
vec4 dataDiffuse = texelFetch( uPosFragsMap, coord, 0 );
vec4 dataSpecular = texelFetch( uPosFragsMap, coord, 0 );
vec4 dataDepth = texelFetch( uPosFragsMap, coord, 0 );

in vec3 normal, lightPos, fragPos;
in vec2 texCoords;

uniform vec3 ambient, specular, diffuse;
uniform float shininess;
uniform bool uHasPosFragMap, uHasNormalMap, uHasAmbientMap, uHasDiffuseMap, uHasSpecularMap, uHasDepthMap;

in VS_OUT {
	vec3 Ptangent;
	vec3 Ltangent;
} fs_in;

void main()
{
	vec3 posFragTexture, ambientTexture, diffuseTexture, specularTexture, depthMapTexture;
	float shininessTexture, alphaDiffuseTexture;

	if (uHasPosFragMap) 
	{
		posFragTexture = vec3(texture(dataPos, coord));
	}

	if (uHasDepthMap) 
	{
		depthMapTexture = vec3(texture(uDepthMap, coord));
	}

	if (uHasDiffuseMap) 
	{
		diffuseTexture = vec3(texture(uDiffuseMap, coord));
		alphaDiffuseTexture = texture(uDiffuseMap, coord).w;
		if (alphaDiffuseTexture <= 0.5) discard;
	}
	else
	{
		diffuseTexture = diffuse;
	}

	if (uHasAmbientMap) 
	{
		ambientTexture = vec3(texture(uAmbientMap, coord));
	}
	else
	{
		ambientTexture = ambient;
	}

	if (uHasSpecularMap) 
	{
		specularTexture = texture(uSpecularMap, coord).xxx;
		shininessTexture = texture(uSpecularMap, coord).w;
	}
	else
	{
		specularTexture = specular;
		shininessTexture = shininess;
	}

	vec3 lightDir, viewDir, N;
	if ( uHasNormalMap )
	{
		lightDir = normalize( - fs_in.Ptangent );
		viewDir	 = normalize( -fs_in.Ptangent );
		N		 = texture( uNormalMap, coord ).xyz;
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