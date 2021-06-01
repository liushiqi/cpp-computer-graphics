#pragma once

#ifdef VULKAN_ENABLED
#define VK_ENABLE_BETA_EXTENSIONS
#include <glad/vulkan.h>
#else
#include <glad/gl.h>
#endif
#include <atomic>
#include <filesystem>
#include <glfw.h>
#include <memory>
#include <mutex>
#include <optional>
#include <vector>

namespace liu {

std::filesystem::path parse_argument(int argc, const char *argv[]);

void init_context();

void clean_context();

#ifdef VULKAN_ENABLED
std::string vk_error_to_string(VkResult error);
#endif

class callbacks_t;

class shader;

class base_application {
public:
#ifdef NDEBUG
  static constexpr bool enable_validation_layers = false;
#else
  static constexpr bool enable_validation_layers = true;
#endif

  base_application(const std::filesystem::path &assets_path, uint32_t width, uint32_t height, const std::optional<uint32_t> &max_frame_rate,
                   const std::string &title, std::unique_ptr<callbacks_t> callbacks = std::make_unique<callbacks_t>());

  void run();

  void close();

  void resize(uint32_t width, uint32_t height);

  ~base_application();

private:
  void create_window();

  void init_context();

  void register_callbacks();

  void clean_context();

  friend class callbacks_t;

  friend class shader;

protected:
  virtual void main_loop() = 0;

  const std::filesystem::path &assets_base_path;
  const std::optional<uint32_t> max_frame_rate;
  std::unique_ptr<callbacks_t> callbacks;
  const std::string &title;
  GLFWwindow *window;
  uint32_t width, height;
  std::atomic<bool> should_close;
#ifdef VULKAN_ENABLED
  VkInstance instance = nullptr;
  VkPhysicalDevice physical_device = nullptr;
  VkDebugUtilsMessengerEXT debug_messenger = nullptr;
  VkSurfaceKHR surface = nullptr;
  VkDevice device = nullptr;
  std::optional<uint32_t> graphics_family = std::nullopt, present_family = std::nullopt;
  VkQueue graphics_queue = nullptr, present_queue = nullptr;
  VkSwapchainKHR swap_chain;
  std::vector<VkImage> swap_chain_images;
  VkFormat swap_chain_image_format;
  VkExtent2D swap_chain_extent;
  std::vector<VkImageView> swap_chain_image_view;
  VkRenderPass render_pass;
  std::vector<VkFramebuffer> swap_chain_frame_buffers;
  VkCommandPool command_pool;
  std::vector<VkCommandBuffer> command_buffers;
#else
#endif
};

class callbacks_t {
public:
  virtual ~callbacks_t() = default;

protected:
  virtual void key_callback(base_application *app, int key, [[maybe_unused]] int scan_code, int action, [[maybe_unused]] int mode) {
    if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS) {
      app->close();
    }
  }

  virtual void character_callback([[maybe_unused]] base_application *app, [[maybe_unused]] unsigned int codepoint) {}

  virtual void cursor_position_callback([[maybe_unused]] base_application *app, [[maybe_unused]] double x_pos, [[maybe_unused]] double y_pos) {}

  virtual void cursor_enter_callback([[maybe_unused]] base_application *app, [[maybe_unused]] int entered) {}

  virtual void mouse_button_callback([[maybe_unused]] base_application *app, [[maybe_unused]] int button, [[maybe_unused]] int action,
                                     [[maybe_unused]] int mods) {}

  virtual void scroll_callback([[maybe_unused]] base_application *app, [[maybe_unused]] double x_offset, [[maybe_unused]] double y_offset) {}

  virtual void drop_callback([[maybe_unused]] base_application *app, [[maybe_unused]] int count, [[maybe_unused]] const char **paths) {}

private:
  friend class base_application;
};
} // namespace liu
