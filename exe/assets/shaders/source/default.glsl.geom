#version 460 core
#extension GL_ARB_separate_shader_objects : enable
#extension GL_ARB_gpu_shader_fp64 : enable
#extension GL_ARB_gpu_shader_int64 : enable

layout(triangles) in;
layout(line_strip, max_vertices=6) out;

in dvec3 pos_result[3];
in dvec3 norm_result[3];
in dvec2 coord_result[3];

layout(binding=1) uniform rotation_uniform { float rotation; } rotation;

out dvec3 pos_result_geom;
out dvec3 norm_result_geom;
out dvec2 coord_result_geom;

void GenerateLine(int index) {
    gl_Position = gl_in[index].gl_Position;
    EmitVertex();
    gl_Position = gl_in[index].gl_Position + vec4(norm_result[index], 0.0f) * rotation.rotation;
    EmitVertex();
    EndPrimitive();
}

void main() {
    GenerateLine(0);// First vertex normal
    GenerateLine(1);// Second vertex normal
    GenerateLine(2);// Third vertex normal
}
