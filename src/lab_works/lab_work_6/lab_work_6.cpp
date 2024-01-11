#include "lab_work_6.hpp"
#include "glm/gtc/type_ptr.hpp"
#include "imgui.h"
#include "utils/random.hpp"
#include "utils/read_file.hpp"
#include <iostream>

namespace M3D_ISICG
{
	const std::string LabWork6::_shaderFolder = "src/lab_works/lab_work_6/shaders/";

	LabWork6::~LabWork6()
	{
		glDeleteProgram( program );
		glDeleteProgram( _geometryPassProgram );

		Sponza.cleanGL();
	}

	bool LabWork6::init()
	{
		std::cout << "Initializing lab work 6..." << std::endl;

		// Set the color used by glClear to clear the color buffer (in render()).
		glClearColor( _bgColor.x, _bgColor.y, _bgColor.z, _bgColor.w );

		const std::string fragmentShaderStr = readFile( _shaderFolder + "geometry_pass.frag" );
		const std::string vertexShaderStr	= readFile( _shaderFolder + "geometry_pass.vert" );

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

		Sponza.load( "Sponza", "data/models/sponza/sponza.obj" );

		glUseProgram( program );

		// Dans la fonction init, recuperez l'adresse de cette variable via la fonction glGetUniformLocation et
		// stockez-la comme attribut de la classe LabWork6.
		locLum = glGetUniformLocation( program, "luminosite" );
		// Matrice model view projection
		MVP = glGetUniformLocation( program, "uMVPMatrix" );
		// Matrice model view
		MV = glGetUniformLocation( program, "uMVMatrix" );
		// Matrice normale
		normalMatrix = glGetUniformLocation( program, "uNormalMatrix" );
		// position de la lumi�re
		lightPos = glGetUniformLocation( program, "lightPos" );
		// Initialisation luminosite et couleur
		glProgramUniform1f( program, locLum, luminosite );
		glClearColor( _bgColor.x, _bgColor.y, _bgColor.z, _bgColor.w );
		// Initialisation Matrice mod�le
		mMatrix = glm::scale( MAT4F_ID, glm::vec3( 0.01f ) );

		_initCamera();

		std::cout << "Done!" << std::endl;
		return true;
	}

	void LabWork6::animate( const float p_deltaTime ) 
	{
	}

	void LabWork6::render()
	{ 
		glClear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT );
		_updateViewMatrix();
		_updateProjMatrix();
		MVPMatrix		= _camera.getProjectionMatrix() * _camera.getViewMatrix() * mMatrix;
		MVMatrix		= _camera.getViewMatrix() * mMatrix;
		Mat4f normalMat = glm::transpose( glm::inverse( MVMatrix ) );
		glProgramUniformMatrix4fv( program, MVP, 1, 0, glm::value_ptr( MVPMatrix ) );
		glProgramUniformMatrix4fv( program, MV, 1, 0, glm::value_ptr( MVMatrix ) );
		glProgramUniformMatrix4fv( program, normalMatrix, 1, 0, glm::value_ptr( normalMat ) );
		glProgramUniform3fv( program, lightPos, 1, glm::value_ptr(VEC3F_ZERO) );
		Sponza.render( program );
	}

	void LabWork6::handleEvents( const SDL_Event & p_event )
	{
		if ( p_event.type == SDL_KEYDOWN )
		{
			switch ( p_event.key.keysym.scancode )
			{
			case SDL_SCANCODE_W: // Front
				_camera.moveFront( _cameraSpeed );
				_updateViewMatrix();
				break;
			case SDL_SCANCODE_S: // Back
				_camera.moveFront( -_cameraSpeed );
				_updateViewMatrix();
				break;
			case SDL_SCANCODE_A: // Left
				_camera.moveRight( -_cameraSpeed );
				_updateViewMatrix();
				break;
			case SDL_SCANCODE_D: // Right
				_camera.moveRight( _cameraSpeed );
				_updateViewMatrix();
				break;
			case SDL_SCANCODE_R: // Up
				_camera.moveUp( _cameraSpeed );
				_updateViewMatrix();
				break;
			case SDL_SCANCODE_F: // Bottom
				_camera.moveUp( -_cameraSpeed );
				_updateViewMatrix();
				break;
			default: break;
			}
		}

		// Rotate when left click + motion (if not on Imgui widget).
		if ( p_event.type == SDL_MOUSEMOTION && p_event.motion.state & SDL_BUTTON_LMASK
			 && !ImGui::GetIO().WantCaptureMouse )
		{
			_camera.rotate( p_event.motion.xrel * _cameraSensitivity, p_event.motion.yrel * _cameraSensitivity );
			_updateViewMatrix();
		}
	}

	void LabWork6::displayUI()
	{
		ImGui::Begin( "Settings lab work 1" );
		ImGui::Text( "No setting available!" );
		if ( ImGui::SliderFloat( "Couleur Cube", &luminosite, 0.0f, 1.0f ) )
			glProgramUniform1f( program, locLum, luminosite );
		if ( ImGui::ColorEdit3( "Couleur Fond", glm::value_ptr( _bgColor ) ) )
			glClearColor( _bgColor.x, _bgColor.y, _bgColor.z, _bgColor.w );
		if ( ImGui::SliderFloat( "fovy", &fovy, 60.0f, 120.0f ) )
			_camera.setFovy( fovy );
		/*ImGui::BeginListBox( "test", ImVec2( 100, 100 ) );

		 for ( int i = 0; i < 6; i++ )
		 {

		 }

		ImGui::EndListBox();*/
		ImGui::End();		
	}

	void LabWork6::_updateViewMatrix()
	{
		glProgramUniformMatrix4fv( program, viewMatrix, 1, 0, glm::value_ptr( _camera.getViewMatrix() ) );
	}

	void LabWork6::_updateProjMatrix()
	{
		glProgramUniformMatrix4fv( program, projMatrix, 1, 0, glm::value_ptr( _camera.getProjectionMatrix() ) );
	}

	void LabWork6::_initCamera() 
	{ 
		_camera.setPosition( Vec3f( 0.0f, 0.0f, 0.2f ) );
		_camera.setScreenSize( BaseLabWork::getWindowWidth(), BaseLabWork::getWindowHeight() );
		_camera.setFovy( fovy );
	}

	bool LabWork6::initGeometryPass() 
	{
		std::cout << "Initializing lab work 6..." << std::endl;

		// Set the color used by glClear to clear the color buffer (in render()).
		glClearColor( _bgColor.x, _bgColor.y, _bgColor.z, _bgColor.w );

		const std::string fragmentShaderStr = readFile( _shaderFolder + "geometry_pass.frag" );
		const std::string vertexShaderStr	= readFile( _shaderFolder + "geometry_pass.vert" );

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

		// Initialisation
		_geometryPassProgram = glCreateProgram();
		glAttachShader( _geometryPassProgram, fragmentShader );
		glAttachShader( _geometryPassProgram, vertexShader );
		glLinkProgram( _geometryPassProgram );

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

		Sponza.load( "Sponza", "data/models/sponza/sponza.obj" );

		glUseProgram( _geometryPassProgram );

		std::cout << "Done!" << std::endl;
		return true;
	}

	bool LabWork6::initShadingPass()
	{
		std::cout << "Initializing lab work 6..." << std::endl;

		// Set the color used by glClear to clear the color buffer (in render()).
		glClearColor( _bgColor.x, _bgColor.y, _bgColor.z, _bgColor.w );

		const std::string fragmentShaderStr = readFile( _shaderFolder + "geometry_pass.frag" );
		const std::string vertexShaderStr	= readFile( _shaderFolder + "geometry_pass.vert" );

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

		// Initialisation
		_shadingPassProgram = glCreateProgram();
		glAttachShader( _shadingPassProgram, fragmentShader );
		glAttachShader( _shadingPassProgram, vertexShader );
		glLinkProgram( _shadingPassProgram );

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

		//Sponza.load( "Sponza", "data/models/sponza/sponza.obj" );

		glUseProgram( _shadingPassProgram );

		// Cr�er un VBO pour les coordonn�es du quad
		const GLfloat quadVertices[] = {
			-1.0f, 1.0f,  0.0f, // En haut � gauche
			-1.0f, -1.0f, 0.0f, // En bas � gauche
			1.0f,  -1.0f, 0.0f, // En bas � droite
			1.0f,  1.0f,  0.0f	// En haut � droite
		};

		GLuint vboQuad;
		glGenBuffers( 1, &vboQuad );
		glBindBuffer( GL_ARRAY_BUFFER, vboQuad );
		glBufferData( GL_ARRAY_BUFFER, sizeof( quadVertices ), quadVertices, GL_STATIC_DRAW );

		// Cr�er un IBO pour les indices du quad
		const GLuint quadIndices[] = {
			0, 1, 2, // Premier triangle
			0, 2, 3	 // Deuxi�me triangle
		};

		GLuint eboQuad;
		glGenBuffers( 1, &eboQuad );
		glBindBuffer( GL_ELEMENT_ARRAY_BUFFER, eboQuad );
		glBufferData( GL_ELEMENT_ARRAY_BUFFER, sizeof( quadIndices ), quadIndices, GL_STATIC_DRAW );

		// Cr�er un VAO pour le quad
		GLuint vaoQuad;
		glGenVertexArrays( 1, &vaoQuad );
		glBindVertexArray( vaoQuad );

		// Sp�cifier le format de l'attribut pour les coordonn�es
		glVertexAttribPointer( 0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof( GLfloat ), (GLvoid *)0 );
		glEnableVertexAttribArray( 0 );

		// Liaison de l'EBO au VAO
		glBindBuffer( GL_ELEMENT_ARRAY_BUFFER, eboQuad );

		// Revenir � l'�tat par d�faut
		glBindVertexArray( 0 );

		std::cout << "Done!" << std::endl;
		return true;
	}
} // namespace M3D_ISICG
