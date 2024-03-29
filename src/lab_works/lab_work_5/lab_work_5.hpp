#ifndef __LAB_WORK_5_HPP__
#define __LAB_WORK_5_HPP__

#include "GL/gl3w.h"
#include "common/base_lab_work.hpp"
#include "common/models/triangle_mesh_model.hpp"
#include "common/models/triangle_mesh.hpp"
#include "common/camera.hpp"
#include "define.hpp"
#include <vector>

namespace M3D_ISICG
{
	class LabWork5 : public BaseLabWork
	{
	  public:
		LabWork5() : BaseLabWork() {}
		~LabWork5();

		bool init() override;
		void animate( const float p_deltaTime ) override;
		void render() override;

		void handleEvents( const SDL_Event & p_event ) override;
		void displayUI() override;
		void _initCamera();

	  private:
		// ================ Scene data.
		// ================

		// ================ GL data.
		GLuint			   program = GL_INVALID_INDEX;
		GLint			   locLum  = GL_INVALID_INDEX;	
		GLint			   MVP = GL_INVALID_INDEX;
		GLint			   MV					  = GL_INVALID_INDEX;
		GLint			   normalMatrix			  = GL_INVALID_INDEX;
		GLint			   lightPos				  = GL_INVALID_INDEX;
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
		Mat4f			  mMatrix			 = MAT4F_ID;
		Mat4f			  MVMatrix			 = MAT4F_ID;
		TriangleMeshModel bunny;
		Vec3f			  posLumiere = Vec3f( 1.5f / 0.003f, 1.5f / 0.003f, -0.3f / 0.003f );
		// ================
		static const std::string _shaderFolder;
	};
} // namespace M3D_ISICG

#endif // __LAB_WORK_5_HPP__
