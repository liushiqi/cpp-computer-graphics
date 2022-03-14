#pragma once

#include <application_t.hpp>
#include <filesystem>
#include <functional>
#include <glad/vulkan.h>
#include <map>
#include <optional>
#include <string>
#include <vector>

namespace liu {
enum class shader_group : std::uint8_t {

};

enum class shader_type : std::uint8_t {
  VERTEX,
  TESSELLATION_CONTROL,
  TESSELLATION_EVALUATION,
  GEOMETRY,
  FRAGMENT,
  COMPUTE,
  // Provided by VK_KHR_ray_tracing_pipeline not available in OpenGL
  VK_RAY_RAYGEN,
  VK_RAY_ANY_HIT,
  VK_RAY_CLOSEST_HIT,
  VK_RAY_MISS,
  VK_RAY_INTERSECTION,
  VK_RAY_CALLABLE,
};

constexpr liu::shader_type all_shader_types[] = {

    liu::shader_type::VERTEX,
    liu::shader_type::TESSELLATION_CONTROL,
    liu::shader_type::TESSELLATION_EVALUATION,
    liu::shader_type::GEOMETRY,
    liu::shader_type::FRAGMENT,
    liu::shader_type::COMPUTE,
    // Provided by VK_KHR_ray_tracing_pipeline not available in OpenGL
    liu::shader_type::VK_RAY_RAYGEN,
    liu::shader_type::VK_RAY_ANY_HIT,
    liu::shader_type::VK_RAY_CLOSEST_HIT,
    liu::shader_type::VK_RAY_MISS,
    liu::shader_type::VK_RAY_INTERSECTION,
    liu::shader_type::VK_RAY_CALLABLE,
};

std::ostream &operator<<(std::ostream &out, const shader_type &type);

std::optional<std::vector<std::uint8_t>> load_shader(const std::filesystem::path &base_path, const std::string &name,
                                                      const liu::shader_type &type);

class shader {
private:
#ifdef VULKAN_ENABLED
  VkPipelineLayout pipeline_layout = nullptr;
  VkPipeline graphics_pipeline = nullptr;
#else
  std::uint32_t program_id{};
#endif
  const liu::base_application_t &app;
  std::string name;
  std::map<std::string, std::uint32_t> attribute_indices;
  std::map<std::string, std::uint32_t> uniform_indices;

public:
  shader(const liu::base_application_t &app, const std::string &name);

  ~shader();

  [[nodiscard]] std::optional<std::int32_t> get_attribute_index(const std::string &attribute_name) const;

  [[nodiscard]] std::optional<std::int32_t> get_uniform_index(const std::string &uniform_name) const;

  void active() const;

  void inactive() const;

  void apply(std::function<void()> &callback) const;
};
} // namespace liu
