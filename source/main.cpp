#include <iostream>
#include <windows.h>
#include <GL/glcorearb.h>
#include <GL/gl.h>
#include <GL/glext.h>
#include <opengl_loader.h>
#include <wavefront_loader.h>

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
  	pixel_format_desc.cColorBits = 24; // TODO: This might have to be 32. Although I have my doubts about it
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

	// -- OpenGL test

	GLuint VertexArrayID;
	gl->glGenVertexArrays(1, &VertexArrayID);
	gl->glBindVertexArray(VertexArrayID);

	static const GLfloat g_vertex_buffer_data[] = {
	   -1.0f, -1.0f, 0.0f,
	   1.0f, -1.0f, 0.0f,
	   0.0f,  1.0f, 0.0f,
	};

	// This will identify our vertex buffer
	GLuint vertexbuffer;
	// Generate 1 buffer, put the resulting identifier in vertexbuffer
	gl->glGenBuffers(1, &vertexbuffer);
	// The following commands will talk about our 'vertexbuffer' buffer
	gl->glBindBuffer(GL_ARRAY_BUFFER, vertexbuffer);
	// Give our vertices to OpenGL.
	gl->glBufferData(GL_ARRAY_BUFFER, sizeof(g_vertex_buffer_data), g_vertex_buffer_data, GL_STATIC_DRAW);

	// 1st attribute buffer : vertices
	gl->glEnableVertexAttribArray(0);
	gl->glBindBuffer(GL_ARRAY_BUFFER, vertexbuffer);
	gl->glVertexAttribPointer(
	   0,                  // attribute 0. No particular reason for 0, but must match the layout in the shader.
	   3,                  // size
	   GL_FLOAT,           // type
	   GL_FALSE,           // normalized?
	   0,                  // stride
	   (void*)0            // array buffer offset
	);
	// Draw the triangle !
	gl->glDrawArrays(GL_TRIANGLES, 0, 3); // Starting from vertex 0; 3 vertices total -> 1 triangle
	gl->glDisableVertexAttribArray(0);

	SwapBuffers(window_dc);

	// -- Start message loop

	MSG msg = {};

	while (msg.message != WM_QUIT) {

		GetMessage(&msg, NULL, 0, 0);
		if (msg.hwnd == window_handle) {
			
			TranslateMessage(&msg);
			DispatchMessage(&msg);
		
		}
	}

	// -- Release device context

	ReleaseDC(window_handle, window_dc);

	// -- Delete OpenGL context loader

	OpenGLLoaderDelete(gl);

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

	Wavefront_Loader::Load();

	return 0;

}