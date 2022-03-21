#version 460 core
#extension GL_ARB_separate_shader_objects : enable
#extension GL_ARB_gpu_shader_fp64 : enable
#extension GL_ARB_gpu_shader_int64 : enable

in flat dvec3 pos_result_geom;
in flat dvec3 norm_result_geom;
in flat dvec2 coord_result_geom;

layout(binding=2) uniform sampler2D diffuse;
layout(binding=3) uniform light_uniform { vec3 light; } light;

out vec4 result;

void main() {
    dvec3 a_to_l = light.light - pos_result_geom;
    double cos_alpha = dot(a_to_l, norm_result_geom) / length(norm_result_geom) / length(a_to_l);
    cos_alpha = cos_alpha > 0 ? cos_alpha : 0;

    result = texture(diffuse, vec2(coord_result_geom)) * pow(float(cos_alpha), 2);
}
