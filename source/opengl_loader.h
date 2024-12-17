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
		static PFNGLDISABLEVERTEXATTRIBARRAYPROC glDisableVertexAttribArray;

	private:

		OpenGL_Loader();
		~OpenGL_Loader();
		friend OpenGL_Loader* OpenGLLoaderInit();
		friend void OpenGLLoaderDelete(OpenGL_Loader* loader);

		template<typename FUNCTYPE>
		void Load(FUNCTYPE& func, char* function_string);

};
