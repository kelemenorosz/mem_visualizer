cmake_minimum_required(VERSION 3.0)
add_executable(
	main WIN32
	main.cpp
	application.cpp
	opengl_loader.cpp
	renderer.cpp
	shader_loader.cpp
	wavefront_loader.cpp
	wavefront_object.cpp
	)
target_include_directories(main PRIVATE ${CMAKE_CURRENT_SOURCE_DIR})
target_link_libraries(main Opengl32.lib)
install(TARGETS main DESTINATION ${CMAKE_SOURCE_DIR})