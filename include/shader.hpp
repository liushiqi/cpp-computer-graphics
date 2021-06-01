#pragma once

#include <base_application.hpp>
#include <filesystem>
#include <functional>
#include <glad/vulkan.h>
#include <map>
#include <optional>
#include <string>
#include <vector>

namespace liu {
enum class shader_type : std::uint8_t {
  VERTEX,
  TESSELLATION_CONTROL,
  TESSELLATION_EVALUATION,
  GEOMETRY,
  FRAGMENT,
  COMPUTE,
  // Provided by VK_KHR_ray_tracing_pipeline not available in OpenGL
  RAYGEN,
  ANY_HIT,
  CLOSEST_HIT,
  MISS,
  INTERSECTION,
  CALLABLE,
  // Provided by VK_NV_mesh_shader
  TASK,
  MESH
};

std::ostream &operator<<(std::ostream &out, const shader_type &type);

std::optional<std::vector<std::uint8_t>> load_shader(const std::filesystem::path &base_path, const std::string &name, const liu::shader_type &type);

class shader {
private:
#ifdef VULKAN_ENABLED
  VkPipelineLayout pipeline_layout = nullptr;
  VkPipeline graphics_pipeline = nullptr;
#else
  std::uint32_t program_id;
#endif
  const liu::base_application &app;
  std::string name;
  std::map<std::string, std::int32_t> attribute_indices;
  std::map<std::string, std::int32_t> uniform_indices;

  void build_indices();

public:
  shader(const liu::base_application &app, const std::string &name);

  ~shader();

  [[nodiscard]] std::int32_t get_attribute_index(const std::string &attribute_name) const;

  [[nodiscard]] std::int32_t get_uniform_index(const std::string &uniform_name) const;

  void active() const;

  static void inactive();

  void apply(const std::function<void()> &callback);

  void apply(const std::function<void()> &callback) const;
};
} // namespace liu
