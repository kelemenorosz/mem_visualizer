#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <windows.h>
#include <GL/glcorearb.h>
#include <GL/gl.h>
#include <GL/glext.h>
#include <glm/glm.hpp>
#include <glm/ext/matrix_transform.hpp>
#include <glm/ext/matrix_clip_space.hpp>
#include <opengl_loader.h>
#include <wavefront_loader.h>
#include <wavefront_object.h>
#include <shader_loader.h>

#define STB_IMAGE_IMPLEMENTATION
#include <stb/stb_image.h>

LRESULT CALLBACK WindowCallback(HWND hWnd, UINT Msg, WPARAM wParam, LPARAM lParam);

int WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nShowCmd) {

	// -- Register window class

	WNDCLASS window_class = {};  

	window_class.style = CS_HREDRAW | CS_VREDRAW | CS_OWNDC;
  	window_class.lpfnWndProc = WindowCallback;
  	window_class.cbClsExtra = NULL;
  	window_class.cbWndExtra = NULL;
  	window_class.hInstance = hInstance;
  	window_class.hIcon = NULL;
  	window_class.hCursor = NULL;
  	window_class.hbrBackground = (HBRUSH)COLOR_GRAYTEXT;
  	window_class.lpszMenuName = "Memory Visualizer Menu";
  	window_class.lpszClassName = "Memory Visualizer Class";

	RegisterClass(&window_class);

	// -- Create window

	HWND window_handle = CreateWindow("Memory Visualizer Class", "Memory Visualizer", WS_OVERLAPPEDWINDOW, 0, 0, 640, 480, NULL, NULL, hInstance, nullptr);

	// -- Get the device context associated with the window

	HDC window_dc = GetDC(window_handle);

	// -- Create Pixel Format Descriptor for the Device Context
	
	// Refer to the ChoosePixelFormat() documentation on how to set the PIXELFORMATDESCRIPTOR 
	// https://learn.microsoft.com/en-us/windows/win32/api/wingdi/nf-wingdi-choosepixelformat

	PIXELFORMATDESCRIPTOR pixel_format_desc = {};

	pixel_format_desc.nSize = sizeof(PIXELFORMATDESCRIPTOR);
  	pixel_format_desc.nVersion = 1;
  	pixel_format_desc.dwFlags = PFD_SUPPORT_OPENGL | PFD_DRAW_TO_WINDOW | PFD_DOUBLEBUFFER;
  	pixel_format_desc.iPixelType = PFD_TYPE_RGBA;
  	pixel_format_desc.cColorBits = 32 * 4; // TODO: This might have to be 32. Although I have my doubts about it
  	pixel_format_desc.cRedBits = 0;
  	pixel_format_desc.cRedShift = 0;
  	pixel_format_desc.cGreenBits = 0;
  	pixel_format_desc.cGreenShift = 0;
  	pixel_format_desc.cBlueBits = 0;
  	pixel_format_desc.cBlueShift = 0;
  	pixel_format_desc.cAlphaBits = 0; // No alpha buffer
  	pixel_format_desc.cAlphaShift = 0;
  	pixel_format_desc.cAccumBits = 0; // No accumulation buffer
  	pixel_format_desc.cAccumRedBits = 0;
  	pixel_format_desc.cAccumGreenBits = 0;
  	pixel_format_desc.cAccumBlueBits = 0;
  	pixel_format_desc.cAccumAlphaBits = 0;
  	pixel_format_desc.cDepthBits = 24; // 24-bit depth buffer
  	pixel_format_desc.cStencilBits = 8; // 8-bit stencil buffer
  	pixel_format_desc.cAuxBuffers = 0; // No auxiliary buffers
  	pixel_format_desc.iLayerType = PFD_MAIN_PLANE; // Documentation indicates this field is ignored in latter implementations of OpenGL
  	pixel_format_desc.bReserved = 0;
  	pixel_format_desc.dwLayerMask = 0;
  	pixel_format_desc.dwVisibleMask = 0;
  	pixel_format_desc.dwDamageMask = 0;

  	INT pixel_format_value = ChoosePixelFormat(window_dc, &pixel_format_desc);
  	SetPixelFormat(window_dc, pixel_format_value, &pixel_format_desc);

  	// -- Create OpenGL context

  	HGLRC opengl_context_handle = wglCreateContext(window_dc);

  	// -- Set OpenGL context as current
  	// TODO: check for return value

  	wglMakeCurrent(window_dc, opengl_context_handle);

  	// -- Create OpenGL function loader

  	OpenGL_Loader* gl = OpenGLLoaderInit();

	// -- Show window

	ShowWindow(window_handle, SW_SHOW);

	// -- Create console

	AllocConsole();
	FILE* dummy_file;
	freopen_s(&dummy_file, "CONOUT$", "w", stdout);
	std::cout.clear();

	// -- Load wavefront object

	Wavefront_Object *cube_obj = Wavefront_Loader::Load("./mesh/cube.obj");	

	// -- Load shaders

	GLuint program = Shader_Loader::Load(*gl, "vf", "./source/shaders/vertex.glsl", "./source/shaders/fragment.glsl");

	// -- Use OpenGL program

	gl->glUseProgram(program);

	// -- Load texture file 

	int texture_width, texture_height, texture_channels;
	unsigned char* texture = stbi_load("./texture/quartz.png", &texture_width, &texture_height, &texture_channels, 0); 

	if (texture == NULL) {
		std::cout << "[STBI] stbi_load() failed." << std::endl;
	}
	else {
		std::cout << "[TEXTURE] Texture width: " << texture_width << ", height: " << texture_height << ", channels: " << texture_channels << std::endl;
	}

	// -- Set texture buffer

	GLuint texture_buffer;
	gl->glGenTextures(1, &texture_buffer);
	gl->glBindTexture(GL_TEXTURE_2D, texture_buffer);
	gl->glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, texture_width, texture_height, 0, GL_RGB, GL_UNSIGNED_BYTE, texture);
	gl->glGenerateMipmap(GL_TEXTURE_2D);

	gl->glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	gl->glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	gl->glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAX_ANISOTROPY, 10.0f);

	GLint texture_location = gl->glGetUniformLocation(program, "texture_buffer");	

	// -- Free texture

	stbi_image_free(texture);

	// -- Set index buffer
	
	GLuint index_buffer;
	gl->glGenBuffers(1, &index_buffer);
	gl->glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, index_buffer);
	gl->glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(GLint) * cube_obj->index_list.size(), &cube_obj->index_list[0], GL_STATIC_DRAW);

	// -- Set vertex buffer
	
	GLuint vertex_buffer;
	gl->glGenBuffers(1, &vertex_buffer);
	gl->glBindBuffer(GL_ARRAY_BUFFER, vertex_buffer);
	gl->glBufferData(GL_ARRAY_BUFFER, sizeof(Vertex) * cube_obj->vertex_list.size(), &cube_obj->vertex_list[0], GL_STATIC_DRAW);

	// -- Set world/projection matrix

	glm::mat4 model_matrix = glm::rotate(glm::mat4(1.0f), glm::radians(45.0f), glm::normalize(glm::vec3(0, 1, 0)));
	glm::mat4 world_matrix = glm::lookAtLH(glm::vec3(0.0f, 1.5f, -3.0f), glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, 1.0f, 0.0f));
	// glm::mat4 projection_matrix = glm::perspectiveFovLH(0.0174533f, 640.0f, 480.0f, 0.1f, 100.0f);
	glm::mat4 projection_matrix = glm::perspectiveLH(glm::radians(90.0f), 4.0f / 3.0f, 0.1f, 100.0f);

	glm::mat4 projection_world_matrix = projection_matrix * world_matrix * model_matrix; 

	GLint matrix_location = gl->glGetUniformLocation(program, "MVP");

	// -- Create framebuffer

	GLuint frame_buffer;
	gl->glGenFramebuffers(1, &frame_buffer);
	gl->glBindFramebuffer(GL_FRAMEBUFFER, frame_buffer);

	GLuint fb_render_buffer;
	gl->glGenTextures(1, &fb_render_buffer);
	gl->glBindTexture(GL_TEXTURE_2D_MULTISAMPLE, fb_render_buffer);
	gl->glTexImage2DMultisample(GL_TEXTURE_2D_MULTISAMPLE, 8, GL_RGB, 640, 480, GL_TRUE);
	gl->glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D_MULTISAMPLE, fb_render_buffer, 0);

	GLuint fb_depth_stencil_buffer;
	gl->glGenTextures(1, &fb_depth_stencil_buffer);
	gl->glBindTexture(GL_TEXTURE_2D_MULTISAMPLE, fb_depth_stencil_buffer);
	gl->glTexImage2DMultisample(GL_TEXTURE_2D_MULTISAMPLE, 8, GL_DEPTH24_STENCIL8, 640, 480, GL_TRUE);
	gl->glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_TEXTURE_2D, fb_depth_stencil_buffer, 0);

	if (gl->glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE) {
		std::cout << "[FRAMEBUFFER] Framebuffer is not complete." << std::endl;
	}
	else {
		std::cout << "[FRAMEBUFFER] Framebuffer is complete." << std::endl;
	}

	// gl->glBindFramebuffer(GL_FRAMEBUFFER, 0);

	// -- Setup

	gl->glEnable(GL_DEPTH_TEST);
	gl->glEnable(GL_MULTISAMPLE);

	// -- Start message loop

	MSG msg = {};

	while (msg.message != WM_QUIT) {

		GetMessage(&msg, NULL, 0, 0);
		if (msg.hwnd == window_handle) {

			gl->glBindFramebuffer(GL_FRAMEBUFFER, frame_buffer);

			gl->glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
			// gl->glClear(GL_COLOR_BUFFER_BIT);

			gl->glUniformMatrix4fv(matrix_location, 1, GL_FALSE, &projection_world_matrix[0][0]);

			gl->glActiveTexture(GL_TEXTURE0);
			gl->glBindTexture(GL_TEXTURE_2D, texture_buffer);
			gl->glUniform1i(texture_location, 0);

			gl->glEnableVertexAttribArray(0);
			gl->glBindBuffer(GL_ARRAY_BUFFER, vertex_buffer);
			gl->glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, position));

			gl->glEnableVertexAttribArray(1);
			gl->glBindBuffer(GL_ARRAY_BUFFER, vertex_buffer);
			gl->glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, texture));

			gl->glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, index_buffer);
			gl->glDrawElements(GL_TRIANGLES, cube_obj->index_list.size(), GL_UNSIGNED_INT, 0);

			gl->glDisableVertexAttribArray(0);
			gl->glDisableVertexAttribArray(1);


			gl->glBindFramebuffer(GL_READ_FRAMEBUFFER, frame_buffer);
			gl->glBindFramebuffer(GL_DRAW_FRAMEBUFFER, 0);
			gl->glBlitFramebuffer(0, 0, 640, 480, 0, 0, 640, 480, GL_COLOR_BUFFER_BIT, GL_NEAREST);

			SwapBuffers(window_dc);

			TranslateMessage(&msg);
			DispatchMessage(&msg);
		
		}
	}

	// -- Release device context

	ReleaseDC(window_handle, window_dc);

	// -- Delete OpenGL context loader

	OpenGLLoaderDelete(gl);

	// -- Delete wavefront object

	delete cube_obj;

	return 0;

}

LRESULT CALLBACK WindowCallback(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam) {

	switch(msg) {

		case WM_DESTROY:
			DestroyWindow(hWnd);
			PostQuitMessage(0);
			break;
		default:
			return DefWindowProc(hWnd, msg, wParam, lParam);			
	
	}

	return S_OK;

}

int main() {

	Wavefront_Object *obj = Wavefront_Loader::Load("./mesh/cube.obj");

	return 0;

}