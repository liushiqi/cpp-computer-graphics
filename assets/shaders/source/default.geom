#version 460 core

layout (triangles) in;
layout (line_strip, max_vertices = 6) out;

uniform mat { mat4 position_matrix; };

out vec3 position1[3];
in vec3 normal1[3];
in vec2 coordinate1[3];

out vec3 position2;
out vec3 normal2;
out vec2 coordinate2;

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
