#pragma once

#include <windows.h>
#include <GL/glcorearb.h>
#include <GL/gl.h>
#include <GL/glext.h>

class OpenGL_Loader {

	public:
		
		OpenGL_Loader();
		~OpenGL_Loader();

		PFNGLGENVERTEXARRAYSPROC glGenVertexArrays;
		PFNGLBINDVERTEXARRAYPROC glBindVertexArray;
		PFNGLGENBUFFERSPROC glGenBuffers;
		PFNGLBINDBUFFERPROC glBindBuffer;
		PFNGLBUFFERDATAPROC glBufferData;
		PFNGLENABLEVERTEXATTRIBARRAYPROC glEnableVertexAttribArray;
		PFNGLVERTEXATTRIBPOINTERPROC glVertexAttribPointer;
		PFNGLDRAWARRAYSPROC glDrawArrays;
		PFNGLDRAWELEMENTSPROC glDrawElements;
		PFNGLDISABLEVERTEXATTRIBARRAYPROC glDisableVertexAttribArray;
		PFNGLCREATESHADERPROC glCreateShader;
		PFNGLSHADERSOURCEPROC glShaderSource; 
		PFNGLCOMPILESHADERPROC glCompileShader;
		PFNGLGETSHADERIVPROC glGetShaderiv; 
		PFNGLGETSHADERINFOLOGPROC glGetShaderInfoLog;
		PFNGLCREATEPROGRAMPROC glCreateProgram;
		PFNGLATTACHSHADERPROC glAttachShader;
		PFNGLLINKPROGRAMPROC glLinkProgram;
		PFNGLGETPROGRAMIVPROC glGetProgramiv;
		PFNGLGETPROGRAMINFOLOGPROC glGetProgramInfoLog;
		PFNGLDELETESHADERPROC glDeleteShader;
		PFNGLUSEPROGRAMPROC glUseProgram;
		PFNGLUNIFORMMATRIX4FVPROC glUniformMatrix4fv;
		PFNGLGETUNIFORMLOCATIONPROC glGetUniformLocation;
		PFNGLCLEARPROC glClear;
		PFNGLENABLEPROC glEnable;
		PFNGLGENTEXTURESPROC glGenTextures;
		PFNGLBINDTEXTUREPROC glBindTexture;
		PFNGLTEXIMAGE2DPROC glTexImage2D;
		PFNGLACTIVETEXTUREPROC glActiveTexture;
		PFNGLUNIFORM1IPROC glUniform1i;
		PFNGLTEXPARAMETERIPROC glTexParameteri;
		PFNGLGENERATEMIPMAPPROC glGenerateMipmap;
		PFNGLTEXPARAMETERFPROC glTexParameterf;
		PFNGLGENFRAMEBUFFERSPROC glGenFramebuffers;
		PFNGLCHECKFRAMEBUFFERSTATUSPROC glCheckFramebufferStatus;
		PFNGLBINDFRAMEBUFFERPROC glBindFramebuffer;
		PFNGLTEXIMAGE2DMULTISAMPLEPROC glTexImage2DMultisample;
		PFNGLFRAMEBUFFERTEXTURE2DPROC glFramebufferTexture2D;
		PFNGLBLITFRAMEBUFFERPROC glBlitFramebuffer;
		PFNGLDISABLEPROC glDisable;
		PFNGLFINISHPROC glFinish;
		PFNGLCULLFACEPROC glCullFace;
		PFNGLDRAWELEMENTSINSTANCEDPROC glDrawElementsInstanced;
		PFNGLVERTEXATTRIBDIVISORPROC glVertexAttribDivisor;
		PFNGLDEPTHFUNCPROC glDepthFunc;
		PFNGLFRONTFACEPROC glFrontFace;
		PFNGLVIEWPORTPROC glViewport;

	private:

		template<typename FUNCTYPE>
		void Load(FUNCTYPE& func, char* function_string);

};
