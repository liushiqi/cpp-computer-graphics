function(generate_spirv_reflect_binding)
  set(_spirv_reflect_binding_root "${CMAKE_CURRENT_SOURCE_DIR}/scripts/spirv_reflect")

  add_custom_target(spirv_reflect_libs
      COMMAND ${CMAKE_COMMAND} -E env CC=${CMAKE_C_COMPILER} CXX=${CMAKE_CXX_COMPILER} ${Python3_EXECUTABLE} ${_spirv_reflect_binding_root}/spirv_reflect_build.py
      WORKING_DIRECTORY ${_spirv_reflect_binding_root}
      DEPENDS Python3::Interpreter ${_spirv_reflect_binding_root}/spirv_reflect_build.py
      BYPRODUCTS ${_spirv_reflect_binding_root}/build
      VERBATIM)
endfunction()

function(add_shader assets_dir target_name shader_name)
  file(GLOB SHADER_SOURCES "${assets_dir}/shaders/source/${shader_name}*")
  set(SHADER_GENERATED_PATH "${CMAKE_CURRENT_BINARY_DIR}/generated")
  set(SHADER_BUILD_OUTPUTS "")

  foreach (SOURCE ${SHADER_SOURCES})
    string(REGEX
        REPLACE
        "${assets_dir}/shaders/source/${shader_name}.(glsl|hlsl)"
        "${assets_dir}/shaders/binary/${shader_name}"
        SHADER_BUILD_OUTPUT
        ${SOURCE}.spv)
    string(REGEX MATCH "glsl|hlsl" SHADER_EXT ${SOURCE})
    string(REGEX MATCH "([a-z]*)$" SHADER_STAGE ${SOURCE})

    if (GRAPHICS_USE_VULKAN)
      set(SHADER_COMPILE_OPTION "--target-env=vulkan1.2")
    else ()
      set(SHADER_COMPILE_OPTION "--target-env=opengl")
    endif ()
    if ("${SHADER_EXT}" STREQUAL "glsl")
      list(APPEND SHADER_COMPILE_OPTION -x glsl -std=460core)
    elseif ("${SHADER_EXT}" STREQUAL "hlsl")
      list(APPEND SHADER_COMPILE_OPTION -x hlsl)
    endif ()

    add_custom_target(${shader_name}_${SHADER_STAGE}_binary
        COMMAND ${CMAKE_COMMAND} -E make_directory "${assets_dir}/shaders/binary"
        COMMAND glslc_exe -fauto-bind-uniforms -fauto-map-locations ${SHADER_COMPILE_OPTION} -o ${SHADER_BUILD_OUTPUT} ${SOURCE}
        DEPENDS ${SOURCE} glslc_exe
        BYPRODUCTS ${SHADER_BUILD_OUTPUT}
        VERBATIM)
    set_source_files_properties(${SHADER_BUILD_OUTPUT} PROPERTIES GENERATED TRUE)
    add_dependencies(${target_name} ${shader_name}_${SHADER_STAGE}_binary)
    message(STATUS "Added shader ${shader_name}_${SHADER_STAGE}_binary to target ${target_name}")

    list(APPEND SHADER_BUILD_OUTPUTS ${SHADER_BUILD_OUTPUT})
  endforeach ()

  set(SHADER_GENERATED_HEADER "${SHADER_GENERATED_PATH}/include/${shader_name}_shader_inputs.hpp")
  set(SHADER_GENERATED_SOURCE "${SHADER_GENERATED_PATH}/src/${shader_name}_shader_inputs.cpp")

  add_custom_target(${shader_name}_binding_files
      COMMAND ${CMAKE_COMMAND} -E make_directory "${SHADER_GENERATED_PATH}/src"
      COMMAND ${CMAKE_COMMAND} -E make_directory "${SHADER_GENERATED_PATH}/include"
      COMMAND ${Python3_EXECUTABLE} ${GRAPHICS_BASE_PATH}/scripts/main.py ${SHADER_GENERATED_HEADER} ${SHADER_GENERATED_SOURCE} ${assets_dir}/shaders/binary ${shader_name}
      DEPENDS ${SOURCE} spirv_reflect_libs ${SHADER_BUILD_OUTPUTS}
      BYPRODUCTS ${SHADER_GENERATED_HEADER}
      VERBATIM)
  set_source_files_properties(${SHADER_GENERATED_HEADER} PROPERTIES GENERATED TRUE)
  set_source_files_properties(${SHADER_GENERATED_SOURCE} PROPERTIES GENERATED TRUE)
  add_dependencies(${target_name} ${shader_name}_binding_files)

  target_sources(${target_name} PRIVATE ${SHADER_GENERATED_HEADER} ${SHADER_GENERATED_SOURCE})

  target_include_directories(${target_name} PRIVATE "${SHADER_GENERATED_PATH}/include")
endfunction()
