cmake_minimum_required(VERSION 3.11)
project(computer_graphics C CXX)

include(CheckCXXCompilerFlag)

option(GRAPHICS_USE_VULKAN "Use vulkan api in graphics program" OFF)

set(CMAKE_POLICY_DEFAULT_CMP0012 NEW)
set(CMAKE_POLICY_DEFAULT_CMP0054 NEW)
set(CMAKE_POLICY_DEFAULT_CMP0076 NEW)
set(CMAKE_POLICY_DEFAULT_CMP0077 NEW)

set(CMAKE_CXX_STANDARD 20)

add_subdirectory(external)

find_package(glfw3 3.3 REQUIRED FATAL_ERROR)

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

function(add_shader target_name shader_name)
  file(GLOB SHADER_SOURCES "${CMAKE_CURRENT_SOURCE_DIR}/assets/shaders/source/${shader_name}*")

  foreach (SOURCE ${SHADER_SOURCES})
    string(REGEX REPLACE "assets/shaders/source/${shader_name}" "assets/shaders/binary/${shader_name}" SHADER_FILE_OUTPUT ${SOURCE}.spv)
    add_custom_command(
        OUTPUT ${SHADER_FILE_OUTPUT}
        COMMAND glslc_exe -fauto-bind-uniforms -fauto-map-locations -o ${SHADER_FILE_OUTPUT} ${SOURCE}
        DEPENDS ${SOURCE} glslc_exe
        VERBATIM
    )

    set_source_files_properties(${SHADER_FILE_OUTPUT} PROPERTIES GENERATED TRUE)

    target_sources(${target_name} PRIVATE ${SHADER_FILE_OUTPUT})

    message(STATUS "Added shader file ${SOURCE}")
  endforeach ()

  message(STATUS "Added shader ${shader_name} to ${target_name}")
endfunction()

file(GLOB_RECURSE LIB_GRAPHICS_SOURCES "src/general/*.cpp" "src/general/*.c")
file(GLOB_RECURSE LIB_GRAPHICS_SOURCES_OPENGL "src/opengl/*.cpp" "src/opengl/*.c")
file(GLOB_RECURSE LIB_GRAPHICS_SOURCES_VULKAN "src/vulkan/*.cpp" "src/vulkan/*.c")

file(GLOB_RECURSE UTILS_SOURCES "utils/src/*.cpp" "utils/src/*.c")
file(GLOB_RECURSE UTILS_HEADERS "utils/include/*.h")
add_library(utils ${UTILS_SOURCES})
target_include_directories(utils PUBLIC utils/include)
target_precompile_headers(utils PUBLIC ${UTILS_HEADERS})

add_library(graphics ${LIB_GRAPHICS_SOURCES})
if (GRAPHICS_USE_VULKAN)
  target_sources(graphics PUBLIC ${LIB_GRAPHICS_SOURCES_VULKAN})
  target_compile_definitions(graphics PUBLIC VULKAN_ENABLED)
  file(MAKE_DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}/assets/shaders/binary")
else ()
  target_sources(graphics PUBLIC ${LIB_GRAPHICS_SOURCES_OPENGL})
endif ()

enable_warning(graphics)
if (CMAKE_BUILD_TYPE STREQUAL "Debug")
  if (NOT GRAPHICS_USE_VULKAN)
    # disable sanitizer on vulkan because vulkan itself have memory leaks.
    enable_sanitizer(graphics)
  endif ()
  enable_coverage(graphics)
endif ()

target_compile_definitions(graphics PRIVATE __SOURCE_DIR__="${CMAKE_SOURCE_DIR}")
target_include_directories(graphics PUBLIC ${CMAKE_SOURCE_DIR}/include)
target_link_libraries(graphics PUBLIC
    glfw
    utils
    cxxopts
    glm::glm
    assimp::assimp
    spdlog::spdlog
    ${CMAKE_DL_LIBS})

file(GLOB_RECURSE BIN_GRAPHICS_SOURCES "exe/src/*.cpp" "exe/src/*.c")
add_executable(display ${BIN_GRAPHICS_SOURCES})
target_link_libraries(display PUBLIC graphics)
target_include_directories(display PUBLIC exe/include)

add_shader(display default)

file(MAKE_DIRECTORY "${CMAKE_CURRENT_BINARY_DIR}/logs")
