#version 460 core
#extension GL_ARB_separate_shader_objects : enable

layout(location=0) in vec3 position;
layout(location=1) in vec3 normal;
layout(location=2) in vec2 coordinate;

layout(location=0) out vec3 pos_result;
layout(location=1) out vec3 norm_result;
layout(location=2) out vec2 coord_result;

layout(binding=0) uniform pos_mat_uniform { mat4 pos_mat; } pos_mat;

void main() {
  gl_Position = pos_mat.pos_mat * vec4(position, 1.0f);
  pos_result = position;
  norm_result = normal;
  coord_result = coordinate;
}
