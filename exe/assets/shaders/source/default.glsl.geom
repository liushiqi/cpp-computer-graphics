#version 460 core
#extension GL_ARB_separate_shader_objects : enable

layout(triangles) in;
layout(line_strip, max_vertices=6) out;

layout(location=0) in vec3 pos_result[3];
layout(location=1) in vec3 norm_result[3];
layout(location=2) in vec2 coord_result[3];

layout(location=0) out vec3 pos_result_geom;
layout(location=1) out vec3 norm_result_geom;
layout(location=2) out vec2 coord_result_geom;

layout(binding=1) uniform rotation_uniform { float rotation; } rotation;

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
