#version 450

layout( location = 0 ) out vec4 fragColor;

layout( binding = 0 ) uniform sampler2D uPosFragsMap;
layout( binding = 1 ) uniform sampler2D uNormalMap;
layout( binding = 2 ) uniform sampler2D uAmbientMap;
layout( binding = 3 ) uniform sampler2D uDiffuseMap;
layout( binding = 4 ) uniform sampler2D uSpecularMap;
layout( binding = 5 ) uniform sampler2D uDepthMap;

const vec3 lightPos = vec3( 0.0 ); // light at the camera (view space), as in TP 5

void main()
{
	ivec2 coord = ivec2( gl_FragCoord.xy );

	vec4 pos = texelFetch( uPosFragsMap, coord, 0 );
	if ( pos.w == 0.0 ) discard; // background: no geometry written here

	vec3 fragPos  = pos.xyz;
	vec3 N        = normalize( texelFetch( uNormalMap, coord, 0 ).xyz );
	vec3 ambient  = texelFetch( uAmbientMap, coord, 0 ).xyz;
	vec3 diffuse  = texelFetch( uDiffuseMap, coord, 0 ).xyz;
	vec4 spec     = texelFetch( uSpecularMap, coord, 0 );

	vec3 lightDir = normalize( lightPos - fragPos );
	vec3 viewDir  = normalize( -fragPos );
	if ( dot( N, lightDir ) < 0 ) N = -N;

	vec3 reflectDir = reflect( -lightDir, N );
	vec3 diffuseColor  = diffuse * max( dot( N, lightDir ), 0.f );
	vec3 specularColor = spec.xyz * pow( max( dot( viewDir, reflectDir ), 0.f ), spec.w );

	fragColor = vec4( ambient + diffuseColor + specularColor, 1.0 );
}
