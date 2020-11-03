#version 460 core

in vec3 position1;
in vec3 normal1;
in vec2 coordinate1;

uniform sampler2D diffuse;
uniform vec { vec3 light; };

out vec4 result;
out vec4 color_result;

void main() {
  vec3 a_to_l = light - position1;
  float cos_alpha = dot(a_to_l, normal1) / length(normal1) / length(a_to_l);
  cos_alpha = cos_alpha > 0 ? cos_alpha : 0;

  result = texture(diffuse, coordinate1) * pow(cos_alpha, 2);
  color_result = vec4(1.0f, 1.0f, 1.0f, 1.0f);
}
