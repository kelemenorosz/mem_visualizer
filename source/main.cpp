#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <thread>
#include <random>
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
#include <application.h>
#include <renderer.h>

#define STB_IMAGE_IMPLEMENTATION
#include <stb/stb_image.h>

void ApplicationThread(HINSTANCE hInstance);

int WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nShowCmd) {

	// -- Allocate console for the process

	AllocConsole();
	FILE* dummy_file;
	freopen_s(&dummy_file, "CONOUT$", "w", stdout);
	std::cout.clear();

	// -- Create applications

	std::thread app(ApplicationThread, hInstance);

	app.join();

	return 0;

}

void ApplicationThread(HINSTANCE hInstance) {

	// -- Thread for an application

	Application* application = new Application(hInstance);

	// -- Set the renderer to be used

	application->RegisterRenderer<Renderer>();

	MSG msg = {};

	while (GetMessage(&msg, NULL, 0, 0) != 0) {

		TranslateMessage(&msg);
		DispatchMessage(&msg);
		
	}

	delete application;

	return;

}

