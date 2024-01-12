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
		glDeleteProgram( _shadingPassProgram );

		Sponza.cleanGL();
	}

	bool LabWork6::init()
	{
		std::cout << "Initializing lab work 6..." << std::endl;

		// Set the color used by glClear to clear the color buffer (in render()).
		glClearColor( _bgColor.x, _bgColor.y, _bgColor.z, _bgColor.w );

		const std::string fragmentShaderStr = readFile( _shaderFolder + "geometry_pass.frag" );
		const std::string vertexShaderStr	= readFile( _shaderFolder + "geometry_pass.vert" );
		const std::string FragmenshadingPassStr = readFile( _shaderFolder + "shading_pass.frag" );

		const GLuint fragmentShader = glCreateShader( GL_FRAGMENT_SHADER );
		const GLuint vertexShader	= glCreateShader( GL_VERTEX_SHADER );
		const GLuint shadingPassShader	= glCreateShader( GL_FRAGMENT_SHADER );

		const GLchar * vSrcFrag = fragmentShaderStr.c_str();
		const GLchar * vSrcVert = vertexShaderStr.c_str();
		const GLchar * vSrcFragShadingPass = FragmenshadingPassStr.c_str();

		glShaderSource( fragmentShader, 1, &vSrcFrag, NULL );
		glShaderSource( vertexShader, 1, &vSrcVert, NULL );
		glShaderSource( shadingPassShader, 1, &vSrcFragShadingPass, NULL );

		glCompileShader( fragmentShader );
		glCompileShader( vertexShader );
		glCompileShader( shadingPassShader );

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

		// program
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
			std ::cerr << " Error linking " << program << " : " << log << std ::endl;
			return false;
		}
		return true; 

		// _geometryPassProgram
		_geometryPassProgram = glCreateProgram();
		glAttachShader( _geometryPassProgram, fragmentShader );
		glAttachShader( _geometryPassProgram, vertexShader );
		glLinkProgram( _geometryPassProgram );

		// Check if link is ok.
		GLint linkedGPP;
		glGetProgramiv( _geometryPassProgram, GL_LINK_STATUS, &linkedGPP );
		if ( !linkedGPP )
		{
			GLchar log[ 1024 ];
			glGetProgramInfoLog( _geometryPassProgram, sizeof( log ), NULL, log );
			std ::cerr << " Error linking " << _geometryPassProgram << " : " << log << std ::endl;
			return false;
		}
		return true;

		// _shadingPassProgram
		_shadingPassProgram = glCreateProgram();
		glAttachShader( _shadingPassProgram, shadingPassShader );
		glLinkProgram( _shadingPassProgram );

		// Check if link is ok.
		GLint linkedSPP;
		glGetProgramiv( _shadingPassProgram, GL_LINK_STATUS, &linkedSPP );
		if ( !linkedSPP )
		{
			GLchar log[ 1024 ];
			glGetProgramInfoLog( _shadingPassProgram, sizeof( log ), NULL, log );
			std ::cerr << " Error linking " << _shadingPassProgram << " : " << log << std ::endl;
			return false;
		}
		return true;

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
		// position de la lumière
		lightPos = glGetUniformLocation( program, "lightPos" );
		// Initialisation luminosite et couleur
		glProgramUniform1f( program, locLum, luminosite );
		glClearColor( _bgColor.x, _bgColor.y, _bgColor.z, _bgColor.w );
		// Initialisation Matrice modèle
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
		glProgramUniform3fv( program, lightPos, 1, glm::value_ptr( VEC3F_ZERO ) );

		_geometryPass();
		glClear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT );
		Sponza.render( program );
		_shadingPass();
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

	// TP6 : frameBuffer
	void LabWork6::initGBuffer()
	{
		glCreateFramebuffers( 1, &fboId );

		GLenum drawBuffers[] = { GL_COLOR_ATTACHMENT0, GL_COLOR_ATTACHMENT1, GL_COLOR_ATTACHMENT2,
								 GL_COLOR_ATTACHMENT3, GL_COLOR_ATTACHMENT4, GL_DEPTH_ATTACHMENT };		

		for (int i = 0; i < 6; i++) 
		{

			// Create a texture on the GPU.
			glCreateTextures( GL_TEXTURE_2D, 6, &_gBufferTextures[ i ] );

			// Niveau de mipmap
			GLint mipmapLevels = std::floor( std::log2( std::max( BaseLabWork::getWindowWidth(), BaseLabWork::getWindowHeight() ) ) );

			// Setup the texture format.
			if (i == 5) // Profondeur
			{
				glTextureStorage2D( _gBufferTextures[ i ], mipmapLevels, GL_DEPTH_COMPONENT32F, BaseLabWork::getWindowWidth(), BaseLabWork::getWindowHeight() );
			}
			else
			{
				glTextureStorage2D( _gBufferTextures[ i ], mipmapLevels, GL_RGBA32F, BaseLabWork::getWindowWidth(), BaseLabWork::getWindowHeight() );
			}
			
			glTextureParameteri( _gBufferTextures[ i ], GL_TEXTURE_WRAP_S, GL_REPEAT );
			glTextureParameteri( _gBufferTextures[ i ], GL_TEXTURE_WRAP_T, GL_REPEAT );
			glTextureParameteri( _gBufferTextures[ i ], GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR );
			glTextureParameteri( _gBufferTextures[ i ], GL_TEXTURE_MAG_FILTER, GL_LINEAR );

			// Fill the texture.
			glTextureSubImage2D( _gBufferTextures[ i ], 0, 0, 0, BaseLabWork::getWindowWidth(), BaseLabWork::getWindowHeight(), GL_RGBA, GL_UNSIGNED_BYTE, nullptr );
			glGenerateTextureMipmap( _gBufferTextures[ i ] );

			// Liaison texture FBO
			glNamedFramebufferTexture( fboId, drawBuffers[ i ], _gBufferTextures[ i ], mipmapLevels );
		}

		glNamedFramebufferDrawBuffers( fboId, 6, drawBuffers );
		glCheckNamedFramebufferStatus;

		// Copie d'1 texture
		glNamedFramebufferReadBuffer( fboId, drawBuffers[ 0 ] );
		glBlitNamedFramebuffer( fboId, 0, 0, 0, BaseLabWork::getWindowWidth(), BaseLabWork::getWindowHeight(), 0, 0, BaseLabWork::getWindowWidth(), BaseLabWork::getWindowHeight(), GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT, GL_NEAREST);
	}

	void LabWork6::_geometryPass()
	{
		//  On indique que le FS écrira dans le FBO
		glBindFramebuffer( GL_DRAW_FRAMEBUFFER, fboId );
		glBindVertexArray( vao );
		glEnable( GL_DEPTH_TEST );

		// texture posFrag
		glBindTextureUnit( 0, _gBufferTextures[ 0 ] );
		// texture normales
		glBindTextureUnit( 1, _gBufferTextures[ 1 ] );
		// texture ambiante
		glBindTextureUnit( 2, _gBufferTextures[ 2 ] );
		// texture diffuse
		glBindTextureUnit( 3, _gBufferTextures[ 3 ] );
		// texture spéculaire
		glBindTextureUnit( 4, _gBufferTextures[ 4 ] );
		// texture depth
		glBindTextureUnit( 5, _gBufferTextures[ 5 ] );

		glBindVertexArray( 0 );
		glBindTextureUnit( 0, 0 );
		glBindTextureUnit( 1, 0 );
		glBindTextureUnit( 2, 0 );
		glBindTextureUnit( 3, 0 );
		glBindTextureUnit( 4, 0 );
	}

	void LabWork6::_shadingPass()
	{
		//  On indique que le FS écrira dans le FB par défaut
		glBindFramebuffer( GL_DRAW_FRAMEBUFFER, 0 );
		glBindVertexArray( vao );
		glDisable( GL_DEPTH_TEST );

		// texture posFrag
		glBindTextureUnit( 6, _gBufferTextures[ 0 ] );
		// texture normales
		glBindTextureUnit( 7, _gBufferTextures[ 1 ] );
		// texture ambiante
		glBindTextureUnit( 8, _gBufferTextures[ 2 ] );
		// texture diffuse
		glBindTextureUnit( 9, _gBufferTextures[ 3 ] );
		// texture spéculaire
		glBindTextureUnit( 10, _gBufferTextures[ 4 ] );
		// texture depth
		glBindTextureUnit( 11, _gBufferTextures[ 5 ] );

		// Reprise du TP2
		quad();

		// Création sur GPU
		glCreateVertexArrays( 1, &vao );
		glCreateBuffers( 1, &vbo );
		glCreateBuffers( 1, &ebo );

		// Remplissage du buffer
		glNamedBufferData( vbo, sommets.size() * sizeof( Vec2f ), sommets.data(), GL_STATIC_DRAW );
		glNamedBufferData( ebo, indices.size() * sizeof( int ), indices.data(), GL_STATIC_DRAW );

		// Activer les attributs du VAO
		glEnableVertexArrayAttrib( vao, 0 );
		glEnableVertexArrayAttrib( vao, 1 );

		// Définir le format de l'attribut
		glVertexArrayAttribFormat( vao, 0, 2, GL_FLOAT, GL_FALSE, 0 );
		glVertexArrayAttribFormat( vao, 1, 3, GL_FLOAT, GL_FALSE, 0 );

		// Spécifier le VBO à lire
		glVertexArrayVertexBuffer( vao, 0, vbo, 0, sizeof( float ) * 2 );

		// Connecter le VAO avec le Vertex Shader
		glVertexArrayAttribBinding( vao, 0, 0 );
		glVertexArrayAttribBinding( vao, 1, 1 );

		// Liaison EBO VAO
		glVertexArrayElementBuffer( vao, ebo );

		glDrawElements( GL_TRIANGLES, indices.size(), GL_UNSIGNED_INT, 0 );
		glBindVertexArray( 0 );
		glBindTextureUnit( 0, 0 );
		glBindTextureUnit( 1, 0 );
		glBindTextureUnit( 2, 0 );
		glBindTextureUnit( 3, 0 );
		glBindTextureUnit( 4, 0 );
	}

	void LabWork6::quad()
	{
		// Les sommets du quad
		sommets = { { -1.f, 1.f }, { 1.f, 1.f }, { 1.f, -1.f }, { -1.f, -1.f } };

		// Les indices des sommets pour former les triangles
		indices = { 0, 1, 2, 2, 3, 0 };
	}
} // namespace M3D_ISICG
