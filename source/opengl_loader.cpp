#include <windows.h>

#include <opengl_loader.h>

#define FormatLoad(func) \
	this->Load(this->func, #func)

OpenGL_Loader::OpenGL_Loader() {

	FormatLoad(glGenVertexArrays);
	FormatLoad(glBindVertexArray);
	FormatLoad(glGenBuffers);
	FormatLoad(glBindBuffer);
	FormatLoad(glBufferData);
	FormatLoad(glEnableVertexAttribArray);
	FormatLoad(glVertexAttribPointer);
	FormatLoad(glDrawArrays);
	FormatLoad(glDrawElements);
	FormatLoad(glDisableVertexAttribArray);
	FormatLoad(glCreateShader);
	FormatLoad(glShaderSource);
	FormatLoad(glCompileShader);
	FormatLoad(glGetShaderiv);
	FormatLoad(glGetShaderInfoLog);
	FormatLoad(glCreateProgram);
	FormatLoad(glAttachShader);
	FormatLoad(glLinkProgram);
	FormatLoad(glGetProgramiv);
	FormatLoad(glGetProgramInfoLog);
	FormatLoad(glDeleteShader);
	FormatLoad(glUseProgram);
	FormatLoad(glUniformMatrix4fv);
	FormatLoad(glGetUniformLocation);
	FormatLoad(glClear);
	FormatLoad(glEnable);
	FormatLoad(glGenTextures);
	FormatLoad(glBindTexture);
	FormatLoad(glTexImage2D);
	FormatLoad(glActiveTexture);
	FormatLoad(glUniform1i);
	FormatLoad(glTexParameteri);
	FormatLoad(glGenerateMipmap);
	FormatLoad(glTexParameterf);
	FormatLoad(glGenFramebuffers);
	FormatLoad(glCheckFramebufferStatus);
	FormatLoad(glBindFramebuffer);
	FormatLoad(glTexImage2DMultisample);
	FormatLoad(glFramebufferTexture2D);
	FormatLoad(glBlitFramebuffer);
	FormatLoad(glDisable);
	FormatLoad(glFinish);
	FormatLoad(glCullFace);
	FormatLoad(glDrawElementsInstanced);
	FormatLoad(glVertexAttribDivisor);
	FormatLoad(glDepthFunc);
	FormatLoad(glFrontFace);

	return;

}

		
OpenGL_Loader::~OpenGL_Loader() {

	return;

}

template<typename FUNCTYPE>
void OpenGL_Loader::Load(FUNCTYPE& func, char* function_string) {

	void* function_ptr = wglGetProcAddress(function_string);
	
	if ((function_ptr == (void*)0) || (function_ptr == (void*)1) || (function_ptr == (void*)2) || (function_ptr == (void*)3) || (function_ptr == (void*)-1)) {

		HMODULE opengl_handle = LoadLibrary("opengl32.dll");
		function_ptr = GetProcAddress(opengl_handle, function_string);	

	}

	func = reinterpret_cast<FUNCTYPE>(function_ptr);

	return;

}