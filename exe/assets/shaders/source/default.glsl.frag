#version 460 core
#extension GL_ARB_separate_shader_objects : enable

layout(location=0) in vec3 pos_result_geom;
layout(location=1) in vec3 norm_result_geom;
layout(location=2) in vec2 coord_result_geom;

layout(binding=2) uniform sampler2D diffuse;
layout(binding=3) uniform light_uniform { vec3 light; } light;

layout(location=0) out vec4 result;

void main() {
  vec3 a_to_l = light.light - pos_result_geom;
  float cos_alpha = dot(a_to_l, norm_result_geom) / length(norm_result_geom) / length(a_to_l);
  cos_alpha = cos_alpha > 0 ? cos_alpha : 0;

  result = texture(diffuse, coord_result_geom) * pow(cos_alpha, 2);
}
