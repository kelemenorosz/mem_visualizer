#pragma once

#include <base_renderer.h>
#include <GL/glcorearb.h>
#include <GL/gl.h>
#include <GL/glext.h>
#include <glm/glm.hpp>
#include <glm/ext/matrix_transform.hpp>
#include <glm/ext/matrix_clip_space.hpp>
#include <opengl_loader.h>
#include <wavefront_loader.h>
#include <wavefront_object.h>

class Renderer : public Base_Renderer {

	public:

		Renderer() = delete;
		Renderer(OpenGL_Loader* gl);
		~Renderer();
		void OnRender();
		void OnMouseButtonLeftMove(WORD x1, WORD x2, WORD y1, WORD y2);
		void OnMouseWheel(float value);

	private:

		glm::vec3 GetUnitVector(WORD x, WORD y);

		OpenGL_Loader* gl;
		GLuint program;
		GLuint texture_buffer;
		GLuint index_buffer;
		GLuint vertex_buffer;
		GLuint instance_buffer;

		GLuint frame_buffer;
		GLuint fb_render_buffer;
		GLuint fb_depth_stencil_buffer;

		glm::mat4 projection_world_matrix;
		glm::mat4 world_matrix;
		glm::mat4 view_matrix;
		glm::mat4 projection_matrix;

		glm::vec3 camera_position;

		GLint texture_location;
		GLint matrix_location;

		Wavefront_Object *cube_obj;

		int instance_count;

};