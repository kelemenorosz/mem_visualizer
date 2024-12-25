#version 330 core

out vec3 color;
in vec2 texture_coordinates;

uniform sampler2D texture1;

void main(){
  color = texture(texture1, texture_coordinates).rgb;
  //color = vec3(1, 0, 0);

}
