#include <shader_loader.h>
#include <iostream>
#include <fstream>
#include <sstream>
#include <cstdarg>
#include <string>
#include <vector>
#include <utility>

using namespace Shader_Loader;

GLuint Shader_Loader::Load(const OpenGL_Loader& gl, const char* format, ...) { 

	va_list args;
	va_start(args, format);

	std::ifstream file_stream;
	std::stringstream file_string_stream;
	std::string shader_string;

	std::vector<std::pair<GLenum, GLuint>> shaders;

	// -- Read and compile shaders

	while (*format != '\0') {

		const char* file_name = va_arg(args, const char*);
		file_stream.open(file_name, std::ios_base::in);
		file_string_stream.str("");
		file_string_stream.clear();
		file_string_stream << file_stream.rdbuf();
		shader_string = file_string_stream.str();
		const char* shader_string_ptr = shader_string.c_str();

		if (*format == 'v') {

			// -- Vertex shader
			shaders.emplace_back(GL_VERTEX_SHADER, gl.glCreateShader(GL_VERTEX_SHADER));
			
		}
		else if (*format == 'f') {

			// -- Fragment shader
			shaders.emplace_back(GL_FRAGMENT_SHADER, gl.glCreateShader(GL_FRAGMENT_SHADER));
			
		} 

		gl.glShaderSource(shaders.back().second, 1, &shader_string_ptr, NULL);
		gl.glCompileShader(shaders.back().second);

		file_stream.close();

		format++;

	}

	// -- Check compilation status

	GLint compile_status;
	int info_log_length;
	for (const auto& shader : shaders) {
		
		const char* shader_type;
		if (shader.first == GL_VERTEX_SHADER) {
			shader_type = "[VERTEX_SHADER]";
		} 
		else if (shader.first == GL_FRAGMENT_SHADER) {
			shader_type = "[FRAGMENT_SHADER]";
		}

		gl.glGetShaderiv(shader.second, GL_COMPILE_STATUS, &compile_status);
		if (compile_status == GL_FALSE) {
			std::cout << shader_type << " Compilation failed." << std::endl;
		}
		else {
			std::cout << shader_type << " Compilation successful." << std::endl;
		}

		gl.glGetShaderiv(shader.second, GL_INFO_LOG_LENGTH, &info_log_length);
		if (info_log_length > 0) {
			std::vector<char> log_msg(info_log_length);
			gl.glGetShaderInfoLog(shader.second, info_log_length, NULL, &log_msg[0]);
			std::cout << &log_msg[0] << std::endl;
		}

	}

	// -- Create program

	GLuint program = gl.glCreateProgram();
	
	for (const auto& shader : shaders) {
		gl.glAttachShader(program, shader.second);
	}

	gl.glLinkProgram(program);

	// -- Check program linkage

	GLint link_status = GL_FALSE;
	
	gl.glGetProgramiv(program, GL_LINK_STATUS, &link_status);
	if (link_status == GL_FALSE) {
		std::cout << "[PROGRAM] Linking failed." << std::endl;
	}
	else {
		std::cout << "[PROGRAM] Linking successful." << std::endl;
	}

	gl.glGetProgramiv(program, GL_INFO_LOG_LENGTH, &info_log_length);
	if (info_log_length > 0) {
		std::vector<char> log_msg(info_log_length);
		gl.glGetProgramInfoLog(program, info_log_length, NULL, &log_msg[0]);
		std::cout << &log_msg[0] << std::endl;
	}

	// -- Cleanup

	for (const auto& shader : shaders) {

		gl.glDeleteShader(shader.second);

	}

	return program;

}