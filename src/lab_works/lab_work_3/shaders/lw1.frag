#version 450

layout( location = 0 ) out vec4 fragColor;

// Variable d'entree pour recuperer la couleur du Vertex Shader
in vec4 couleurs;

// Variable de contrôle
uniform float luminosite;

void main() 
{
	fragColor = couleurs * luminosite;
}
