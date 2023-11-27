#ifndef __LAB_WORK_3_HPP__
#define __LAB_WORK_3_HPP__

#include "GL/gl3w.h"
#include "common/base_lab_work.hpp"
#include "common/camera.hpp"
#include "define.hpp"
#include <vector>

namespace M3D_ISICG
{
	struct Mesh
	{
		std::vector<Vec3f>		  posSommets, couleursSommets;
		std::vector<unsigned int> indcSommets;
		Mat4f					  Transformation = MAT4F_ID;
		GLuint					  vboMesh = GL_INVALID_INDEX;
		GLuint					  vboMeshCouleurs = GL_INVALID_INDEX;
		GLuint					  vaoMesh = GL_INVALID_INDEX;
		GLuint					  eboMesh = GL_INVALID_INDEX;
	};

	class LabWork3 : public BaseLabWork
	{
	  public:
		LabWork3() : BaseLabWork() {}
		~LabWork3();

		bool init() override;
		void animate( const float p_deltaTime ) override;
		void render() override;

		void handleEvents( const SDL_Event & p_event ) override;
		void displayUI() override;
		void _updateViewMatrix();
		void _updateProjMatrix();
		void _initCamera();

	  private:
		void _createCube( Mesh & _cube );
		// ================ Scene data.
		Mesh			   _cube;
		// ================

		// ================ GL data.
		GLuint			   program = GL_INVALID_INDEX;
		GLint			   locLum  = GL_INVALID_INDEX;	
		GLint			   MVP = GL_INVALID_INDEX;
		GLint			   modelMatrix	  = GL_INVALID_INDEX;
		GLint			   viewMatrix = GL_INVALID_INDEX;
		GLint			   projMatrix	  = GL_INVALID_INDEX;
		// ================
		
		// ================ Settings.
		Vec4f _bgColor = Vec4f( 0.8f, 0.8f, 0.8f, 1.f ); // Background color
		float luminosite = 1.0f;
		Camera _camera;
		float  fovy = 60.f;
		// Dans le .hpp
		float _cameraSpeed		 = 0.1f;
		float _cameraSensitivity = 0.1f;
		Mat4f MVPMatrix			 = MAT4F_ID;
		// ================
		static const std::string _shaderFolder;
	};
} // namespace M3D_ISICG

#endif // __LAB_WORK_3_HPP__
