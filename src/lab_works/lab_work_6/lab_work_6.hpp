#ifndef __LAB_WORK_6_HPP__
#define __LAB_WORK_6_HPP__

#include "GL/gl3w.h"
#include "common/base_lab_work.hpp"
#include "common/models/triangle_mesh_model.hpp"
#include "common/models/triangle_mesh.hpp"
#include "common/camera.hpp"
#include "define.hpp"
#include <vector>

namespace M3D_ISICG
{
	class LabWork6 : public BaseLabWork
	{
	  public:
		LabWork6() : BaseLabWork() {}
		~LabWork6();

		bool init() override;
		void animate( const float p_deltaTime ) override;
		void render() override;

		void handleEvents( const SDL_Event & p_event ) override;
		void displayUI() override;
		void _updateViewMatrix();
		void _updateProjMatrix();
		void _initCamera();

		// TP6
		void initGBuffer();
		void _geometryPass();
		void _shadingPass();
		void quad();

	  private:
		// ================ Scene data.
		std::vector<Vec2f> sommets;
		std::vector<int>   indices;
		// ================

		// ================ GL data.
		GLuint			   _geometryPassProgram	  = GL_INVALID_INDEX;
		GLuint			   _shadingPassProgram	  = GL_INVALID_INDEX;
		GLint			   locLum  = GL_INVALID_INDEX;	
		GLint			   MVP = GL_INVALID_INDEX;
		GLint			   MV					  = GL_INVALID_INDEX;
		GLint			   normalMatrix			  = GL_INVALID_INDEX;
		GLint			   viewMatrix			  = GL_INVALID_INDEX;
		GLint			   projMatrix			  = GL_INVALID_INDEX;
		GLint			   lightPos				  = GL_INVALID_INDEX;
		//TP6
		GLuint			   fboId = GL_INVALID_INDEX;
		GLuint			   vbo	 = GL_INVALID_INDEX;
		GLuint			   vao	 = GL_INVALID_INDEX;
		GLuint			   ebo	 = GL_INVALID_INDEX;
		GLenum			   textureChoisie = GL_INVALID_INDEX;
		// ================
		
		// ================ Settings.
		Vec4f _bgColor = Vec4f( 0.8f, 0.8f, 0.8f, 1.f ); // Background color
		float luminosite = 1.0f;
		Camera _camera;
		float  fovy = 60.f;
		GLuint _gBufferTextures[6];
		// Dans le .hpp
		float _cameraSpeed		 = 0.1f;
		float _cameraSensitivity = 0.1f;
		Mat4f MVPMatrix			 = MAT4F_ID;
		Mat4f			  mMatrix			 = MAT4F_ID;
		Mat4f			  MVMatrix			 = MAT4F_ID;
		TriangleMeshModel Sponza;
		Vec3f			  posLumiere = Vec3f( 1.5f / 0.003f, 1.5f / 0.003f, -0.3f / 0.003f );
		// ================
		static const std::string _shaderFolder;
	};
} // namespace M3D_ISICG

#endif // __LAB_WORK_6_HPP__
