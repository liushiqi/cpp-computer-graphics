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
set(ASSETS_BUILD_DIR "${CMAKE_CURRENT_BINARY_DIR}/assets")
file(GLOB_RECURSE ASSETS_FILES "${ASSETS_DIR}/*")
string(REGEX REPLACE "${ASSETS_DIR}" "${ASSETS_BUILD_DIR}" ASSETS_BUILD_FILES "${ASSETS_FILES}")
add_custom_target(assets_build
    COMMAND ${CMAKE_COMMAND} -E copy_directory ${ASSETS_DIR} ${CMAKE_CURRENT_BINARY_DIR}/assets
    DEPENDS ${ASSETS_FILES}
    BYPRODUCTS ${ASSETS_BUILD_FILES}
    VERBATIM)
set_source_files_properties(${ASSETS_BUILD_FILES} PROPERTIES GENERATED TRUE)
add_dependencies(liu_graphics_main assets_build)

add_shader(${ASSETS_BUILD_DIR} liu_graphics_main default)
