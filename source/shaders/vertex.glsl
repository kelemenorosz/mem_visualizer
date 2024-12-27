#version 330 core

layout(location = 0) in vec3 vertex_position;
layout(location = 1) in vec2 vertex_texture;
layout(location = 2) in vec3 instance;

uniform mat4 MVP;

out vec2 texture_coordinates;

void main(){
  vec3 temp_vector = vertex_position + instance;
  gl_Position = MVP * vec4(temp_vector, 1);
  texture_coordinates = vertex_texture;
}