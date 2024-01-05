#include "lab_work_3.hpp"
#include "glm/gtc/type_ptr.hpp"
#include "imgui.h"
#include "utils/random.hpp"
#include "utils/read_file.hpp"
#include <iostream>

namespace M3D_ISICG
{
	const std::string LabWork3::_shaderFolder = "src/lab_works/lab_work_3/shaders/";

	LabWork3::~LabWork3()
	{
		glDeleteProgram( program );

		// Destruction buffer
		glDeleteBuffers( 1, &_cube.vboMesh );
		glDeleteBuffers( 1, &_cube.vboMeshCouleurs );
		glDeleteBuffers( 1, &_cube.eboMesh );

		// Destruction connexion _cube.vaoMesh avec Vertex Shader
		glDisableVertexArrayAttrib( _cube.vaoMesh, 0 );
		glDisableVertexArrayAttrib( _cube.vaoMesh, 1 );

		// Destruction _cube.vaoMesh
		glDeleteVertexArrays( 1, &_cube.vaoMesh );
	}

	bool LabWork3::init()
	{
		std::cout << "Initializing lab work 3..." << std::endl;

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

		// La fonction la
		_createCube( _cube );

		// Création sur GPU
		glCreateVertexArrays( 1, &_cube.vaoMesh );
		glCreateBuffers( 1, &_cube.vboMesh );
		glCreateBuffers( 1, &_cube.vboMeshCouleurs );
		glCreateBuffers( 1, &_cube.eboMesh );

		// Remplissage du buffer
		glNamedBufferData( _cube.vboMesh, _cube.posSommets.size() * sizeof( Vec3f ), _cube.posSommets.data(), GL_STATIC_DRAW );
		glNamedBufferData( _cube.vboMeshCouleurs,
						   _cube.couleursSommets.size() * sizeof( Vec3f ),
						   _cube.couleursSommets.data(),
						   GL_STATIC_DRAW );
		glNamedBufferData( _cube.eboMesh,
						   _cube.indcSommets.size() * sizeof( unsigned int ),
						   _cube.indcSommets.data(),
						   GL_STATIC_DRAW );

		// Activer les attributs du _cube.vaoMesh
		glEnableVertexArrayAttrib( _cube.vaoMesh, 0 );
		glEnableVertexArrayAttrib( _cube.vaoMesh, 1 );

		// Définir le format de l'attribut
		glVertexArrayAttribFormat( _cube.vaoMesh, 0, 3, GL_FLOAT, GL_FALSE, 0 );
		glVertexArrayAttribFormat( _cube.vaoMesh, 1, 3, GL_FLOAT, GL_FALSE, 0 );

		// Spécifier le _cube.vboMesh à lire
		glVertexArrayVertexBuffer( _cube.vaoMesh, 0, _cube.vboMesh, 0, sizeof( Vec3f ) );
		glVertexArrayVertexBuffer( _cube.vaoMesh, 1, _cube.vboMeshCouleurs, 0, sizeof( Vec3f ) );

		// Connecter le _cube.vaoMesh avec le Vertex Shader
		glVertexArrayAttribBinding( _cube.vaoMesh, 0, 0 );
		glVertexArrayAttribBinding( _cube.vaoMesh, 1, 1 );

		// Liaison _cube.eboMesh _cube.vaoMesh
		glVertexArrayElementBuffer( _cube.vaoMesh, _cube.eboMesh );

		glUseProgram( program );

		// Dans la fonction init, recuperez l'adresse de cette variable via la fonction glGetUniformLocation et
		// stockez-la comme attribut de la classe LabWork3.
		locLum = glGetUniformLocation( program, "luminosite" );
		// Matrice model view projection
		MVP = glGetUniformLocation( program, "MVP" );

		// Initialisation luminosite et couleur
		glProgramUniform1f( program, locLum, luminosite );
		glClearColor( _bgColor.x, _bgColor.y, _bgColor.z, _bgColor.w );
		// Init matrice transformation
		glProgramUniformMatrix4fv( program, modelMatrix, 1, 0, glm::value_ptr( _cube.Transformation ) );

		_initCamera();

		std::cout << "Done!" << std::endl;
		return true;
	}

	void LabWork3::animate( const float p_deltaTime ) 
	{
		_updateViewMatrix();
		_updateProjMatrix();
		_cube.Transformation = glm::rotate( _cube.Transformation, p_deltaTime, glm::vec3( 0.f, 1.0f, 1.f ) );
		glProgramUniformMatrix4fv( program, modelMatrix, 1, 0, glm::value_ptr( _cube.Transformation ) );
		MVPMatrix = _camera.getProjectionMatrix() * _camera.getViewMatrix() * _cube.Transformation;
		glProgramUniformMatrix4fv( program, MVP, 1, 0, glm::value_ptr( MVPMatrix ) );
	}

	void LabWork3::render()
	{
		glBindVertexArray( _cube.vaoMesh );
		glEnable( GL_DEPTH_TEST );
		glClear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT ); 
		glDrawElements( GL_TRIANGLES, _cube.indcSommets.size(), GL_UNSIGNED_INT, 0 );
		glBindVertexArray( 0 );
	}

	void LabWork3::handleEvents( const SDL_Event & p_event )
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

	void LabWork3::displayUI()
	{
		ImGui::Begin( "Settings lab work 3" );
		ImGui::Text( "No setting available!" );
		if ( ImGui::SliderFloat( "Couleur Cube", &luminosite, 0.0f, 1.0f ) )
			glProgramUniform1f( program, locLum, luminosite );
		if ( ImGui::ColorEdit3( "Couleur Fond", glm::value_ptr( _bgColor ) ) )
			glClearColor( _bgColor.x, _bgColor.y, _bgColor.z, _bgColor.w );
		if ( ImGui::SliderFloat( "fovy", &fovy, 60.0f, 120.0f ) )
			_camera.setFovy( fovy );
		ImGui::End();
	}

	void LabWork3::_createCube( Mesh & _cube )
	{
		_cube.posSommets = {
			{ 0.5, 0.5, 0.5 },	{ 0.5, -0.5, 0.5 },	 { -0.5, 0.5, 0.5 },  { -0.5, -0.5, 0.5 },
			{ 0.5, 0.5, -0.5 }, { 0.5, -0.5, -0.5 }, { -0.5, 0.5, -0.5 }, { -0.5, -0.5, -0.5 },
		};

		for ( int i = 0; i < _cube.posSommets.size(); i++ )
		{
			_cube.couleursSommets.push_back( getRandomVec3f() );
		}

		_cube.indcSommets = { 
			0, 3, 2, 
			0, 3, 1, 
			4, 7, 5, 
			4, 7, 6,	
			0, 6, 4, 
			0, 6, 2, 
			1, 7, 5, 
			1, 7, 3, 
			0, 5, 1, 
			0, 5, 4, 
			2, 7, 3, 
			2, 7, 6 };

		
		_cube.Transformation = glm::scale( _cube.Transformation, glm::vec3( 0.8f ) );
	}

	void LabWork3::_updateViewMatrix()
	{
		glProgramUniformMatrix4fv( program, viewMatrix, 1, 0, glm::value_ptr( _camera.getViewMatrix() ) );
	}

	void LabWork3::_updateProjMatrix()
	{
		glProgramUniformMatrix4fv( program, projMatrix, 1, 0, glm::value_ptr( _camera.getProjectionMatrix() ) );
	}

	void LabWork3::_initCamera() 
	{ 
		_camera.setPosition( Vec3f( 0.0f, 1.0f, 3.0f ) );
		_camera.setScreenSize( BaseLabWork::getWindowWidth(), BaseLabWork::getWindowHeight() );
		_camera.setFovy( fovy );
	}
} // namespace M3D_ISICG
