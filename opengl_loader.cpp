#include <windows.h>

#include <opengl_loader.h>

#define FormatLoad(func) \
	this->Load(OpenGL_Loader::func, #func)

BOOL m_is_initialized = FALSE;

PFNGLGENVERTEXARRAYSPROC OpenGL_Loader::glGenVertexArrays;
PFNGLBINDVERTEXARRAYPROC OpenGL_Loader::glBindVertexArray;
PFNGLGENBUFFERSPROC OpenGL_Loader::glGenBuffers;
PFNGLBINDBUFFERPROC OpenGL_Loader::glBindBuffer;
PFNGLBUFFERDATAPROC OpenGL_Loader::glBufferData;
PFNGLENABLEVERTEXATTRIBARRAYPROC OpenGL_Loader::glEnableVertexAttribArray;
PFNGLVERTEXATTRIBPOINTERPROC OpenGL_Loader::glVertexAttribPointer;
PFNGLDRAWARRAYSPROC OpenGL_Loader::glDrawArrays;
PFNGLDISABLEVERTEXATTRIBARRAYPROC OpenGL_Loader::glDisableVertexAttribArray;

OpenGL_Loader* OpenGLLoaderInit() {

	if (m_is_initialized == FALSE) {

		OpenGL_Loader* loader = new OpenGL_Loader();
		return loader;

	}

	return nullptr;

}

void OpenGLLoaderDelete(OpenGL_Loader* loader) {

	delete loader;

	return;

}

OpenGL_Loader::OpenGL_Loader() {

	FormatLoad(glGenVertexArrays);
	FormatLoad(glBindVertexArray);
	FormatLoad(glGenBuffers);
	FormatLoad(glBindBuffer);
	FormatLoad(glBufferData);
	FormatLoad(glEnableVertexAttribArray);
	FormatLoad(glVertexAttribPointer);
	FormatLoad(glDrawArrays);
	FormatLoad(glDisableVertexAttribArray);

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