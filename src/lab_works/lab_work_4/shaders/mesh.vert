#version 450

layout( location = 0 ) in vec3 aVertexPosition;
layout( location = 1 ) in vec3 aVertexNormal;
layout( location = 2 ) in vec2 aVertexTexCoords;
layout( location = 3 ) in vec3 aVertexTangent;
layout( location = 4 ) in vec3 aVertexBitangent;

out vec3 normal, lightPos, fragPos;         // Normale, position du fragment et position de la lumiere

uniform mat4 uMVPMatrix; //  Model * View * Projection
uniform mat4 uMVMatrix; // Model * View
uniform mat4 normalMatrix;

void main()
{
    gl_Position = uMVPMatrix * vec4( aVertexPosition, 1.f );
    normal = vec3( normalMatrix * vec4(aVertexNormal, 1.f));
    fragPos = vec3( uMVMatrix * vec4(aVertexPosition, 1.f));
}
