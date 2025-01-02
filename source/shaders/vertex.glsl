#version 330 core

layout(location = 0) in vec3 vertex_position;
layout(location = 1) in vec2 vertex_texture;
layout(location = 2) in vec3 instance;
layout(location = 3) in vec3 instance_color;
layout(location = 4) in float memmap_state;

uniform mat4 MVP;
uniform int color_switch;

out vec2 texture_coordinates;
out vec3 color_modifier;

void main(){
  vec3 temp_vector = vertex_position + instance;
  gl_Position = MVP * vec4(temp_vector, 1);
  texture_coordinates = vertex_texture;
  if (memmap_state == 0.0f) {
    color_modifier = vec3(0.0f, 0.0f, 0.0f);
  } 
  else if (memmap_state == 1.0f) {
    color_modifier = vec3(0.0f, 1.0f, 0.0f);
  }
  else if (memmap_state == 2.0f) {
    color_modifier = vec3(1.0f, 0.0f, 0.0f);
  }
}