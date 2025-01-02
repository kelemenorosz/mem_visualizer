#include <application.h>
#include <windows.h>
#include <iostream>
#include <random>
#include <chrono>
#include <renderer.h>
#include <gl/wglext.h>

Application::Application(HINSTANCE hInstance) : state(aOpen), temp_window_status(true) {

	// -- RNG

	std::random_device rd;
	std::mt19937 mersenne_twister;
	mersenne_twister.seed(rd());

	unsigned int class_number = mersenne_twister();
	window_class_name = "Memory Visualizer Class" + std::to_string(class_number);

	// -- Register window class

	WNDCLASS window_class = {};  

	window_class.style = CS_HREDRAW | CS_VREDRAW | CS_OWNDC;
  	window_class.lpfnWndProc = s_WindowProc;
  	window_class.cbClsExtra = NULL;
  	window_class.cbWndExtra = NULL;
  	window_class.hInstance = hInstance;
  	window_class.hIcon = NULL;
  	window_class.hCursor = NULL;
  	window_class.hbrBackground = (HBRUSH)COLOR_GRAYTEXT;
  	window_class.lpszMenuName = "Memory Visualizer Menu";
  	window_class.lpszClassName = window_class_name.c_str();

	RegisterClass(&window_class);

	// -- Create temporary window

	window_handle = CreateWindow(window_class_name.c_str(), "Temp Window", WS_OVERLAPPEDWINDOW, 0, 0, 0, 0, NULL, NULL, hInstance, NULL);

	// -- Create Pixel Format Descriptor for the temporary Device Context
	
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

	// -- Get the device context associated with the temporary window

	window_dc = GetDC(window_handle);

	// -- Set the pixel format for the temporary window
	{	
		INT pixel_format_value = ChoosePixelFormat(window_dc, &pixel_format_desc);
		SetPixelFormat(window_dc, pixel_format_value, &pixel_format_desc);
	}	
	// -- Create temporary OpenGL context

	opengl_context_handle = wglCreateContext(window_dc);
  	wglMakeCurrent(window_dc, opengl_context_handle);

  	// -- Get WGL extension functions

  	PFNWGLGETEXTENSIONSSTRINGARBPROC wglGetExtensionsStringARB;
  	void* function_ptr = wglGetProcAddress("wglGetExtensionsStringARB");
  	if ((function_ptr == (void*)0) || (function_ptr == (void*)1) || (function_ptr == (void*)2) || (function_ptr == (void*)3) || (function_ptr == (void*)-1)) {
  		std::cout << "WGL extensions not found." << std::endl;
  	}  
  	else {
  		std::cout << "WGL extensions found." << std::endl;
  	}
  	wglGetExtensionsStringARB = reinterpret_cast<PFNWGLGETEXTENSIONSSTRINGARBPROC>(function_ptr);
  	
  	const char* extension_string = wglGetExtensionsStringARB(window_dc);
  	char* extension_string_search = new char[strlen(extension_string) + 1];
  	memcpy(extension_string_search, extension_string, strlen(extension_string) + 1);

  	char* strtok_context;
  	char* extension_name = strtok_s(extension_string_search, " ", &strtok_context);
  	bool create_context_profile_available = false;
  	bool choose_pixel_format_available = false;

  	while (extension_name != nullptr) {
  		if (strcmp(extension_name, "WGL_ARB_create_context_profile") == 0) create_context_profile_available = true;
  		if (strcmp(extension_name, "WGL_ARB_pixel_format") == 0) choose_pixel_format_available = true;
  		extension_name = strtok_s(NULL, " ", &strtok_context);
  	}

  	delete[] extension_string_search;

	PFNWGLCREATECONTEXTATTRIBSARBPROC wglCreateContextAttribsARB;
	function_ptr = wglGetProcAddress("wglCreateContextAttribsARB");
	if ((function_ptr == (void*)0) || (function_ptr == (void*)1) || (function_ptr == (void*)2) || (function_ptr == (void*)3) || (function_ptr == (void*)-1)) {
  		std::cout << "WGL create context attribs not found." << std::endl;
  	}  
  	else {
  		std::cout << "WGL create context attribs found." << std::endl;
  	}
  	wglCreateContextAttribsARB = reinterpret_cast<PFNWGLCREATECONTEXTATTRIBSARBPROC>(function_ptr);

	PFNWGLCHOOSEPIXELFORMATARBPROC wglChoosePixelFormatARB;
	function_ptr = wglGetProcAddress("wglChoosePixelFormatARB");
	if ((function_ptr == (void*)0) || (function_ptr == (void*)1) || (function_ptr == (void*)2) || (function_ptr == (void*)3) || (function_ptr == (void*)-1)) {
  		std::cout << "WGL choose pixel format not found." << std::endl;
  	}  
  	else {
  		std::cout << "WGL choose pixel format found." << std::endl;
  	}
  	wglChoosePixelFormatARB = reinterpret_cast<PFNWGLCHOOSEPIXELFORMATARBPROC>(function_ptr);

  	// -- Delete the temporary OpenGL context

  	wglMakeCurrent(window_dc, NULL);
  	wglDeleteContext(opengl_context_handle);

	// -- Release the temporary window's device context

	ReleaseDC(window_handle, window_dc);

	// -- Destroy temporary window

	DestroyWindow(window_handle);
	temp_window_status = false;

	// -- Create window

	window_handle = CreateWindow(window_class_name.c_str(), "Memory Visualizer", WS_OVERLAPPEDWINDOW, 0, 0, 640, 480, NULL, NULL, hInstance, this);

	// -- Get the device context associated with the window

	window_dc = GetDC(window_handle);
	
	// -- Set the pixel format for the window

	int pixel_attrib_list[] = {
		WGL_DRAW_TO_WINDOW_ARB, GL_TRUE,
	    WGL_SUPPORT_OPENGL_ARB, GL_TRUE,
	    WGL_DOUBLE_BUFFER_ARB, GL_TRUE,
	    WGL_PIXEL_TYPE_ARB, WGL_TYPE_RGBA_ARB,
	    WGL_COLOR_BITS_ARB, 32,
	    WGL_DEPTH_BITS_ARB, 24,
	    WGL_STENCIL_BITS_ARB, 8,
	    0
	};
	int pixel_format;
	unsigned int num_formats;
	wglChoosePixelFormatARB(window_dc, pixel_attrib_list, NULL, 1, &pixel_format, &num_formats);
	SetPixelFormat(window_dc, pixel_format, &pixel_format_desc);

	// TODO: Use this if WGL extensions are not available
  	// {	
	// 	INT pixel_format_value = ChoosePixelFormat(window_dc, &pixel_format_desc);
	// 	SetPixelFormat(window_dc, pixel_format_value, &pixel_format_desc);
	// }	

  	// -- Create OpenGL context

	int context_attrib_list[] = {
		WGL_CONTEXT_MAJOR_VERSION_ARB, 3,
		WGL_CONTEXT_MINOR_VERSION_ARB, 0,
		0
	};
	opengl_context_handle = wglCreateContextAttribsARB(window_dc, 0, context_attrib_list);

	// TODO: Use this if WGL extensions are not available
  	// opengl_context_handle = wglCreateContext(window_dc);

  	// -- Set OpenGL context as current
  	// TODO: check for return value

  	wglMakeCurrent(window_dc, opengl_context_handle);

  	// -- Create OpenGL function loader

  	gl = new OpenGL_Loader();

	// -- Show window

	ShowWindow(window_handle, SW_SHOW);

	// -- Set t0

	t0 = std::chrono::high_resolution_clock::now();
	elapsed_time = 0.0f;

	return;

}

Application::~Application() {

	// -- Delete Renderer

	delete renderer;

	// -- Delete OpenGL function loader

	delete gl; 

	return;

}

LRESULT CALLBACK Application::s_WindowProc(HWND windowInstance, UINT uMsg, WPARAM wParam, LPARAM lParam) {

	Application* application = nullptr;
	if (uMsg == WM_NCCREATE) {

		LPCREATESTRUCT createStruct = reinterpret_cast<LPCREATESTRUCT>(lParam);
		application = static_cast<Application*>(createStruct->lpCreateParams);
		SetWindowLongPtr(windowInstance, GWLP_USERDATA, reinterpret_cast<LONG_PTR>(application));

	}
	else {

		application = reinterpret_cast<Application*>(GetWindowLongPtr(windowInstance, GWLP_USERDATA));

	}

	if (application == nullptr) return DefWindowProc(windowInstance, uMsg, wParam, lParam);

	return application->WindowProc(windowInstance, uMsg, wParam, lParam);

}

LRESULT CALLBACK Application::WindowProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam) {

	switch(msg) {

		case WM_DESTROY:
			if (temp_window_status == false) {
				PostQuitMessage(0);
			}
			break;
		case WM_PAINT:
			OnRender();
			OnUpdate();
			break;
		case WM_SIZING:
			OnResizing(wParam, lParam);
			break;
		case WM_SIZE:
			OnResize(wParam, lParam);
			break;
		case WM_CHAR:
			OnChar(wParam, lParam);
			break;
		case WM_MOUSEWHEEL:
			OnMouseWheel(wParam, lParam);
			break;
		case WM_LBUTTONDOWN:
			x1 = LOWORD(lParam);
			y1 = HIWORD(lParam);
			break;
		case WM_MOUSEMOVE:
			if (wParam == MK_LBUTTON) {
				WORD x2 = LOWORD(lParam);
				WORD y2 = HIWORD(lParam);
				if (x2 != x1 || y2 != y1) OnMouseButtonLeftMove(x1, x2, y1, y2);
				x1 = x2;
				y1 = y2;
			}
			break;
		default:
			return DefWindowProc(hWnd, msg, wParam, lParam);
	
	}

	return S_OK;

}

void Application::OnRender() {

	if (state == aRunning) {

		renderer->OnRender();

	}

	SwapBuffers(window_dc);

	return;

}

void Application::OnUpdate() {

	if (state == aRunning) {
		
		std::chrono::high_resolution_clock::time_point t1 = std::chrono::high_resolution_clock::now();
		std::chrono::duration<__int64, std::nano> delta = t1 - t0;
		t0 = t1;

		elapsed_time = delta.count() * 1e-9;

		renderer->OnUpdate(elapsed_time);

	}

	return;

}

void Application::OnMouseButtonLeftMove(WORD x1, WORD x2, WORD y1, WORD y2) {

	if (state == aRunning) {

		renderer->OnMouseButtonLeftMove(x1, x2, y1, y2);

	}

	return;

}

void Application::OnMouseWheel(WPARAM wParam, LPARAM lParam) {

	if (state == aRunning) {

		FLOAT delta = ((SHORT) HIWORD(wParam)) / (FLOAT) WHEEL_DELTA;
		renderer->OnMouseWheel(delta);

	}

	return;

}

void Application::OnResizing(WPARAM wParam, LPARAM lParam) {

	RECT* window_rectangle = reinterpret_cast<RECT*>(lParam);
	std::cout << "[WINDOW_RESIZING] left: " << window_rectangle->left << ", right: " << window_rectangle->right << ", top: " << window_rectangle->top << ", bottom: " << window_rectangle->bottom << std::endl;

	return;

}

void Application::OnResize(WPARAM wParam, LPARAM lParam) {

	UINT16 width = static_cast<UINT16>(lParam);
	UINT16 height = static_cast<UINT16>(lParam >> 0x10);

	std::cout << "[WINDOW_RESIZE] width: " << width	<< ", height: " << height << std::endl;

	if (state == aRunning) {
		renderer->OnResize(width, height);
	}

	return;

}

void Application::OnChar(WPARAM wParam, LPARAM lParam) {

	UINT8 scancode = (UINT8)(lParam >> 16);
	UINT8 isRepeat = (UINT8)(lParam >> 24);

	std::cout << "[KEYPRESS] " << static_cast<INT>(scancode) << " " << static_cast<INT>(isRepeat) << std::endl;

	if (state == aRunning) {
		renderer->OnKeyPress(scancode, isRepeat);
	}

	return;

}
