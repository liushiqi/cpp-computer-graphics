#include <fstream>
#include <iomanip>
#include <iterator>
#include <logger.hpp>
#include <shader.hpp>

std::ostream &liu::operator<<(std::ostream &out, const liu::shader_type &type) {
#define print_string(name)                                                                                             \
  case liu::shader_type::name:                                                                                         \
    out << #name;                                                                                                      \
    break
  switch (type) {
    print_string(VERTEX);
    print_string(TESSELLATION_CONTROL);
    print_string(TESSELLATION_EVALUATION);
    print_string(GEOMETRY);
    print_string(FRAGMENT);
    print_string(COMPUTE);
    // Provided by VK_KHR_ray_tracing_pipeline not available in OpenGL
    print_string(VK_RAY_RAYGEN);
    print_string(VK_RAY_ANY_HIT);
    print_string(VK_RAY_CLOSEST_HIT);
    print_string(VK_RAY_MISS);
    print_string(VK_RAY_INTERSECTION);
    print_string(VK_RAY_CALLABLE);
  }
  return out;
#undef print_string
}

std::string shader_type_string(liu::shader_type type) {
  switch (type) {
  case liu::shader_type::VERTEX:
    return "vert";
  case liu::shader_type::TESSELLATION_CONTROL:
    return "tesc";
  case liu::shader_type::TESSELLATION_EVALUATION:
    return "tese";
  case liu::shader_type::GEOMETRY:
    return "geom";
  case liu::shader_type::FRAGMENT:
    return "frag";
  case liu::shader_type::COMPUTE:
    return "comp";
  case liu::shader_type::VK_RAY_RAYGEN:
    return "rgen";
  case liu::shader_type::VK_RAY_ANY_HIT:
    return "rahit";
  case liu::shader_type::VK_RAY_CLOSEST_HIT:
    return "rchit";
  case liu::shader_type::VK_RAY_MISS:
    return "rmiss";
  case liu::shader_type::VK_RAY_INTERSECTION:
    return "rint";
  case liu::shader_type::VK_RAY_CALLABLE:
    return "rcall";
  }
  return "unknown";
}

std::optional<std::vector<std::uint8_t>> liu::load_shader(const std::filesystem::path &base_path,
                                                          const std::string &name, const liu::shader_type &type) {
  std::filesystem::path shader_path =
      base_path / "shaders" / "binary" / (name + "." + shader_type_string(type) + ".spv");

  if (!std::filesystem::exists(shader_path))
    return std::nullopt;

  info("Loading shader {}", shader_path);
  std::vector<std::uint8_t> shader{};
  std::ifstream shader_file(shader_path, std::ios::in | std::ios::binary);
  shader_file.unsetf(std::ios::skipws);
  std::istream_iterator<std::uint8_t> shader_file_iter(shader_file), file_iter_end;
  std::copy(shader_file_iter, file_iter_end, std::back_inserter(shader));

  return shader;
}
