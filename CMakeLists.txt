cmake_minimum_required(VERSION 3.3)
project(computer_graphics C CXX)

option(GRAPHICS_USE_VULKAN "Use vulkan api in graphics program" OFF)

set(CMAKE_POLICY_DEFAULT_CMP0012 NEW)
set(CMAKE_POLICY_DEFAULT_CMP0076 NEW)

set(CMAKE_CXX_STANDARD 17)

add_subdirectory(external)

find_package(glfw3 REQUIRED)

set(PROGRAM_MAIN_FILE "src/main.cpp")
file(GLOB_RECURSE PROGRAM_SOURCES "src/general/*.cpp" "src/general/*.c")
file(GLOB_RECURSE PROGRAM_SOURCES_OPENGL "src/opengl/*.cpp" "src/opengl/*.c")
file(GLOB_RECURSE PROGRAM_SOURCES_VULKAN "src/vulkan/*.cpp" "src/vulkan/*.c")

function(add_shader TARGET SHADER_NAME)
  find_program(GLSLC glslc)
  file(GLOB SHADER_SOURCES "${CMAKE_CURRENT_SOURCE_DIR}/assets/shaders/source/${SHADER_NAME}*")
  set(SHADER_OUTPUTS "")

  foreach (SOURCE ${SHADER_SOURCES})
    string(REGEX REPLACE "assets/shaders/source/${SHADER_NAME}" "assets/shaders/binary/${SHADER_NAME}" SHADER_FILE_OUTPUT ${SOURCE}.spv)
    add_custom_command(
      OUTPUT ${SHADER_FILE_OUTPUT}
      COMMAND glslc_exe -fauto-bind-uniforms -fauto-map-locations -o ${SHADER_FILE_OUTPUT} ${SOURCE}
      DEPENDS ${SOURCE} glslc_exe
      VERBATIM
    )

    set_source_files_properties(${SHADER_FILE_OUTPUT} PROPERTIES GENERATED TRUE)

    target_sources(${TARGET} PRIVATE ${SHADER_FILE_OUTPUT})

    message(STATUS "Added shader file ${SOURCE}")
  endforeach ()

  message(STATUS "Added shader ${SHADER_NAME}")
endfunction()

add_executable(computer_graphics ${PROGRAM_MAIN_FILE} ${PROGRAM_SOURCES})
if (GRAPHICS_USE_VULKAN)
  target_sources(computer_graphics PUBLIC ${PROGRAM_SOURCES_VULKAN})
  target_link_libraries(computer_graphics PUBLIC vulkan)
  target_compile_definitions(computer_graphics PRIVATE VULKAN_ENABLED)
  file(MAKE_DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}/shaders/binary")
  add_shader(computer_graphics default)
else ()
  target_sources(computer_graphics PUBLIC ${PROGRAM_SOURCES_OPENGL})
endif ()

spdlog_enable_warnings(computer_graphics)
if (CMAKE_BUILD_TYPE STREQUAL "Debug")
  spdlog_enable_sanitizer(computer_graphics)
endif ()

target_compile_definitions(computer_graphics PRIVATE __SOURCE_DIR__="${CMAKE_SOURCE_DIR}")

target_compile_options(computer_graphics PUBLIC -Wall -Wextra)

target_include_directories(computer_graphics PUBLIC ${CMAKE_SOURCE_DIR}/include)
target_include_directories(computer_graphics PUBLIC ${CMAKE_SOURCE_DIR}/glad_include)

target_link_libraries(computer_graphics PUBLIC glfw cxxopts glm::glm assimp::assimp spdlog::spdlog ${CMAKE_DL_LIBS})

file(MAKE_DIRECTORY "${CMAKE_CURRENT_BINARY_DIR}/logs")
