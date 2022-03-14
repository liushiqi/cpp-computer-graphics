#version 460 core
#extension GL_ARB_separate_shader_objects : enable

layout(location=0) in vec3 position1;
layout(location=1) in vec3 normal1;
layout(location=2) in vec2 coordinate1;

layout(binding=2) uniform sampler2D diffuse;
layout(binding=3) uniform vec { vec3 light; };

layout(location=0) out vec4 result;
layout(location=1) out vec4 color_result;

void main() {
  vec3 a_to_l = light - position1;
  float cos_alpha = dot(a_to_l, normal1) / length(normal1) / length(a_to_l);
  cos_alpha = cos_alpha > 0 ? cos_alpha : 0;

  result = texture(diffuse, coordinate1) * pow(cos_alpha, 2);
  color_result = vec4(1.0f, 1.0f, 1.0f, 1.0f);
}
