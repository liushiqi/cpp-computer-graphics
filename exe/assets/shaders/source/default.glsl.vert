#version 460 core
#extension GL_ARB_separate_shader_objects : enable
#extension GL_ARB_gpu_shader_fp64 : enable
#extension GL_ARB_gpu_shader_int64 : enable

layout(location=0) in vec3 position;
layout(location=1) in vec3 normal;
layout(location=2) in vec2 coordinate;

layout(binding=0) uniform pos_mat_uniform { mat4 pos_mat; } pos_mat;

out vec3 pos_result;
out vec3 norm_result;
out vec2 coord_result;

void main() {
    gl_Position = pos_mat.pos_mat * vec4(position, 1.0f);
    pos_result = position;
    norm_result = normal;
    coord_result = coordinate;
}
