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
			glDeleteShader( shadingPassShader );
			std ::cerr << " Error compiling vertex shader : " << log << std ::endl;
			return false;
		}

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
			std ::cerr << " Error linking _geometryPassProgram : " << log << std ::endl;
			return false;
		}

		// _shadingPassProgram
		_shadingPassProgram = glCreateProgram();
		glAttachShader( _shadingPassProgram, shadingPassShader );
		glAttachShader( _shadingPassProgram, vertexShader );
		glLinkProgram( _shadingPassProgram );

		// Check if link is ok.
		GLint linkedSPP;
		glGetProgramiv( _shadingPassProgram, GL_LINK_STATUS, &linkedSPP );
		if ( !linkedSPP )
		{
			GLchar log[ 1024 ];
			glGetProgramInfoLog( _shadingPassProgram, sizeof( log ), NULL, log );
			std ::cerr << " Error linking _shadingPassProgram : " << log << std ::endl;
			return false;
		}

		glDeleteShader( fragmentShader );
		glDeleteShader( vertexShader );
		glDeleteShader( shadingPassShader );

		Sponza.load( "Sponza", "data/models/sponza/sponza.obj" );

		glUseProgram( _geometryPassProgram );

		// Dans la fonction init, recuperez l'adresse de cette variable via la fonction glGetUniformLocation et
		// stockez-la comme attribut de la classe LabWork6.
		locLum = glGetUniformLocation( _geometryPassProgram, "luminosite" );
		// Matrice model view projection
		MVP = glGetUniformLocation( _geometryPassProgram, "uMVPMatrix" );
		// Matrice model view
		MV = glGetUniformLocation( _geometryPassProgram, "uMVMatrix" );
		// Matrice normale
		normalMatrix = glGetUniformLocation( _geometryPassProgram, "uNormalMatrix" );
		// position de la lumière
		lightPos = glGetUniformLocation( _geometryPassProgram, "lightPos" );
		// Initialisation luminosite et couleur
		glProgramUniform1f( _geometryPassProgram, locLum, luminosite );

		glClearColor( _bgColor.x, _bgColor.y, _bgColor.z, _bgColor.w );
		// Initialisation Matrice modèle
		mMatrix = glm::scale( MAT4F_ID, glm::vec3( 0.01f ) );

		_initCamera();
		initGBuffer();

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

		std::cout << "Done!" << std::endl;
		return true;
	}

	void LabWork6::animate( const float p_deltaTime ) 
	{
	}

	void LabWork6::render()
	{ 
		//glClear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT );

		_updateViewMatrix();
		_updateProjMatrix();
		MVPMatrix		= _camera.getProjectionMatrix() * _camera.getViewMatrix() * mMatrix;
		MVMatrix		= _camera.getViewMatrix() * mMatrix;
		Mat4f normalMat = glm::transpose( glm::inverse( MVMatrix ) );

		glProgramUniformMatrix4fv( _geometryPassProgram, MVP, 1, 0, glm::value_ptr( MVPMatrix ) );
		glProgramUniformMatrix4fv( _geometryPassProgram, MV, 1, 0, glm::value_ptr( MVMatrix ) );
		glProgramUniformMatrix4fv( _geometryPassProgram, normalMatrix, 1, 0, glm::value_ptr( normalMat ) );
		glProgramUniform3fv( _geometryPassProgram, lightPos, 1, glm::value_ptr( VEC3F_ZERO ) );

		_geometryPass();
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
		ImGui::SetWindowSize( ImVec2( 300, 250 ) );
		ImGui::Begin( "Settings lab work 6" );
		if ( ImGui::ColorEdit3( "Couleur Fond", glm::value_ptr( _bgColor ) ) )
			glClearColor( _bgColor.x, _bgColor.y, _bgColor.z, _bgColor.w );
		if ( ImGui::SliderFloat( "fovy", &fovy, 60.0f, 120.0f ) )
			_camera.setFovy( fovy );
		
		ImGui::Text("Choisir une texture:");

		const char * items[]
			= { "Positions",
				"Normales",
				"Lumiere ambiante",
				"Lumiere diffuse", 
				"Lumiere speculaire" };

		static int currentItem = 0;

		if (ImGui::ListBox("##TextureList", &currentItem, items, IM_ARRAYSIZE(items), 5))
		{
			// User selected a texture, perform the corresponding actions
			switch (currentItem)
			{
			case 0:
				textureChoisie =  GL_COLOR_ATTACHMENT0;
				break;
			case 1: 
				textureChoisie = GL_COLOR_ATTACHMENT1;
				break;
			case 2: 
				textureChoisie = GL_COLOR_ATTACHMENT2;
				break;
			case 3: 
				textureChoisie = GL_COLOR_ATTACHMENT3;
				break;
			case 4: 
				textureChoisie = GL_COLOR_ATTACHMENT4;
				break;
			}
		}
		ImGui::End();		
	}

	void LabWork6::_updateViewMatrix()
	{
		glProgramUniformMatrix4fv( _geometryPassProgram, viewMatrix, 1, 0, glm::value_ptr( _camera.getViewMatrix() ) );
	}

	void LabWork6::_updateProjMatrix()
	{
		glProgramUniformMatrix4fv( _geometryPassProgram, projMatrix, 1, 0, glm::value_ptr( _camera.getProjectionMatrix() ) );
	}

	void LabWork6::_initCamera() 
	{ 
		_camera.setPosition( Vec3f( 0.0f, 0.0f, 0.2f ) );
		_camera.setScreenSize( BaseLabWork::getWindowWidth(), BaseLabWork::getWindowHeight() );
		_camera.setFovy( fovy );
	}

	void LabWork6::initGBuffer()
	{
		glCreateFramebuffers( 1, &fboId );

		GLenum drawBuffers[] = { GL_COLOR_ATTACHMENT0, GL_COLOR_ATTACHMENT1, GL_COLOR_ATTACHMENT2,
								 GL_COLOR_ATTACHMENT3, GL_COLOR_ATTACHMENT4, GL_DEPTH_ATTACHMENT };	

		// Niveau de mipmap
		GLint mipmapLevels = std::floor( std::log2( std::max( getWindowWidth(), getWindowHeight() ) ) );

		for ( int i = 0; i < 6; i++ )
		{
			// Create a texture on the GPU.
			// A faire : Notez qu’il est possible de créer plusieurs textures en un appel
			glCreateTextures( GL_TEXTURE_2D, 1, &_gBufferTextures[ i ] );

			// Configuration de l'objet
			if ( i == 5 )
				glTextureStorage2D(
					_gBufferTextures[ i ], mipmapLevels, GL_DEPTH_COMPONENT32F, getWindowWidth(), getWindowHeight() );
			else
				glTextureStorage2D( 
					_gBufferTextures[ i ], mipmapLevels, GL_RGBA32F, getWindowWidth(), getWindowHeight() );
			
			// Liaison texture/FBO
			glNamedFramebufferTexture( fboId, drawBuffers[ i ], _gBufferTextures[ i ], 0 );
		}

		// Associer les textures à la sortie du fragment shader
		glNamedFramebufferDrawBuffers( fboId, 5, drawBuffers );
		// Contrôler la validité du FBO
		glCheckNamedFramebufferStatus( fboId, GL_FRAMEBUFFER );
	}

	void LabWork6::_geometryPass()
	{
		glEnable( GL_DEPTH_TEST );
		glUseProgram( _geometryPassProgram );
		//  On indique que le FS écrira dans le FBO
		glBindFramebuffer( GL_DRAW_FRAMEBUFFER, fboId );
		glClear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT );
		Sponza.render( _geometryPassProgram );
		glBindFramebuffer( GL_DRAW_FRAMEBUFFER, 0 );
		if (textureChoisie != GL_INVALID_INDEX)
		{
			glNamedFramebufferReadBuffer( fboId, textureChoisie );
		glBlitNamedFramebuffer(fboId, 0, 0, 0, getWindowWidth(), getWindowHeight(),
									0, 0, getWindowWidth(), getWindowHeight(), GL_COLOR_BUFFER_BIT, GL_NEAREST);
		}	
	}

	void LabWork6::_shadingPass()
	{
		// On commence par désactiver le test de profondeur
		glDisable( GL_DEPTH_TEST );
		glUseProgram( _shadingPassProgram );
		//  On indique que le FS écrira dans le FB par défaut
		glBindFramebuffer( GL_DRAW_FRAMEBUFFER, 0 );	

		// Passage des textures
		// texture positions fragments
		glBindTextureUnit( 5, _gBufferTextures[ 0 ] );
		// texture normales
		glBindTextureUnit( 6, _gBufferTextures[ 1 ] );
		// texture ambiante
		glBindTextureUnit( 7, _gBufferTextures[ 2 ] );
		// texture diffuse
		glBindTextureUnit( 8, _gBufferTextures[ 3 ] );
		// texture spéculaire
		glBindTextureUnit( 9, _gBufferTextures[ 4 ] );
		// texture profondeur
		glBindTextureUnit( 10, _gBufferTextures[ 5 ] );

		glBindVertexArray( vao );
		// Dessin
		glDrawElements( GL_TRIANGLES, indices.size(), GL_UNSIGNED_INT, 0 );

		// Nettoyage du buffer
		glBindVertexArray( 0 );
		glBindTextureUnit( 5, 0 );
		glBindTextureUnit( 6, 0 );
		glBindTextureUnit( 7, 0 );
		glBindTextureUnit( 8, 0 );
		glBindTextureUnit( 9, 0 );
		glBindTextureUnit( 10, 0 );
	}

	void LabWork6::quad()
	{
		// Les sommets du quad
		sommets = { { -1.f, 1.f }, { 1.f, 1.f }, { 1.f, -1.f }, { -1.f, -1.f } };

		// Les indices des sommets pour former les triangles
		indices = { 0, 1, 2, 2, 3, 0 };
	}
} // namespace M3D_ISICG
