cmake_minimum_required(VERSION 3.11)

file(GLOB_RECURSE BIN_GRAPHICS_SOURCES "src/*.cpp" "src/*.c")
file(GLOB_RECURSE BIN_GRAPHICS_HEADERS "include/*.hpp" "include/*.h")
add_executable(liu_graphics_main ${BIN_GRAPHICS_SOURCES} ${BIN_GRAPHICS_HEADERS})
target_link_libraries(liu_graphics_main PUBLIC liu_graphics)
target_include_directories(liu_graphics_main PUBLIC include)
target_precompile_headers(liu_graphics_main PRIVATE ${BIN_GRAPHICS_HEADERS})

enable_warning(liu_graphics_main)
if (CMAKE_BUILD_TYPE STREQUAL "Debug")
  enable_sanitizer(liu_graphics_main)
  enable_coverage(liu_graphics_main)
endif ()

set(ASSETS_DIR "${CMAKE_CURRENT_SOURCE_DIR}/assets")
file(COPY ${ASSETS_DIR} DESTINATION ${CMAKE_CURRENT_BINARY_DIR})
add_shader(${CMAKE_CURRENT_BINARY_DIR}/assets liu_graphics_main default)