#pragma once

#include <windows.h>
#include <GL/glcorearb.h>
#include <GL/gl.h>
#include <GL/glext.h>

class OpenGL_Loader {

	public:

		static PFNGLGENVERTEXARRAYSPROC glGenVertexArrays;
		static PFNGLBINDVERTEXARRAYPROC glBindVertexArray;
		static PFNGLGENBUFFERSPROC glGenBuffers;
		static PFNGLBINDBUFFERPROC glBindBuffer;
		static PFNGLBUFFERDATAPROC glBufferData;
		static PFNGLENABLEVERTEXATTRIBARRAYPROC glEnableVertexAttribArray;
		static PFNGLVERTEXATTRIBPOINTERPROC glVertexAttribPointer;
		static PFNGLDRAWARRAYSPROC glDrawArrays;
		static PFNGLDRAWELEMENTSPROC glDrawElements;
		static PFNGLDISABLEVERTEXATTRIBARRAYPROC glDisableVertexAttribArray;
		static PFNGLCREATESHADERPROC glCreateShader;
		static PFNGLSHADERSOURCEPROC glShaderSource; 
		static PFNGLCOMPILESHADERPROC glCompileShader;
		static PFNGLGETSHADERIVPROC glGetShaderiv; 
		static PFNGLGETSHADERINFOLOGPROC glGetShaderInfoLog;
		static PFNGLCREATEPROGRAMPROC glCreateProgram;
		static PFNGLATTACHSHADERPROC glAttachShader;
		static PFNGLLINKPROGRAMPROC glLinkProgram;
		static PFNGLGETPROGRAMIVPROC glGetProgramiv;
		static PFNGLGETPROGRAMINFOLOGPROC glGetProgramInfoLog;
		static PFNGLDELETESHADERPROC glDeleteShader;
		static PFNGLUSEPROGRAMPROC glUseProgram;
		static PFNGLUNIFORMMATRIX4FVPROC glUniformMatrix4fv;
		static PFNGLGETUNIFORMLOCATIONPROC glGetUniformLocation;
		static PFNGLCLEARPROC glClear;
		static PFNGLENABLEPROC glEnable;
		static PFNGLGENTEXTURESPROC glGenTextures;
		static PFNGLBINDTEXTUREPROC glBindTexture;
		static PFNGLTEXIMAGE2DPROC glTexImage2D;
		static PFNGLACTIVETEXTUREPROC glActiveTexture;
		static PFNGLUNIFORM1IPROC glUniform1i;
		static PFNGLTEXPARAMETERIPROC glTexParameteri;
		static PFNGLGENERATEMIPMAPPROC glGenerateMipmap;
		static PFNGLTEXPARAMETERFPROC glTexParameterf;
		static PFNGLGENFRAMEBUFFERSPROC glGenFramebuffers;
		static PFNGLCHECKFRAMEBUFFERSTATUSPROC glCheckFramebufferStatus;
		static PFNGLBINDFRAMEBUFFERPROC glBindFramebuffer;
		static PFNGLTEXIMAGE2DMULTISAMPLEPROC glTexImage2DMultisample;
		static PFNGLFRAMEBUFFERTEXTURE2DPROC glFramebufferTexture2D;
		static PFNGLBLITFRAMEBUFFERPROC glBlitFramebuffer;
		static PFNGLDISABLEPROC glDisable;

	private:

		OpenGL_Loader();
		~OpenGL_Loader();
		friend OpenGL_Loader* OpenGLLoaderInit();
		friend void OpenGLLoaderDelete(OpenGL_Loader* loader);

		template<typename FUNCTYPE>
		void Load(FUNCTYPE& func, char* function_string);

};
