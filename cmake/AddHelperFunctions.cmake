function(append_to_list list_append elements)
  foreach (elem ${elements})
    if (NOT "${list_append}" MATCHES "${elem}")
      list(APPEND ${list_append} ${elem})
    endif ()
  endforeach ()
  set(${list_append} ${${list_append}} PARENT_SCOPE)
endfunction()

function(add_msvc_flags all_flags)
  # Force to always compile with W4
  if ("${all_flags}" MATCHES "/W[0-4]")
    string(REGEX REPLACE "/W[0-4]" "/W4" all_flags "${all_flags}")
  else ()
    list(APPEND all_flags "/W4")
  endif ()
  # Allow non fatal security warnings for msvc 2015 and above
  if (MSVC_VERSION GREATER 1900 AND NOT CMAKE_CXX_COMPILER_ID STREQUAL "Clang")
    append_to_list(all_flags "/Wx")
  endif ()
  set(${all_flags} ${${all_flags}} PARENT_SCOPE)
endfunction()

# Add -Wall -Wextra -Wconversion -Wpedantic
function(add_gcc_flags all_flags)
  append_to_list(${all_flags} "-Wall;-Wextra;-Wconversion;-Wpedantic")
  set(${all_flags} ${${all_flags}} PARENT_SCOPE)
endfunction()

function(enable_sanitizer target_name)
  if (CMAKE_CXX_COMPILER_ID STREQUAL "MSVC")
    message(STATUS "Address sanitizer should be enabled as said in https://devblogs.microsoft.com/cppblog/addresssanitizer-asan-for-windows-with-msvc/")
  elseif (CMAKE_CXX_COMPILER_ID MATCHES "GNU")
    target_compile_options(${target_name} PUBLIC -fsanitize=address,undefined -fno-omit-frame-pointer)
    target_link_options(${target_name} PUBLIC -fsanitize=address,undefined)
    message(STATUS "Enabled address sanitizer for target ${target_name}")
  elseif (CMAKE_CXX_COMPILER_ID MATCHES "Clang")
    if (CMAKE_VERSION LESS 3.15 OR CMAKE_CXX_COMPILER_FRONTEND_VARIANT MATCHES "GNU")
      target_compile_options(${target_name} PUBLIC -fsanitize=address,undefined -fno-omit-frame-pointer)
      target_link_options(${target_name} PUBLIC -fsanitize=address,undefined)
      message(STATUS "Enabled address sanitizer for target ${target_name}")
    elseif (CMAKE_GENERATOR_PLATFORM MATCHES "x86")
      target_compile_options(${target_name} PUBLIC -fsanitize=address)
      target_link_options(${target_name} PUBLIC -fsanitize=address)
      message(STATUS "Enabled address sanitizer for target ${target_name}")
    else ()
      message(STATUS "Could not enable address sanitizer on x64 clang-cl")
    endif ()
  endif ()
endfunction()

function(enable_coverage target_name)
  if (CMAKE_CXX_COMPILER_ID STREQUAL "MSVC")
    message(STATUS "Code coverage could not be enabled in MSVC")
  elseif (CMAKE_CXX_COMPILER_ID STREQUAL "GNU")
    target_compile_options(${target_name} PUBLIC -fprofile-arcs -ftest-coverage --coverage)
    target_link_options(${target_name} PUBLIC -fprofile-arcs -ftest-coverage --coverage)
    message(STATUS "Enabled coverage test for target ${target_name}")
  elseif (CMAKE_CXX_COMPILER_ID STREQUAL "Clang")
    target_compile_options(${target_name} PUBLIC -fprofile-instr-generate -fcoverage-mapping)
    target_link_options(${target_name} PUBLIC -fprofile-instr-generate -fcoverage-mapping)
    message(STATUS "Enabled coverage test for target ${target_name}")
  endif ()
endfunction()

function(enable_warning target_name)
  get_target_property(ALL_FLAGS ${target_name} COMPILE_OPTIONS)
  if ("${ALL_FLAGS}" STREQUAL "ALL_FLAGS-NOTFOUND")
    set(ALL_FLAGS "")
  endif ()
  if (CMAKE_CXX_COMPILER_ID STREQUAL "MSVC")
    add_msvc_flags(ALL_FALGS)
  elseif (CMAKE_CXX_COMPILER_ID STREQUAL "GNU")
    add_gcc_flags(ALL_FLAGS)
  elseif (CMAKE_CXX_COMPILER_ID STREQUAL "Clang")
    if (CMAKE_VERSION LESS "3.15")
      message(STATUS "CMake version prior to 3.15 do not support clang-cl, default to GNU variant clang")
      set(CMAKE_CXX_COMPILER_FRONTEND_VARIANT "GNU")
    endif ()
    if (CMAKE_CXX_COMPILER_FRONTEND_VARIANT STREQUAL "MSVC")
      add_msvc_flags(ALL_FLAGS)
    elseif (CMAKE_CXX_COMPILER_FRONTEND_VARIANT STREQUAL "GNU")
      add_gcc_flags(ALL_FLAGS)
    endif ()
  elseif (CMAKE_CXX_COMPILER_ID STREQUAL "Intel")
    # -w3 -diag-disable:remark
    append_to_list(ALL_FLAGS "-w3;-diag-disable:remark")
  endif ()
  set_target_properties(${target_name} PROPERTIES COMPILE_OPTIONS "${ALL_FLAGS}")
  message(STATUS "Enabled warning for target ${target_name} with flags ${ALL_FLAGS}")
endfunction()

function(add_shader assets_dir target_name shader_name)
  file(GLOB SHADER_SOURCES "${assets_dir}/shaders/source/${shader_name}*")
  file(MAKE_DIRECTORY "${assets_dir}/shaders/binary")

  foreach (SOURCE ${SHADER_SOURCES})
    string(REGEX REPLACE "${assets_dir}/shaders/source/${shader_name}" "${assets_dir}/shaders/binary/${shader_name}" SHADER_FILE_OUTPUT ${SOURCE}.spv)
    if (GRAPHICS_USE_VULKAN)
      set(SHADER_COMPILE_OPTION "--target-env=vulkan1.3")
    else ()
      set(SHADER_COMPILE_OPTION "--target-env=opengl")
    endif ()

    add_custom_command(
        OUTPUT ${SHADER_FILE_OUTPUT}
        COMMAND glslc_exe -fauto-bind-uniforms -fauto-map-locations ${SHADER_COMPILE_OPTION} -o ${SHADER_FILE_OUTPUT} ${SOURCE}
        DEPENDS ${SOURCE} glslc_exe
        VERBATIM
    )

    set_source_files_properties(${SHADER_FILE_OUTPUT} PROPERTIES GENERATED TRUE)

    target_sources(${target_name} PRIVATE ${SHADER_FILE_OUTPUT})

    message(STATUS "Added shader file ${SOURCE}")
  endforeach ()

  message(STATUS "Added shader ${shader_name} to ${target_name}")
endfunction()
