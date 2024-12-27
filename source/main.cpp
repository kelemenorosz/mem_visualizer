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

	// -- Create applications

	std::thread app(ApplicationThread, hInstance);

	app.join();

	return 0;

}

void ApplicationThread(HINSTANCE hInstance) {

	Application* application = new Application(hInstance);

	application->RegisterRenderer<Renderer>();

	MSG msg = {};

	while (GetMessage(&msg, NULL, 0, 0) != 0) {

		TranslateMessage(&msg);
		DispatchMessage(&msg);
		
	}

	delete application;

	return;

}

int main() {

	INT instance_row = 10;
	INT instance_column = 10;
	INT instance_count = instance_row * instance_column;
	INT instance_buffer_size = instance_count * 3;

	float* instance_data = new float[instance_buffer_size];
	float* instance_ptr = instance_data;

	std::cout << reinterpret_cast<unsigned long long>(instance_data) << std::endl;

	for (int i = 0; i < instance_row; ++i) {
		for (int j = 0; j < instance_column; ++j) {
			instance_ptr[0] = static_cast<float>(i * 3);
			instance_ptr[1] = 0.0f;
			instance_ptr[2] = static_cast<float>(j * 3);
			instance_ptr += 3;
			std::cout << reinterpret_cast<unsigned long long>(instance_ptr) << std::endl;
		}
	}

	delete[] instance_data;

	return 0;

}
