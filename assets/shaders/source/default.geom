#version 460 core
#extension GL_ARB_separate_shader_objects : enable

layout (triangles) in;
layout (line_strip, max_vertices = 6) out;

layout(binding=1) uniform mat { mat4 position_matrix; };

layout(location=0) in vec3 position1[3];
layout(location=1) in vec3 normal1[3];
layout(location=2) in vec2 coordinate1[3];

layout(location=0) out vec3 position2;
layout(location=1) out vec3 normal2;
layout(location=2) out vec2 coordinate2;

const float MAGNITUDE = 0.4f;

void GenerateLine(int index) {
    gl_Position = gl_in[index].gl_Position;
    EmitVertex();
    gl_Position = gl_in[index].gl_Position + vec4(normal1[index], 0.0f) * MAGNITUDE;
    EmitVertex();
    EndPrimitive();
}

void main() {
    GenerateLine(0);// First vertex normal
    GenerateLine(1);// Second vertex normal
    GenerateLine(2);// Third vertex normal
}
