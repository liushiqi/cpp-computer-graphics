function(_generate_spirv_reflect_binding)
  add_custom_command()
endfunction()

function(add_shader assets_dir target_name shader_name)
  file(GLOB SHADER_SOURCES "${assets_dir}/shaders/source/${shader_name}*")

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
    add_dependencies(${target_name} ${shader_name}_${SHADER_STAGE}_binary)
    message(STATUS "Added shader ${shader_name}_${SHADER_STAGE}_binary to target ${target_name}")

    set_source_files_properties(${SHADER_BUILD_OUTPUT} PROPERTIES GENERATED TRUE)

    target_sources(${target_name} PRIVATE ${SHADER_BUILD_OUTPUT})
  endforeach ()
endfunction()
