#pragma once

#include <wavefront_object.h>
#include <string>

namespace Wavefront_Loader {

	Wavefront_Object* Load(const std::string& path);

}