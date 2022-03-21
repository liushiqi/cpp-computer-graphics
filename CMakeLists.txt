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

set(GRAPHICS_BASE_PATH ${CMAKE_CURRENT_SOURCE_DIR})

find_package(glfw3 3.3 REQUIRED FATAL_ERROR)
find_package(Python3 COMPONENTS Interpreter REQUIRED)

list(APPEND CMAKE_MODULE_PATH ${CMAKE_CURRENT_SOURCE_DIR}/cmake)
include(AddHelperFunctions)
include(AddShader)

file(GLOB_RECURSE LIB_GRAPHICS_SOURCES "src/general/*.cpp" "src/general/*.c")
file(GLOB_RECURSE LIB_GRAPHICS_SOURCES_OPENGL "src/opengl/*.cpp" "src/opengl/*.c")
file(GLOB_RECURSE LIB_GRAPHICS_SOURCES_VULKAN "src/vulkan/*.cpp" "src/vulkan/*.c")
file(GLOB_RECURSE LIB_GRAPHICS_HEADERS "utils/include/*.h")

file(GLOB_RECURSE UTILS_SOURCES "utils/src/*.cpp" "utils/src/*.c")
file(GLOB_RECURSE UTILS_HEADERS "utils/include/*.h")
add_library(utils ${UTILS_SOURCES} ${UTILS_HEADERS})
target_include_directories(utils PUBLIC utils/include)
target_precompile_headers(utils PUBLIC ${UTILS_HEADERS})

add_library(liu_graphics ${LIB_GRAPHICS_SOURCES} ${LIB_GRAPHICS_HEADERS})
if (GRAPHICS_USE_VULKAN)
  target_sources(liu_graphics PUBLIC ${LIB_GRAPHICS_SOURCES_VULKAN})
  target_compile_definitions(liu_graphics PUBLIC VULKAN_ENABLED)
else ()
  target_sources(liu_graphics PUBLIC ${LIB_GRAPHICS_SOURCES_OPENGL})
endif ()
target_compile_definitions(liu_graphics PRIVATE __SOURCE_DIR__="${CMAKE_SOURCE_DIR}")
target_include_directories(liu_graphics PUBLIC ${CMAKE_SOURCE_DIR}/include)
target_link_libraries(liu_graphics PUBLIC
    glfw
    utils
    cxxopts
    glm::glm
    assimp::assimp
    spdlog::spdlog
    ${CMAKE_DL_LIBS})
target_precompile_headers(liu_graphics PUBLIC ${LIB_GRAPHICS_HEADERS})

enable_warning(liu_graphics)

generate_spirv_reflect_binding()

add_subdirectory(exe)

file(MAKE_DIRECTORY "${CMAKE_CURRENT_BINARY_DIR}/logs")
