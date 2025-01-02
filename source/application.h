#pragma once

#include <windows.h>
#include <string>
#include <chrono>
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
		void OnUpdate();
		void OnMouseButtonLeftMove(WORD x1, WORD x2, WORD y1, WORD y2);
		void OnMouseWheel(WPARAM wParam, LPARAM lParam);
		void OnResizing(WPARAM wParam, LPARAM lParam);
		void OnResize(WPARAM wParam, LPARAM lParam);
		void OnChar(WPARAM wParam, LPARAM lParam);

		static LRESULT CALLBACK s_WindowProc(HWND windowInstance, UINT uMsg, WPARAM wParam, LPARAM lParam);
		LRESULT CALLBACK WindowProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);

	private:

		HWND window_handle;
		std::string window_class_name;
		HDC window_dc;
		HGLRC opengl_context_handle;
		OpenGL_Loader* gl;
		ApplicationState state;
		Base_Renderer* renderer;

		WORD x1;
		WORD y1;

		bool temp_window_status;

		std::chrono::high_resolution_clock::time_point t0;
		double elapsed_time;

};

template<typename RENDERTYPE>
void Application::RegisterRenderer() {

	// -- Create renderer

	renderer = new RENDERTYPE(gl, 640, 480);

	// -- Set state to aRunning

	state = aRunning;

	return;

}