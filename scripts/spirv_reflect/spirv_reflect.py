from os import PathLike
from typing import AnyStr, List

from .build.spirv_reflect_cffi import ffi, lib
from .spirv_enums import SpvReflectFormat, SpvReflectResult, SpvReflectShaderStageFlagBits, SpvReflectDescriptorType


class SpvReflectInterfaceVariable:
    def __init__(self, interface_variable_ptr):
        self._interface_variable_ptr = interface_variable_ptr

    def get_location(self) -> int:
        return self._interface_variable_ptr.location

    def get_format(self) -> SpvReflectFormat:
        return SpvReflectFormat(self._interface_variable_ptr.format)

    def get_name(self) -> AnyStr:
        return ffi.string(self._interface_variable_ptr.name).decode("utf-8")


class SpvReflectDescriptorBinding:
    def __init__(self, descriptor_binding_ptr, stage):
        self._descriptor_binding_ptr = descriptor_binding_ptr
        self.stage = [stage]

    def get_name(self) -> AnyStr:
        return ffi.string(self._descriptor_binding_ptr.name).decode("utf-8")

    def get_binding(self) -> int:
        return self._descriptor_binding_ptr.binding

    def get_descriptor_type(self) -> SpvReflectDescriptorType:
        return SpvReflectDescriptorType(self._descriptor_binding_ptr.descriptor_type)

    def get_count(self) -> int:
        return self._descriptor_binding_ptr.count


class SpvReflectShaderModule:
    def __init__(self, path: str | PathLike[AnyStr]):
        with open(path, "rb") as file:
            self.data = file.read()

        module = ffi.new("SpvReflectShaderModule*")
        result = SpvReflectResult(lib.spvReflectCreateShaderModule(len(self.data), self.data, module))
        if result != SpvReflectResult.SPV_REFLECT_RESULT_SUCCESS:
            raise Exception(
                f"SpvReflectCreateShaderModule failed with error {result}"
            )
        self.module = module

        self._module = ffi.gc(module, lib.spvReflectDestroyShaderModule)

    def get_shader_stage(self) -> SpvReflectShaderStageFlagBits:
        return SpvReflectShaderStageFlagBits(self.module.shader_stage)

    def get_input_variables(self) -> List[SpvReflectInterfaceVariable]:
        input_count = ffi.new("uint32_t*")
        lib.spvReflectEnumerateInputVariables(self._module, input_count, ffi.NULL)
        variables = ffi.new("SpvReflectInterfaceVariable*[]", input_count[0])
        lib.spvReflectEnumerateInputVariables(self._module, input_count, variables)
        return [SpvReflectInterfaceVariable(v) for v in variables]

    def get_output_variables(self) -> List[SpvReflectInterfaceVariable]:
        input_count = ffi.new("uint32_t*")
        lib.spvReflectEnumerateOutputVariables(self._module, input_count, ffi.NULL)
        variables = ffi.new("SpvReflectInterfaceVariable*[]", input_count[0])
        lib.spvReflectEnumerateOutputVariables(self._module, input_count, variables)
        return [SpvReflectInterfaceVariable(v) for v in variables]

    def get_descriptor(self) -> List[SpvReflectDescriptorBinding]:
        input_count = ffi.new("uint32_t*")
        lib.spvReflectEnumerateDescriptorBindings(self._module, input_count, ffi.NULL)
        bindings = ffi.new("SpvReflectDescriptorBinding*[]", input_count[0])
        lib.spvReflectEnumerateDescriptorBindings(self._module, input_count, bindings)
        return [SpvReflectDescriptorBinding(b, self.get_shader_stage()) for b in bindings]
