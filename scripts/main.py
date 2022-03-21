import glob

import sys

from spirv_reflect.spirv_reflect import SpvReflectShaderModule, SpvReflectShaderStageFlagBits

if __name__ == '__main__':
    if len(sys.argv) != 4:
        print("Usage: main.py <output_filename> <shader_path> <shader_name>")
        sys.exit(1)
    shader_path = sys.argv[2]
    shader_name = sys.argv[3]
    with open(sys.argv[1], 'w+') as output_file:
        output_file.write("#pragma once\n\n#include <cstdint>\n#include <glm/glm.hpp>\n\n")
        for module_path in glob.glob(shader_path + "/" + shader_name + ".*.spv"):
            module = SpvReflectShaderModule(module_path)
            if module.get_shader_stage() == SpvReflectShaderStageFlagBits.SPV_REFLECT_SHADER_STAGE_VERTEX_BIT:
                output_file.write(f"struct {shader_name}_inputs {{\n")
                variables = module.get_input_variables()
                for variable in variables:
                    output_file.write(f"  {variable.get_format().c_type} {variable.get_name()};\n")
                output_file.write("};\n")
