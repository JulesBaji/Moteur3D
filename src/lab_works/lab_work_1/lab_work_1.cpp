#include "lab_work_1.hpp"
#include "imgui.h"
#include "utils/read_file.hpp"
#include <iostream>

namespace M3D_ISICG
{
	const std::string LabWork1::_shaderFolder = "src/lab_works/lab_work_1/shaders/";
	GLuint	   program;
	std::vector<Vec2f> sommetsTriangle;
	GLuint			   VBOIdentifiant;
	GLuint			   VAOIdentifiant;

	LabWork1::~LabWork1() 
	{ 
		glDeleteProgram( program );
		glDeleteBuffers( 1, &VBOIdentifiant );
		glDisableVertexArrayAttrib( VAOIdentifiant, 0 );
		glDeleteVertexArrays( 1, &VAOIdentifiant );
	}

	bool LabWork1::init()
	{
		std::cout << "Initializing lab work 1..." << std::endl;
		// Set the color used by glClear to clear the color buffer (in render()).
		glClearColor( _bgColor.x, _bgColor.y, _bgColor.z, _bgColor.w );

		const std::string fragmentShaderStr = readFile( _shaderFolder + "lw1.frag" );
		const std::string vertexShaderStr	= readFile( _shaderFolder + "lw1.vert" );

		const GLuint	  fragmentShader	  = glCreateShader( GL_FRAGMENT_SHADER );
		const GLuint	  vertexShader		  = glCreateShader( GL_VERTEX_SHADER );

		const GLchar *	  vSrcFrag			  = fragmentShaderStr.c_str();
		const GLchar *	  vSrcVert			  = vertexShaderStr.c_str();

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

		sommetsTriangle.push_back( Vec2f( -0.5f, 0.5f ) );
		sommetsTriangle.push_back( Vec2f( 0.5f, 0.5f ) );
		sommetsTriangle.push_back( Vec2f( 0.5f, -0.5f ) );

		glCreateBuffers( 1, &VBOIdentifiant );
		glNamedBufferData( VBOIdentifiant, sommetsTriangle.size() * sizeof( Vec2f ), sommetsTriangle.data(), GL_STATIC_DRAW );

		glCreateVertexArrays( 1, &VAOIdentifiant );
		glEnableVertexArrayAttrib( VAOIdentifiant, 0 );
		glVertexArrayAttribFormat( VAOIdentifiant, 0, 2, GL_FLOAT, GL_FALSE, 0 );

		glVertexArrayVertexBuffer( VAOIdentifiant, 0, VBOIdentifiant, 0, sizeof( float ) * 2 );
		glVertexArrayAttribBinding( VAOIdentifiant, 0, 0 );
		glUseProgram( program );

		std::cout << "Done!" << std::endl;
		return true;
	}

	void LabWork1::animate( const float p_deltaTime ) {}

	void LabWork1::render() 
	{ 
		glClear( GL_COLOR_BUFFER_BIT );	
		glBindVertexArray( VAOIdentifiant );
		glDrawArrays( GL_TRIANGLES, 0, 3 );
		glBindVertexArray( 0 );
	}

	void LabWork1::handleEvents( const SDL_Event & p_event )
	{}

	void LabWork1::displayUI()
	{
		ImGui::Begin( "Settings lab work 1" );
		ImGui::Text( "No setting available!" );
		ImGui::End();
	}

} // namespace M3D_ISICG
