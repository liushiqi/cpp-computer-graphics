import glob

import sys

from spirv_reflect.spirv_reflect import SpvReflectShaderModule, SpvReflectShaderStageFlagBits

if __name__ == '__main__':
    if len(sys.argv) != 5:
        print("Usage: main.py <output_header> <output_source> <shader_path> <shader_name>")
        sys.exit(1)
    shader_path = sys.argv[3]
    shader_name = sys.argv[4]
    modules = []
    input_variables = []
    descriptor_bindings = []
    for module_path in glob.glob(shader_path + "/" + shader_name + ".*.spv"):
        module = SpvReflectShaderModule(module_path)
        modules.append(module)
        if module.get_shader_stage() == SpvReflectShaderStageFlagBits.SPV_REFLECT_SHADER_STAGE_VERTEX_BIT:
            input_variables += module.get_input_variables()
        descriptor_bindings += module.get_descriptor()

    with open(sys.argv[1], 'w+') as output_header, open(sys.argv[2], 'w+') as output_source:
        output_header.write("""#pragma once

#include <cstdint>
#include <shader_input.hpp>

#if defined(__clang__)
#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wdeprecated-volatile"
#elif defined(__GNUC__) || defined(__GNUG__)
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wvolatile"
#endif
#include <glm/glm.hpp>
#if defined(__clang__)
#pragma clang diagnostic pop
#elif defined(__GNUC__) || defined(__GNUG__)
#pragma GCC diagnostic pop
#endif

""")
        output_header.write(f"struct {shader_name}_shader_inputs final {{\n")
        for variable in input_variables:
            output_header.write(f"  {variable.get_format().c_type} {variable.get_name()};\n")
        output_header.write("\n")
        output_header.write("  static liu::shader_input get_input_description();\n")
        output_header.write("};\n")

        output_header.write(f"struct {shader_name}_shader_uniforms final {{\n")
        for uniform in descriptor_bindings:
            print(f"{uniform.get_name()} {uniform.tmp()}\n")
        output_header.write("};\n")

        output_source.write(f"#include <{shader_name}_shader_inputs.hpp>\n\n")
        output_source.write(
            f"liu::shader_input {shader_name}_shader_inputs::get_input_description() {{\n")
        output_source.write("  liu::shader_input description;\n")
        output_source.write(f"  description.stride = sizeof(struct {shader_name}_shader_inputs);\n")
        for variable in input_variables:
            output_source.write(
                f"  description.bindings.emplace_back(liu::input_description::binding_description{{\n"
                f"    .location = {variable.get_location()},\n"
                f"    .format = {variable.get_format().cpp_name},\n"
                f"    .offset = liu::offset_of(&{shader_name}_shader_inputs::{variable.get_name()})\n"
                f"  }});\n")
        output_source.write("  return description;\n")
        output_source.write("}\n")
