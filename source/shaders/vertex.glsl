#version 330 core

layout(location = 0) in vec3 vertex_position;
layout(location = 1) in vec2 vertex_texture;

uniform mat4 MVP;

out vec2 texture_coordinates;

void main(){
  gl_Position = MVP * vec4(vertex_position, 1);
  texture_coordinates = vertex_texture;
}