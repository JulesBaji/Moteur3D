#version 450

layout( location = 0 ) out vec4 fragColor;

layout( binding = 0 ) uniform sampler2D uPosFragsMap;
layout( binding = 1 ) uniform sampler2D uNormalMap;
layout( binding = 2 ) uniform sampler2D uAmbientMap;
layout( binding = 3 ) uniform sampler2D uDiffuseMap;
layout( binding = 4 ) uniform sampler2D uSpecularMap;
layout( binding = 5 ) uniform sampler2D uDepthMap;

in vec3 lightPos;

void main()
{
	// Données du pixel courant
	ivec2 coord = ivec2(gl_FragCoord.xy);

	// Données des textures
	vec4 dataPos = texelFetch( uPosFragsMap, coord, 0 );
	vec4 dataNormal = texelFetch( uNormalMap, coord, 0 );
	vec4 dataAmbient = texelFetch( uAmbientMap, coord, 0 );
	vec4 dataDiffuse = texelFetch( uDiffuseMap, coord, 0 );
	vec4 dataSpecular = texelFetch( uSpecularMap, coord, 0 );
	vec4 dataDepth = texelFetch( uDepthMap, coord, 0 );

	vec4 lightDir, viewDir;

	lightDir = normalize( vec4(lightPos,1.0) - dataPos );
	viewDir	 = normalize( -dataPos );

	vec4 normal = dataNormal;
	if ( dot( normal, lightDir ) < 0 )
		normal = -normal;

	vec4 reflectDir = reflect( -lightDir, normal );
	vec4 diffuseColor = dataDiffuse * max( dot( normal, lightDir ), 0.f );
	vec4 specularColor = dataSpecular * pow(max(dot(viewDir, reflectDir), 0.f), dataSpecular.w);

	//fragColor = dataAmbient + diffuseColor + specularColor;
	fragColor = vec4(1.f,0.f,0.f,0.f);
}