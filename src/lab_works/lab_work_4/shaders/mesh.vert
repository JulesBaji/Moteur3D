#version 450

layout( location = 0 ) in vec3 aVertexPosition;
layout( location = 1 ) in vec3 aVertexNormal;
layout( location = 2 ) in vec2 aVertexTexCoords;
layout( location = 3 ) in vec3 aVertexTangent;
layout( location = 4 ) in vec3 aVertexBitangent;

out vec3 normal, fragPos, lightPos;         // Position et normale du fragment 

uniform mat4 uMVPMatrix; // Projection * View * Model
uniform mat4 uMMatrix;
uniform mat4 normalMatrix;
uniform vec3 posLum;


void main()
{
    gl_Position = uMVPMatrix * vec4( aVertexPosition, 1.f );
    normal = vec3( normalMatrix * vec4(aVertexNormal, 1.f));
    fragPos = vec3( uMMatrix * vec4(aVertexPosition, 1.f));
}
