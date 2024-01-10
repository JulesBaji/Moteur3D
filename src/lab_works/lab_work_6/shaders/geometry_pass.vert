#version 450

layout( location = 0 ) in vec3 aVertexPosition;
layout( location = 1 ) in vec3 aVertexNormal;
layout( location = 2 ) in vec2 aVertexTexCoords;
layout( location = 3 ) in vec3 aVertexTangent;
layout( location = 4 ) in vec3 aVertexBitangent;

out vec3 normal, lightPos, fragPos;
out vec2 texCoords;

uniform mat4 uMVPMatrix; //  Model * View * Projection
uniform mat4 uMVMatrix; // Model * View
uniform mat4 uNormalMatrix;

out VS_OUT {
	vec3 Ptangent;
	vec3 Ltangent;
} vs_out; 

void main()
{
    gl_Position = uMVPMatrix * vec4( aVertexPosition, 1.f );
    normal = vec3( uNormalMatrix * vec4(aVertexNormal, 1.f));
    fragPos = vec3( uMVMatrix * vec4(aVertexPosition, 1.f));
    texCoords = aVertexTexCoords;
	vec3 T			 = normalize( vec3( uNormalMatrix * vec4( aVertexTangent, 1.f ) ) );
	vec3 N			 = normalize( normal );
	T				 = normalize( T - dot( T, N ) * N ); // On veut T orthogonal a N
	vec3 B			 = cross( N, T );
	mat3 inv_TBN	 = transpose( mat3( T, B, N ) );
	vs_out.Ptangent	 = inv_TBN * fragPos;
	vs_out.Ltangent	 = inv_TBN * lightPos;
}
