#version 330 core

out vec3 color;
in vec2 texture_coordinates;
in vec3 color_modifier;

uniform sampler2D texture_buffer;

void main(){
  color = texture(texture_buffer, texture_coordinates).rgb;
  color = mix(color, color_modifier, 0.2);
}
