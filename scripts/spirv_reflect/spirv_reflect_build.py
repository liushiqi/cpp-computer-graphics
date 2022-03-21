# build_test.py

import os

from cffi import FFI

ffi = FFI()

ffi.set_source(
    "build.spirv_reflect_cffi",
    "#include <spirv_reflect.h>",
    libraries=["spirv-reflect-staticd"],
    include_dirs=[os.path.join(os.getcwd(), "../../external/spirv-reflect")],
    library_dirs=[os.path.join(os.environ["CMAKE_CURRENT_BINARY_DIR"], "external/spirv-reflect")]
)

with open(os.path.join(os.path.dirname(__file__), "spirv_reflect_def.h")) as f:
    ffi.cdef(f.read())

if __name__ == "__main__":
    ffi.compile(verbose=True)
