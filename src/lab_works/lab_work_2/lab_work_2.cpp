#include "imgui.h"
#include "lab_work_2.hpp"
#include "utils/read_file.hpp"
#include "glm/gtc/type_ptr.hpp"
#include "utils/random.hpp"
#include <iostream>

namespace M3D_ISICG
{
	const std::string  LabWork2::_shaderFolder = "src/lab_works/lab_work_2/shaders/";

	LabWork2::~LabWork2()
	{
		glDeleteProgram( program );

		// Destruction buffer
		glDeleteBuffers( 1, &vbo );
		glDeleteBuffers( 1, &vboCouleur );
		glDeleteBuffers( 1, &ebo );

		// Destruction connexion VAO avec Vertex Shader
		glDisableVertexArrayAttrib( vao, 0 );
		glDisableVertexArrayAttrib( vao, 1 );

		// Destruction VAO
		glDeleteVertexArrays( 1, &vao );
	}

	bool LabWork2::init()
	{

		std::cout << "Initializing lab work 1..." << std::endl;

		// Set the color used by glClear to clear the color buffer (in render()).
		glClearColor( _bgColor.x, _bgColor.y, _bgColor.z, _bgColor.w );

		const std::string fragmentShaderStr = readFile( _shaderFolder + "lw1.frag" );
		const std::string vertexShaderStr	= readFile( _shaderFolder + "lw1.vert" );

		const GLuint fragmentShader = glCreateShader( GL_FRAGMENT_SHADER );
		const GLuint vertexShader	= glCreateShader( GL_VERTEX_SHADER );

		const GLchar * vSrcFrag = fragmentShaderStr.c_str();
		const GLchar * vSrcVert = vertexShaderStr.c_str();

		glShaderSource( fragmentShader, 1, &vSrcFrag, NULL );
		glShaderSource( vertexShader, 1, &vSrcVert, NULL );

		glCompileShader( fragmentShader );
		glCompileShader( vertexShader );

		// Check if compilation is ok.
		GLint compiled;
		glGetShaderiv( vertexShader, GL_COMPILE_STATUS, &compiled );
		if ( !compiled )
		{
			GLchar log[ 1024 ];
			glGetShaderInfoLog( vertexShader, sizeof( log ), NULL, log );
			glDeleteShader( vertexShader );
			glDeleteShader( fragmentShader );
			std ::cerr << " Error compiling vertex shader : " << log << std ::endl;
			return false;
		}

		program = glCreateProgram();
		glAttachShader( program, fragmentShader );
		glAttachShader( program, vertexShader );
		glLinkProgram( program );

		// Check if link is ok.
		GLint linked;
		glGetProgramiv( program, GL_LINK_STATUS, &linked );
		if ( !linked )
		{
			GLchar log[ 1024 ];
			glGetProgramInfoLog( program, sizeof( log ), NULL, log );
			std ::cerr << " Error linking program : " << log << std ::endl;
			return false;
		}

		glDeleteShader( fragmentShader );
		glDeleteShader( vertexShader );

		disque( c, n, r );

		// Création sur GPU	
		glCreateVertexArrays( 1, &vao );
		glCreateBuffers( 1, &vbo );
		glCreateBuffers( 1, &vboCouleur );
		glCreateBuffers( 1, &ebo );

		// Remplissage du buffer
		glNamedBufferData( vbo, sommetsTriangle.size() * sizeof( Vec2f ), sommetsTriangle.data(), GL_STATIC_DRAW );
		glNamedBufferData( ebo, vectorRect.size() * sizeof( int ), vectorRect.data(), GL_STATIC_DRAW );
		glNamedBufferData( vboCouleur, couleurs.size() * sizeof( Vec3f ), couleurs.data(), GL_STATIC_DRAW );
		
		// Activer les attributs du VAO
		glEnableVertexArrayAttrib( vao, 0 );
		glEnableVertexArrayAttrib( vao, 1 );

		// Définir le format de l'attribut
		glVertexArrayAttribFormat( vao, 0, 2, GL_FLOAT, GL_FALSE, 0 );
		glVertexArrayAttribFormat( vao, 1, 3, GL_FLOAT, GL_FALSE, 0 );

		// Spécifier le VBO à lire
		glVertexArrayVertexBuffer( vao, 0, vbo, 0, sizeof( float ) * 2 );
		glVertexArrayVertexBuffer( vao, 1, vboCouleur, 0, sizeof( float ) * 3 );

		// Connecter le VAO avec le Vertex Shader
		glVertexArrayAttribBinding( vao, 0, 0 );
		glVertexArrayAttribBinding( vao, 1, 1 );

		// Liaison EBO VAO
		glVertexArrayElementBuffer( vao, ebo );

		glUseProgram( program );
		
		// Dans la fonction init, recuperez l'adresse de cette variable via la fonction glGetUniformLocation et stockez-la comme attribut de la classe LabWork2.
		loc = glGetUniformLocation( program, "uTranslationX" );
		locLum = glGetUniformLocation( program, "luminosite" );

		// donner une valeur arbitraire a cette variable
		glProgramUniform1f( program, loc, 0.0f );

		// Initialisation luminosite et couleur
		glProgramUniform1f( program, locLum, luminosite );
		glClearColor( _bgColor.x, _bgColor.y, _bgColor.z, _bgColor.w );

		std::cout << "Done!" << std::endl;
		return true;
	}

	void LabWork2::animate( const float p_deltaTime ) 
	{ 			
		// donner une valeur arbitraire a cette variable
		glProgramUniform1f( program, loc, glm::sin( _time ) / 2 );
		if (modificationLuminosite)
			glProgramUniform1f( program, locLum, luminosite );
		if ( modificationFond )
			glClearColor( _bgColor.x, _bgColor.y, _bgColor.z, _bgColor.w );
		_time += p_deltaTime;
	}

	void LabWork2::render()
	{
		glBindVertexArray( vao );
		glClear( GL_COLOR_BUFFER_BIT );
		glDrawElements( GL_TRIANGLES, vectorRect.size(), GL_UNSIGNED_INT, 0 );
		glBindVertexArray( 0 );
	}

	void LabWork2::handleEvents( const SDL_Event & p_event ) {}

	void LabWork2::displayUI()
	{
		ImGui::Begin( "Settings lab work 1" );
		ImGui::Text( "No setting available!" );
		modificationLuminosite = ImGui::SliderFloat( "Slider", &luminosite, 0.0f, 1.0f );			
		modificationFond = ImGui::ColorEdit3( "Couleur Fond", glm::value_ptr( _bgColor ) );
		ImGui::End();
	}

	void LabWork2::disque( const Vec2f c, const int n, const float r )
	{
		float  x = c.x + r, y = c.y;
		double angle = glm::radians( 360.0 / n );

		for ( int i = 0; i < n; i++ )
		{
			sommetsTriangle.push_back( Vec2f( x, y ) );
			couleurs.push_back( getRandomVec3f() );

			vectorRect.push_back( n );
			vectorRect.push_back( i );
			vectorRect.push_back( ( i + 1 ) % n );

			x = r * ( cos( angle * i ) ) + c.x;
			y = r * ( sin( angle * i ) ) + c.y;
		}
		sommetsTriangle.push_back( c );
		couleurs.push_back( getRandomVec3f() );
	}

} // namespace M3D_ISICG
