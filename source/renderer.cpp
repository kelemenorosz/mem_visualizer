
#include <renderer.h>
#include <wavefront_loader.h>
#include <wavefront_object.h>
#include <shader_loader.h>
#include <log_loader.h>
#include <stb/stb_image.h>
#include <algorithm>
#include <iostream>

Renderer::Renderer(OpenGL_Loader* gl, int viewport_width, int viewport_height) : gl(gl), viewport_width(viewport_width), viewport_height(viewport_height) {

	// -- Get screen size

	HDC screen_dc = GetDC(NULL);
	int horizontal_resolution = GetDeviceCaps(screen_dc, HORZRES);
	int vertical_resolution = GetDeviceCaps(screen_dc, VERTRES);

	// -- Load log

	memmap = Log_Loader::Load("./log/serial.log");

	// -- Load wavefront object

	cube_obj = Wavefront_Loader::Load("./mesh/cube_2.obj");	

	// -- Load shaders

	program = Shader_Loader::Load(*gl, "vf", "./source/shaders/vertex.glsl", "./source/shaders/fragment.glsl");

	// -- Use OpenGL program

	gl->glUseProgram(program);

	// -- Load texture file 

	int texture_width, texture_height, texture_channels;
	stbi_set_flip_vertically_on_load(true);
	unsigned char* texture = stbi_load("./texture/quartz.png", &texture_width, &texture_height, &texture_channels, 0); 

	// -- Set texture buffer

	gl->glGenTextures(1, &texture_buffer);
	gl->glBindTexture(GL_TEXTURE_2D, texture_buffer);
	gl->glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, texture_width, texture_height, 0, GL_RGB, GL_UNSIGNED_BYTE, texture);
	gl->glGenerateMipmap(GL_TEXTURE_2D);

	gl->glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	gl->glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	gl->glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAX_ANISOTROPY, 10.0f);

	texture_location = gl->glGetUniformLocation(program, "texture_buffer");	

	// -- Free texture

	stbi_image_free(texture);

	// -- Set index buffer
	
	gl->glGenBuffers(1, &index_buffer);
	gl->glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, index_buffer);
	gl->glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(GLint) * cube_obj->index_list.size(), &cube_obj->index_list[0], GL_STATIC_DRAW);

	// -- Set vertex buffer
	
	gl->glGenBuffers(1, &vertex_buffer);
	gl->glBindBuffer(GL_ARRAY_BUFFER, vertex_buffer);
	gl->glBufferData(GL_ARRAY_BUFFER, sizeof(Vertex) * cube_obj->vertex_list.size(), &cube_obj->vertex_list[0], GL_STATIC_DRAW);

	// -- Create instance buffer

	INT instance_row = 32;
	INT instance_column = 32;
	instance_count = instance_row * instance_column;

	std::vector<Instance> instance_data;

	for (int i = 0; i < instance_row; ++i) {
		for (int j = 0; j < instance_column; ++j) {
			if ((i * instance_column + j) % 2 == 0) {
				instance_data.emplace_back(static_cast<float>(i * 3), static_cast<float>(j * 3), 0.0f, 0.0f, 1.0f, 0.0f);
			}
			else {
				instance_data.emplace_back(static_cast<float>(i * 3), static_cast<float>(j * 3), 0.0f, 1.0f, 0.0f, 0.0f);
			}
		}
	}

	gl->glGenBuffers(1, &instance_buffer);
	gl->glBindBuffer(GL_ARRAY_BUFFER, instance_buffer);
	gl->glBufferData(GL_ARRAY_BUFFER, instance_count * sizeof(Instance), &instance_data[0], GL_STATIC_DRAW);

	// -- Set world/projection matrix

	camera_position = glm::vec3(0.0f, 0.0f, -3.0f);
	

	// world_matrix = glm::rotate(glm::mat4(1.0f), glm::radians(45.0f), glm::normalize(glm::vec3(0, 1, 0)));
	glm::mat4 translate_matrix = glm::translate(glm::mat4(1.0f), glm::vec3(-50.0f, -50.0f, 0.0f));
	glm::mat4 scale_matrix = glm::scale(glm::mat4(1.0f), glm::vec3(55.0f, 55.0f, 55.0f));
	world_matrix = translate_matrix;
	camera_position = scale_matrix * glm::vec4(camera_position, 1.0f);
	view_matrix = glm::lookAtLH(camera_position, glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, 1.0f, 0.0f));
	projection_matrix = glm::perspectiveLH(glm::radians(75.0f), 4.0f / 3.0f, 0.1f, 1000.0f);

	projection_world_matrix = projection_matrix * view_matrix * world_matrix; 

	matrix_location = gl->glGetUniformLocation(program, "MVP");
	
	// -- Set color switch uniform location

	color_switch_location = gl->glGetUniformLocation(program, "color_switch");
	color_switch = 0;

	// -- Create framebuffer

	gl->glGenFramebuffers(1, &frame_buffer);
	gl->glBindFramebuffer(GL_FRAMEBUFFER, frame_buffer);

	gl->glGenTextures(1, &fb_render_buffer);
	gl->glBindTexture(GL_TEXTURE_2D_MULTISAMPLE, fb_render_buffer);
	gl->glTexImage2DMultisample(GL_TEXTURE_2D_MULTISAMPLE, 8, GL_RGB, horizontal_resolution, vertical_resolution, GL_TRUE);
	gl->glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D_MULTISAMPLE, fb_render_buffer, 0);

	gl->glGenTextures(1, &fb_depth_stencil_buffer);
	gl->glBindTexture(GL_TEXTURE_2D_MULTISAMPLE, fb_depth_stencil_buffer);
	gl->glTexImage2DMultisample(GL_TEXTURE_2D_MULTISAMPLE, 8, GL_DEPTH24_STENCIL8, horizontal_resolution, vertical_resolution, GL_TRUE);
	gl->glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_TEXTURE_2D_MULTISAMPLE, fb_depth_stencil_buffer, 0);

	// -- Setup

	gl->glEnable(GL_DEPTH_TEST);
	gl->glEnable(GL_MULTISAMPLE);
	gl->glEnable(GL_CULL_FACE);

	gl->glDepthFunc(GL_LESS);
	gl->glCullFace(GL_FRONT);
	gl->glFrontFace(GL_CCW);

	// -- Memory map visualization setup

	memmap_parse_index = 0;
	memmap_start = memmap[0].pages_location;
	memmap_error = false;
	memmap_render_error = false;
	memmap_end_error = false;

	std::cout << "[MEMMAP] Start: " << std::hex << memmap_start << std::dec << std::endl;

	memmap_state.resize(instance_count);
	gl->glGenBuffers(1, &memmap_buffer);
	gl->glBindBuffer(GL_ARRAY_BUFFER, memmap_buffer);
	gl->glBufferData(GL_ARRAY_BUFFER, sizeof(float) * memmap_state.size(), &memmap_state[0], GL_STATIC_DRAW);

	return;

}


Renderer::~Renderer() {

	// -- Cleanup

	delete cube_obj;

	return;

}

void Renderer::OnRender() {

	gl->glBindFramebuffer(GL_FRAMEBUFFER, frame_buffer);
	// gl->glBindFramebuffer(GL_FRAMEBUFFER, 0);

	gl->glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	// gl->glClear(GL_DEPTH_BUFFER_BIT);

	gl->glViewport(0, 0, viewport_width, viewport_height);

	gl->glUniformMatrix4fv(matrix_location, 1, GL_FALSE, &projection_world_matrix[0][0]);
	
	gl->glUniform1i(color_switch_location, color_switch);

	gl->glActiveTexture(GL_TEXTURE1);
	gl->glBindTexture(GL_TEXTURE_2D, texture_buffer);
	
	gl->glActiveTexture(GL_TEXTURE0);
	gl->glBindTexture(GL_TEXTURE_2D, 0);
	
	gl->glUniform1i(texture_location, 1);

	gl->glEnableVertexAttribArray(0);
	gl->glBindBuffer(GL_ARRAY_BUFFER, vertex_buffer);
	gl->glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, position));

	gl->glEnableVertexAttribArray(1);
	gl->glBindBuffer(GL_ARRAY_BUFFER, vertex_buffer);
	gl->glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, texture));

	gl->glEnableVertexAttribArray(2);
	gl->glBindBuffer(GL_ARRAY_BUFFER, instance_buffer);
	gl->glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, sizeof(Instance), (void*)offsetof(Instance, position));

	gl->glEnableVertexAttribArray(3);
	gl->glBindBuffer(GL_ARRAY_BUFFER, instance_buffer);
	gl->glVertexAttribPointer(3, 3, GL_FLOAT, GL_FALSE, sizeof(Instance), (void*)offsetof(Instance, color));

	gl->glVertexAttribDivisor(2, 1);
	gl->glVertexAttribDivisor(3, 1);

	// -- Memmap binding

	gl->glEnableVertexAttribArray(4);
	gl->glBindBuffer(GL_ARRAY_BUFFER, memmap_buffer);
	gl->glVertexAttribPointer(4, 1, GL_FLOAT, GL_FALSE, sizeof(float), (void*)0);
	gl->glVertexAttribDivisor(4, 1);

	gl->glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, index_buffer);
	gl->glDrawElementsInstanced(GL_TRIANGLES, static_cast<GLsizei>(cube_obj->index_list.size()), GL_UNSIGNED_INT, 0, instance_count);

	gl->glFinish();

	gl->glDisableVertexAttribArray(0);
	gl->glDisableVertexAttribArray(1);

	gl->glBindFramebuffer(GL_READ_FRAMEBUFFER, frame_buffer);
	gl->glBindFramebuffer(GL_DRAW_FRAMEBUFFER, 0);
	gl->glBlitFramebuffer(0, 0, viewport_width, viewport_height, 0, 0, viewport_width, viewport_height, GL_COLOR_BUFFER_BIT, GL_NEAREST);

	return;

}

void Renderer::OnUpdate(double value) {

	static double total_time = 0.0f;
	static int frame_count = 0;

	static double total_time_memmap = 0.0f;

	total_time += value;
	total_time_memmap += value;
	frame_count++;

	if (total_time > 1.0f) {

		std::cout << "[FPS] " << frame_count << std::endl;

		total_time -= 1.0f;
		frame_count = 0;

		color_switch ^= 1;


	} 

	if (total_time_memmap > 0.1f) {

		total_time_memmap -= 0.1f;
		Update_MemmapState();

	}

	return;

}

void Renderer::OnMouseWheel(float value) {

	glm::mat4 scalingMatrix;
	if (value > 0.0f) {

		scalingMatrix = glm::scale(glm::mat4(1.0f), glm::vec3(0.9f, 0.9f, 0.9f));

	}
	else {

		scalingMatrix = glm::scale(glm::mat4(1.0f), glm::vec3(1.1f, 1.1f, 1.1f));

	}

	glm::vec4 temp_camera_position = scalingMatrix * glm::vec4(camera_position, 1.0f);
	camera_position.x = temp_camera_position.x;
	camera_position.y = temp_camera_position.y;
	camera_position.z = temp_camera_position.z;
	view_matrix = glm::lookAtLH(camera_position, glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, 1.0f, 0.0f));

	projection_world_matrix = projection_matrix * view_matrix * world_matrix; 	

	return;

}

void Renderer::OnMouseButtonLeftMove(WORD x1, WORD x2, WORD y1, WORD y2) {

	glm::vec3 v1 = this->GetUnitVector(x1, y1);
	glm::vec3 v2 = this->GetUnitVector(x2, y2);

	FLOAT rotationAngle = glm::dot(v1, v2);
	rotationAngle = std::min(1.0f, acosf(rotationAngle)) * 2.0f;

	glm::vec3 rotationAxis = glm::cross(v1, v2);

	glm::mat4 rotationMatrix = glm::rotate(glm::mat4(1.0f), rotationAngle, rotationAxis);
	rotationMatrix = glm::inverse(rotationMatrix);
	world_matrix = world_matrix * rotationMatrix;

	projection_world_matrix = projection_matrix * view_matrix * world_matrix; 

	return;

}

void Renderer::OnResize(int width, int height) {

	viewport_width = width;
	viewport_height = height;

	return;
}

void Renderer::OnKeyPress(unsigned __int8 scancode, unsigned __int8 is_repeat) {

	glm::vec3 camera_movement; 

	if (scancode == 17) {

		// -- W

		camera_movement = glm::vec3(0.0f, 0.5f, 0.0f);

	}
	else if (scancode == 30) {

		// -- A

		camera_movement = glm::vec3(-0.5f, 0.0f, 0.0f);
	
	}
	else if (scancode == 31) {

		// -- S
		
		camera_movement = glm::vec3(0.0f, -0.5f, 0.0f);

	}
	else if (scancode == 32) {

		// -- D

		camera_movement = glm::vec3(0.5f, 0.0f, 0.0f);
	
	}

	glm::mat4 translate_matrix = glm::translate(glm::mat4(1.0f), camera_movement);
	world_matrix = world_matrix * translate_matrix;

	projection_world_matrix = projection_matrix * view_matrix * world_matrix; 	

	return;

}


glm::vec3 Renderer::GetUnitVector(WORD x, WORD y) {

	FLOAT vectorX = 0.0f;
	FLOAT vectorY = 0.0f;
	FLOAT vectorZ = 0.0f;
	FLOAT xySquared = 0.0f;

	auto sphereDiameter = std::min(viewport_width, viewport_height);
	FLOAT sphereRadius = static_cast<FLOAT>(sphereDiameter) / 2.0f;

	//Not normalized coordinates
	vectorX = (FLOAT) x - (FLOAT)(viewport_width / 2);
	vectorY = (FLOAT)(viewport_height - y) - (FLOAT)(viewport_height / 2);

	//Cap coordinates
	if (abs(vectorX) > sphereRadius) {
		
		if (vectorX < 0.0f) vectorX = -sphereRadius;
		else vectorX = sphereRadius;
	
	}

	if (abs(vectorY) > sphereRadius) {

		if (vectorY < 0.0f) vectorY = -sphereRadius;
		else vectorY = sphereRadius;

	}

	//Normalized coordinates
	vectorX = vectorX / sphereRadius;
	vectorY = vectorY / sphereRadius;

	//Get Z coordinate
	xySquared = vectorX * vectorX + vectorY * vectorY;
	if (xySquared > 1.0f) {
		
		glm::vec3 returnVector = glm::normalize(glm::vec3(vectorX, vectorY, 0.0f));
		return returnVector;

	}
	
	vectorZ = -sqrtf(1.0f - xySquared);
	return glm::vec3(vectorX, vectorY, vectorZ);

}

void Renderer::Update_MemmapState() {

	if (memmap_error == false && memmap_render_error == false && memmap_end_error == false) {

		int location = memmap[memmap_parse_index].pages_location;
		int nr_allocated = memmap[memmap_parse_index].pages_allocated;
		int operation = memmap[memmap_parse_index].operation;

		location -= memmap_start;
		location /= 0x1000;

		if ((location + nr_allocated - 1) >= instance_count) {
			memmap_render_error = true;
			std::cout << "[MEMMAP] Render error." << std::endl;
			return;
		}

		for (int i = 0; i < nr_allocated; ++i) {

			// -- Alloc
			if (operation == 0) {
				if (memmap_state[location + i] == 0) {
					memmap_state[location + i] = 1;
				}
				else {
					memmap_state[location + i] = 2;
					memmap_error = true;
				}
			} 
			// -- Free
			else {
				if (memmap_state[location + i] == 1) {
					memmap_state[location + i] = 0;
				}
				else {
					memmap_state[location + i] = 2;
					memmap_error = true;
				}
			}

		}

		memmap_parse_index++;
		if (memmap_parse_index > (memmap.size() - 1)) {
			memmap_end_error = true;
			std::cout << "[MEMMAP] End error." << std::endl;
		}

		gl->glBindBuffer(GL_ARRAY_BUFFER, memmap_buffer);
		gl->glBufferData(GL_ARRAY_BUFFER, sizeof(int) * memmap_state.size(), &memmap_state[0], GL_STATIC_DRAW);	

	}

	return;

}
