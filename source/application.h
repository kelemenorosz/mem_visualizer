#pragma once

#include <windows.h>
#include <string>
#include <opengl_loader.h>
#include <base_renderer.h>

enum ApplicationState {

	aOpen = 0,
	aRunning,

};

class Application {

	public:

		Application() = delete;
		Application(HINSTANCE hInstance);
		~Application();

		template<typename RENDERTYPE>
		void RegisterRenderer();
		void OnRender();
		void OnMouseButtonLeftMove(WORD x1, WORD x2, WORD y1, WORD y2);
		void OnMouseWheel(WPARAM wParam, LPARAM lParam);

		static LRESULT CALLBACK s_WindowProc(HWND windowInstance, UINT uMsg, WPARAM wParam, LPARAM lParam);
		LRESULT CALLBACK WindowProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);

	private:

		HWND window_handle;
		std::string window_class_name;
		HDC window_dc;
		OpenGL_Loader* gl;
		ApplicationState state;
		Base_Renderer* renderer;

		WORD x1;
		WORD y1;

};

template<typename RENDERTYPE>
void Application::RegisterRenderer() {

	// -- Create renderer

	renderer = new RENDERTYPE(gl);

	// -- Set state to aRunning

	state = aRunning;

	return;

}