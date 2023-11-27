#ifndef __LAB_WORK_2_HPP__
#define __LAB_WORK_2_HPP__

#include "GL/gl3w.h"
#include "common/base_lab_work.hpp"
#include "define.hpp"
#include <vector>

namespace M3D_ISICG
{
	class LabWork2 : public BaseLabWork
	{
	  public:
		LabWork2() : BaseLabWork() {}
		~LabWork2();

		bool init() override;
		void animate( const float p_deltaTime ) override;
		void render() override;

		void handleEvents( const SDL_Event & p_event ) override;
		void displayUI() override;
		void disque(const Vec2f c, const int n, const float r);

	  private:
		// ================ Scene data.
		std::vector<Vec2f> sommetsTriangle;
		std::vector<int> vectorRect;
		std::vector<Vec3f> couleurs;
		// ================

		// ================ GL data.
		GLuint			   program;
		GLuint			   vbo;
		GLuint			   vao;
		GLuint			   ebo;
		GLuint			   vboCouleur;
		GLint			   loc;	
		GLint			   locLum;		
		// ================

		// ================ Settings.
		Vec4f _bgColor = Vec4f( 0.8f, 0.8f, 0.8f, 1.f ); // Background color
		float _time = 0.0f, luminosite = 1.0f, r = 0.5f;
		bool  modificationLuminosite = 0, modificationFond = 0;
		int	  n = 64;
		Vec2f c = Vec2f( 0.3, -0.2 );
		// ================

		static const std::string _shaderFolder;
	};
} // namespace M3D_ISICG

#endif // __LAB_WORK_1_HPP__
