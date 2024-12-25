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
PFNGLDRAWELEMENTSPROC OpenGL_Loader::glDrawElements;
PFNGLDISABLEVERTEXATTRIBARRAYPROC OpenGL_Loader::glDisableVertexAttribArray;
PFNGLCREATESHADERPROC OpenGL_Loader::glCreateShader;
PFNGLSHADERSOURCEPROC OpenGL_Loader::glShaderSource;
PFNGLCOMPILESHADERPROC OpenGL_Loader::glCompileShader;
PFNGLGETSHADERIVPROC OpenGL_Loader::glGetShaderiv;
PFNGLGETSHADERINFOLOGPROC OpenGL_Loader::glGetShaderInfoLog;
PFNGLCREATEPROGRAMPROC OpenGL_Loader::glCreateProgram;
PFNGLATTACHSHADERPROC OpenGL_Loader::glAttachShader;
PFNGLLINKPROGRAMPROC OpenGL_Loader::glLinkProgram;
PFNGLGETPROGRAMIVPROC OpenGL_Loader::glGetProgramiv;
PFNGLGETPROGRAMINFOLOGPROC OpenGL_Loader::glGetProgramInfoLog;
PFNGLDELETESHADERPROC OpenGL_Loader::glDeleteShader;
PFNGLUSEPROGRAMPROC OpenGL_Loader::glUseProgram;
PFNGLUNIFORMMATRIX4FVPROC OpenGL_Loader::glUniformMatrix4fv;
PFNGLGETUNIFORMLOCATIONPROC OpenGL_Loader::glGetUniformLocation;
PFNGLCLEARPROC OpenGL_Loader::glClear;
PFNGLENABLEPROC OpenGL_Loader::glEnable;
PFNGLGENTEXTURESPROC OpenGL_Loader::glGenTextures;
PFNGLBINDTEXTUREPROC OpenGL_Loader::glBindTexture;
PFNGLTEXIMAGE2DPROC OpenGL_Loader::glTexImage2D;
PFNGLACTIVETEXTUREPROC OpenGL_Loader::glActiveTexture;
PFNGLUNIFORM1IPROC OpenGL_Loader::glUniform1i;
PFNGLTEXPARAMETERIPROC OpenGL_Loader::glTexParameteri;
PFNGLGENERATEMIPMAPPROC OpenGL_Loader::glGenerateMipmap;
PFNGLTEXPARAMETERFPROC OpenGL_Loader::glTexParameterf;
PFNGLGENFRAMEBUFFERSPROC OpenGL_Loader::glGenFramebuffers;
PFNGLCHECKFRAMEBUFFERSTATUSPROC OpenGL_Loader::glCheckFramebufferStatus;
PFNGLBINDFRAMEBUFFERPROC OpenGL_Loader::glBindFramebuffer;
PFNGLTEXIMAGE2DMULTISAMPLEPROC OpenGL_Loader::glTexImage2DMultisample;
PFNGLFRAMEBUFFERTEXTURE2DPROC OpenGL_Loader::glFramebufferTexture2D;
PFNGLBLITFRAMEBUFFERPROC OpenGL_Loader::glBlitFramebuffer;
PFNGLDISABLEPROC OpenGL_Loader::glDisable;

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