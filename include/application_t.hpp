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
std::string vk_error_to_string(VkResult result);
#endif

class shader;

template<typename T>
concept callback_v = requires(int key, int scan_code, int action, int mode) {
  T::key_callback(key, scan_code, action, mode);
}
and requires(unsigned int codepoint) { T::character_callback(codepoint); }
and requires(double x_pos, double y_pos) { T::cursor_position_callback(x_pos, y_pos); }
and requires(int entered) { T::cursor_enter_callback(entered); }
and requires(int button, int action, int mods) { T::mouse_button_callback(button, action, mods); }
and requires(double x_offset, double y_offset) { T::scroll_callback(x_offset, y_offset); }
and requires(int count, const char **paths) { T::drop_callback(count, paths); };

#ifdef VULKAN_ENABLED
struct vulkan_context_t {
  VkInstance instance = nullptr;
  VkPhysicalDevice physical_device = nullptr;
  VkDebugUtilsMessengerEXT debug_messenger = nullptr;
  VkSurfaceKHR surface = nullptr;
  VkDevice device = nullptr;
  std::optional<uint32_t> graphics_family = std::nullopt, present_family = std::nullopt;
  VkQueue graphics_queue = nullptr, present_queue = nullptr;
  VkSwapchainKHR swap_chain = nullptr;
  std::vector<VkImage> swap_chain_images{};
  VkFormat swap_chain_image_format{};
  VkExtent2D swap_chain_extent{};
  std::vector<VkImageView> swap_chain_image_view{};
  VkRenderPass render_pass = nullptr;
  std::vector<VkFramebuffer> swap_chain_frame_buffers;
  VkCommandPool command_pool = nullptr;
  std::vector<VkCommandBuffer> command_buffers{};
  std::vector<VkSemaphore> image_available_semaphores{};
  std::vector<VkSemaphore> render_finished_semaphores{};
  std::vector<VkFence> in_flight_fences{};
  std::vector<VkFence> images_in_flight{};
};
#endif

class base_application_t {
public:
  virtual void run() = 0;

  virtual void close() = 0;

  virtual void resize(uint32_t width, uint32_t height) = 0;

  [[nodiscard]] virtual const std::filesystem::path &get_assets_base_path() const = 0;

#ifdef VULKAN_ENABLED
  virtual const vulkan_context_t &get_vulkan_context() const = 0;

  virtual vulkan_context_t &get_vulkan_context() = 0;
#endif
};

#if defined(NDEBUG) || !defined(VULKAN_ENABLED)
static constexpr bool enable_validation_layers = false;
#else
static constexpr bool enable_validation_layers = true;
#endif

template<callback_v callback_t>
class application_t : public base_application_t {
public:
  application_t(const std::filesystem::path &assets_path, uint32_t width, uint32_t height,
                const std::optional<uint32_t> &max_frame_rate, const std::string &title);

  void run() override;

  void close() override;

  void resize(uint32_t width, uint32_t height) override;

  [[nodiscard]] const std::filesystem::path &get_assets_base_path() const override;

  ~application_t();

#ifdef VULKAN_ENABLED
  [[nodiscard]] const vulkan_context_t &get_vulkan_context() const override;

  vulkan_context_t &get_vulkan_context() override;
#endif

private:
  void create_window();

  void init_context();

  void register_callbacks();

  void clean_context();

protected:
  virtual void main_loop() = 0;

  const std::filesystem::path &assets_base_path;
  const std::optional<uint32_t> max_frame_rate;
  std::unique_ptr<callback_t> callbacks;
  const std::string &title;
  GLFWwindow *window;
  uint32_t width, height;
  std::atomic<bool> should_close;
#ifdef VULKAN_ENABLED
  vulkan_context_t vulkan_context;

  std::uint32_t current_frame = 0;
#else
#endif
};
} // namespace liu
