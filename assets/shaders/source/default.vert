#version 460 core
#extension GL_ARB_separate_shader_objects : enable

in vec3 position;
in vec3 normal;
in vec2 coordinate;

out vec3 pos_result;
out vec3 norm_result;
out vec2 coord_result;

uniform mat { mat4 position_matrix; };

void main() {
  gl_Position = position_matrix * vec4(position, 1.0f);
  pos_result = position;
  norm_result = normal;
  coord_result = coordinate;
}
