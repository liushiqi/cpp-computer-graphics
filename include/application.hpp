#pragma once

#ifdef VULKAN_ENABLED
#include <glad/vulkan.h>
#else
#include <glad/gl.h>
#endif
#include <filesystem>
#include <glfw.h>
#include <memory>

namespace liu {

std::filesystem::path parse_argument(int argc, const char *argv[]);

void init_context();

void clean_context();

class callbacks_t {
protected:
  virtual void key_callback(GLFWwindow *window, int key, [[maybe_unused]] int scan_code, int action, [[maybe_unused]] int mode) {
    if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS) {
      glfwSetWindowShouldClose(window, true);
    }
  }

  virtual void character_callback([[maybe_unused]] GLFWwindow *window, [[maybe_unused]] unsigned int codepoint) {}

  virtual void cursor_position_callback([[maybe_unused]] GLFWwindow *window, [[maybe_unused]] double x_pos, [[maybe_unused]] double y_pos) {}

  virtual void cursor_enter_callback([[maybe_unused]] GLFWwindow *window, [[maybe_unused]] int entered) {}

  virtual void mouse_button_callback([[maybe_unused]] GLFWwindow *window, [[maybe_unused]] int button, [[maybe_unused]] int action,
                                     [[maybe_unused]] int mods) {}

  virtual void scroll_callback([[maybe_unused]] GLFWwindow *window, [[maybe_unused]] double x_offset, [[maybe_unused]] double y_offset) {}

  virtual void drop_callback([[maybe_unused]] GLFWwindow *window, [[maybe_unused]] int count, [[maybe_unused]] const char **paths) {}

private:
  friend class application;
};

class application {
public:
  application(const std::filesystem::path &assets_path, int width, int height, const std::string &title,
              std::unique_ptr<callbacks_t> callbacks = std::make_unique<callbacks_t>());

  void run();

  ~application();

private:
  const std::filesystem::path &assets_base_path;
  std::unique_ptr<callbacks_t> callbacks;
  const std::string &title;
  GLFWwindow *window;
  bool should_close;
#ifdef VULKAN_ENABLED
  VkInstance instance;
  VkPhysicalDevice physical_device;
  VkDebugUtilsMessengerEXT debug_messenger;
#else
#endif

#ifdef NDEBUG
  static constexpr bool enable_validation_layers = false;
#else
  static constexpr bool enable_validation_layers = true;
#endif

  void create_window(int width, int height);

  void init_context();

  void register_callbacks();

  void cleanup_context();

  friend class callbacks_t;

protected:
  virtual void main_loop() = 0;
};
} // namespace liu
