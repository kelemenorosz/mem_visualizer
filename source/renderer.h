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
#include <log_loader.h>

struct Instance {

	glm::vec3 position;
	glm::vec3 color;

	Instance() = delete;
	Instance(float x, float y, float z, float r, float g, float b) {
	
		position = glm::vec3(x, y, z);
		color = glm::vec3(r, g, b);
		return;

	}

};

class Renderer : public Base_Renderer {

	public:

		Renderer() = delete;
		Renderer(OpenGL_Loader* gl, int viewport_width, int viewport_height);
		~Renderer();
		void OnRender();
		void OnUpdate(double value);
		void OnMouseButtonLeftMove(WORD x1, WORD x2, WORD y1, WORD y2);
		void OnMouseWheel(float value);
		void OnResize(int width, int height);
		void OnKeyPress(unsigned __int8 scancode, unsigned __int8 is_repeat);

	private:

		glm::vec3 GetUnitVector(WORD x, WORD y);
		void Update_MemmapState();

		OpenGL_Loader* gl;
		GLuint program;
		GLuint texture_buffer;
		GLuint index_buffer;
		GLuint vertex_buffer;
		GLuint instance_buffer;
		GLuint memmap_buffer;

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
		GLint color_switch_location;

		Wavefront_Object *cube_obj;

		int instance_count;

		int viewport_width;
		int viewport_height;

		int color_switch;

		std::vector<Log_Entry> memmap;
		int memmap_parse_index;
		int memmap_start;
		std::vector<float> memmap_state; 
		bool memmap_error;
		bool memmap_render_error;
		bool memmap_end_error;

};