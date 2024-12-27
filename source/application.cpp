
#include <application.h>
#include <windows.h>
#include <random>
#include <renderer.h>

Application::Application(HINSTANCE hInstance) : state(aOpen) {

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

	// -- Create window

	window_handle = CreateWindow(window_class_name.c_str(), "Memory Visualizer", WS_OVERLAPPEDWINDOW, 0, 0, 640, 480, NULL, NULL, hInstance, this);

	// -- Get the device context associated with the window

	window_dc = GetDC(window_handle);

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

  	gl = new OpenGL_Loader();

	// -- Show window

	ShowWindow(window_handle, SW_SHOW);

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
			DestroyWindow(hWnd);
			PostQuitMessage(0);
			break;
		case WM_PAINT:
			OnRender();
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
