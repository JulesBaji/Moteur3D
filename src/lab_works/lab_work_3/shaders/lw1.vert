#version 450

layout( location = 0 ) in vec3 aVertexPosition;

// Variable d'entrée pour la couleur
layout( location = 1 ) in vec3 vertexColor;

// Variable de sortie pour passer la couleur au Fragment Shader
out vec4 couleurs;

// Matrice MVP
uniform mat4 MVP;

void main() 
{
	couleurs = vec4( vertexColor, 1.0f );
	gl_Position = MVP * vec4(aVertexPosition, 1.f);
}
