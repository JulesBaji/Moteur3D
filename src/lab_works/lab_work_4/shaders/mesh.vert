#version 450

layout( location = 0 ) in vec3 aVertexPosition;
layout( location = 1 ) in vec3 aVertexNormal;
layout( location = 2 ) in vec2 aVertexTexCoords;
layout( location = 3 ) in vec3 aVertexTangent;
layout( location = 4 ) in vec3 aVertexBitangent;

out vec3 normal, L;         // Position et normale du fragment 

uniform mat4 uMVPMatrix; // Projection * View * Model
uniform mat4 normalMatrix;

void main()
{
    // Transformation de la normale du vertex
    normal = normalize(mat3(normalMatrix) * aVertexNormal);

    L = reflect(-aVertexPosition, normal);

	gl_Position = uMVPMatrix * vec4( aVertexPosition, 1.f );
}
