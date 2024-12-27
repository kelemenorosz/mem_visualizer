
#include <renderer.h>
#include <wavefront_loader.h>
#include <wavefront_object.h>
#include <shader_loader.h>
#include <stb/stb_image.h>
#include <algorithm>

Renderer::Renderer(OpenGL_Loader* gl) : gl(gl) {

	// -- Load wavefront object

	cube_obj = Wavefront_Loader::Load("./mesh/cube_numbers_forward.obj");	

	// -- Load shaders

	program = Shader_Loader::Load(*gl, "vf", "./source/shaders/vertex.glsl", "./source/shaders/fragment.glsl");

	// -- Use OpenGL program

	gl->glUseProgram(program);

	// -- Load texture file 

	int texture_width, texture_height, texture_channels;
	stbi_set_flip_vertically_on_load(true);
	unsigned char* texture = stbi_load("./texture/numbers.png", &texture_width, &texture_height, &texture_channels, 0); 

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

	INT instance_row = 2;
	INT instance_column = 2;
	instance_count = instance_row * instance_column;

	float* instance_data = new float[instance_row * instance_column * 3];
	float* instance_ptr = instance_data;

	for (int i = 0; i < instance_row; ++i) {
		for (int j = 0; j < instance_column; ++j) {
			instance_ptr[0] = static_cast<float>(i * 3);
			instance_ptr[1] = 0.0f;
			instance_ptr[2] = static_cast<float>(j * 3);
			instance_ptr += 3;
		}
	}

	gl->glGenBuffers(1, &instance_buffer);
	gl->glBindBuffer(GL_ARRAY_BUFFER, instance_buffer);
	gl->glBufferData(GL_ARRAY_BUFFER, instance_count * sizeof(float) * 3, instance_data, GL_STATIC_DRAW);

	delete[] instance_data;

	// -- Set world/projection matrix

	camera_position = glm::vec3(0.0f, 1.5f, -3.0f);

	// world_matrix = glm::rotate(glm::mat4(1.0f), glm::radians(45.0f), glm::normalize(glm::vec3(0, 1, 0)));
	world_matrix = glm::mat4(1.0f);
	view_matrix = glm::lookAtLH(camera_position, glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, 1.0f, 0.0f));
	projection_matrix = glm::perspectiveLH(glm::radians(90.0f), 4.0f / 3.0f, 1.0f, 100.0f);

	projection_world_matrix = projection_matrix * view_matrix * world_matrix; 

	matrix_location = gl->glGetUniformLocation(program, "MVP");

	// -- Create framebuffer

	gl->glGenFramebuffers(1, &frame_buffer);
	gl->glBindFramebuffer(GL_FRAMEBUFFER, frame_buffer);

	gl->glGenTextures(1, &fb_render_buffer);
	gl->glBindTexture(GL_TEXTURE_2D_MULTISAMPLE, fb_render_buffer);
	gl->glTexImage2DMultisample(GL_TEXTURE_2D_MULTISAMPLE, 8, GL_RGB, 640, 480, GL_TRUE);
	gl->glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D_MULTISAMPLE, fb_render_buffer, 0);

	gl->glGenTextures(1, &fb_depth_stencil_buffer);
	gl->glBindTexture(GL_TEXTURE_2D_MULTISAMPLE, fb_depth_stencil_buffer);
	gl->glTexImage2DMultisample(GL_TEXTURE_2D_MULTISAMPLE, 8, GL_DEPTH24_STENCIL8, 640, 480, GL_TRUE);
	gl->glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_TEXTURE_2D, fb_depth_stencil_buffer, 0);

	// -- Setup

	gl->glDisable(GL_DEPTH_TEST);
	gl->glEnable(GL_MULTISAMPLE);
	gl->glEnable(GL_CULL_FACE);

	gl->glDepthFunc(GL_GREATER);
	gl->glCullFace(GL_FRONT);
	gl->glFrontFace(GL_CCW);

	return;

}


Renderer::~Renderer() {

	// -- Cleanup

	delete cube_obj;

	return;

}

void Renderer::OnRender() {

	// gl->glBindFramebuffer(GL_FRAMEBUFFER, frame_buffer);
	gl->glBindFramebuffer(GL_FRAMEBUFFER, 0);

	gl->glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	// gl->glClear(GL_DEPTH_BUFFER_BIT);

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

	gl->glEnableVertexAttribArray(2);
	gl->glBindBuffer(GL_ARRAY_BUFFER, instance_buffer);
	gl->glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, sizeof(float) * 3, (void*)0);

	gl->glVertexAttribDivisor(2, 1);

	gl->glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, index_buffer);
	// gl->glDrawElements(GL_TRIANGLES, cube_obj->index_list.size(), GL_UNSIGNED_INT, 0);
	gl->glDrawElementsInstanced(GL_TRIANGLES, static_cast<GLsizei>(cube_obj->index_list.size()), GL_UNSIGNED_INT, 0, instance_count);

	gl->glFinish();

	gl->glDisableVertexAttribArray(0);
	gl->glDisableVertexAttribArray(1);

	// gl->glBindFramebuffer(GL_READ_FRAMEBUFFER, frame_buffer);
	// gl->glBindFramebuffer(GL_DRAW_FRAMEBUFFER, 0);
	// gl->glBlitFramebuffer(0, 0, 640, 480, 0, 0, 640, 480, GL_COLOR_BUFFER_BIT, GL_NEAREST);

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

glm::vec3 Renderer::GetUnitVector(WORD x, WORD y) {

	FLOAT vectorX = 0.0f;
	FLOAT vectorY = 0.0f;
	FLOAT vectorZ = 0.0f;
	FLOAT xySquared = 0.0f;

	auto sphereDiameter = std::min(640, 480);
	FLOAT sphereRadius = static_cast<FLOAT>(sphereDiameter) / 2.0f;

	//Not normalized coordinates
	vectorX = (FLOAT) x - (FLOAT)(640 / 2);
	vectorY = (FLOAT)(480 - y) - (FLOAT)(480 / 2);

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