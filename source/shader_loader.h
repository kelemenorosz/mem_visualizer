#pragma once

#include <GL/glcorearb.h>
#include <GL/gl.h>
#include <GL/glext.h>
#include <opengl_loader.h>

namespace Shader_Loader {

	GLuint Load(const OpenGL_Loader& gl, const char* format, ...);

}