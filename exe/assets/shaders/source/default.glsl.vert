#version 460 core
#extension GL_ARB_separate_shader_objects : enable
#extension GL_ARB_gpu_shader_fp64 : enable
#extension GL_ARB_gpu_shader_int64 : enable

layout(location=0) in dvec3 position;
layout(location=2) in dvec3 normal;
layout(location=4) in dvec2 coordinate;

layout(binding=0) uniform pos_mat_uniform { mat4 pos_mat; } pos_mat;

out dvec3 pos_result;
out dvec3 norm_result;
out dvec2 coord_result;

void main() {
    gl_Position = pos_mat.pos_mat * vec4(position, 1.0f);
    pos_result = position;
    norm_result = normal;
    coord_result = coordinate;
}
