#version 450

layout( location = 0 ) in vec2 aVertexPosition;

// Variable d'entrée pour la couleur
layout( location = 1 ) in vec3 vertexColor;

// Variable de sortie pour passer la couleur au Fragment Shader
out vec4 couleurs;

// Variable de contrôle
uniform float uTranslationX;

void main() 
{
	couleurs = vec4( vertexColor, 1.0f );
	gl_Position = vec4(aVertexPosition.x + uTranslationX, aVertexPosition.y, 0.f, 1.f);
}
