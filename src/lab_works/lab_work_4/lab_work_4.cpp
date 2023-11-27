#include "lab_work_4.hpp"
#include "glm/gtc/type_ptr.hpp"
#include "imgui.h"
#include "utils/random.hpp"
#include "utils/read_file.hpp"
#include <iostream>

namespace M3D_ISICG
{
	const std::string LabWork4::_shaderFolder = "src/lab_works/lab_work_4/shaders/";

	LabWork4::~LabWork4()
	{
		glDeleteProgram( program );

		bunny.cleanGL();
	}

	bool LabWork4::init()
	{
		std::cout << "Initializing lab work 4..." << std::endl;

		// Set the color used by glClear to clear the color buffer (in render()).
		glClearColor( _bgColor.x, _bgColor.y, _bgColor.z, _bgColor.w );

		const std::string fragmentShaderStr = readFile( _shaderFolder + "mesh.frag" );
		const std::string vertexShaderStr	= readFile( _shaderFolder + "mesh.vert" );

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

		bunny.load( "Bunny", "data/models/bunny.obj" );

		glUseProgram( program );

		// Dans la fonction init, recuperez l'adresse de cette variable via la fonction glGetUniformLocation et
		// stockez-la comme attribut de la classe LabWork4.
		locLum = glGetUniformLocation( program, "luminosite" );
		// Matrice model view projection
		MVP = glGetUniformLocation( program, "uMVPMatrix" );
		normalMatrix = glGetUniformLocation( program, "normalMatrix" );

		// Initialisation luminosite et couleur
		glProgramUniform1f( program, locLum, luminosite );
		glClearColor( _bgColor.x, _bgColor.y, _bgColor.z, _bgColor.w );

		_initCamera();

		std::cout << "Done!" << std::endl;
		return true;
	}

	void LabWork4::animate( const float p_deltaTime ) 
	{
		_updateViewMatrix();
		_updateProjMatrix();
		MVPMatrix = _camera.getProjectionMatrix() * _camera.getViewMatrix();
		glProgramUniformMatrix4fv( program, MVP, 1, 0, glm::value_ptr( MVPMatrix ) );
	}

	void LabWork4::render()
	{ 
		Mat4f normalMat = glm::transpose( glm::inverse( _camera.getViewMatrix() * MAT4F_ID ) );
		glProgramUniformMatrix4fv( program, MVP, 1, 0, glm::value_ptr( MVPMatrix ) );
		glProgramUniformMatrix4fv( program, normalMatrix, 1, 0, glm::value_ptr( normalMat ) );
		bunny.render( program );
	}

	void LabWork4::handleEvents( const SDL_Event & p_event )
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

	void LabWork4::displayUI()
	{
		ImGui::Begin( "Settings lab work 1" );
		ImGui::Text( "No setting available!" );
		if ( ImGui::SliderFloat( "Couleur Cube", &luminosite, 0.0f, 1.0f ) )
			glProgramUniform1f( program, locLum, luminosite );
		if ( ImGui::ColorEdit3( "Couleur Fond", glm::value_ptr( _bgColor ) ) )
			glClearColor( _bgColor.x, _bgColor.y, _bgColor.z, _bgColor.w );
		if ( ImGui::SliderFloat( "fovy", &fovy, 60.0f, 120.0f ) )
			_camera.setFovy( fovy );
		ImGui::End();
	}

	void LabWork4::_updateViewMatrix()
	{
		glProgramUniformMatrix4fv( program, viewMatrix, 1, 0, glm::value_ptr( _camera.getViewMatrix() ) );
	}

	void LabWork4::_updateProjMatrix()
	{
		glProgramUniformMatrix4fv( program, projMatrix, 1, 0, glm::value_ptr( _camera.getProjectionMatrix() ) );
	}

	void LabWork4::_initCamera() 
	{ 
		_camera.setPosition( Vec3f( 0.0f, 1.0f, 3.0f ) );
		_camera.setScreenSize( BaseLabWork::getWindowWidth(), BaseLabWork::getWindowHeight() );
		_camera.setFovy( fovy );
	}
} // namespace M3D_ISICG
