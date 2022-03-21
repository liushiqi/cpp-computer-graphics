#include <main_application.hpp>

#include "../include/main_application.hpp"

void main_callback_t::key_callback(GLFWwindow *window, int key, int scan_code, int action, int mode) {
  if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS) {
    main_application::get().close();
  }
}

void main_callback_t::character_callback(GLFWwindow *window, unsigned int codepoint) {}

void main_callback_t::cursor_position_callback(GLFWwindow *window, double x_pos, double y_pos) {}

void main_callback_t::cursor_enter_callback(GLFWwindow *window, int entered) {}

void main_callback_t::mouse_button_callback(GLFWwindow *window, int button, int action, int mods) {}

void main_callback_t::scroll_callback(GLFWwindow *window, double x_offset, double y_offset) {}

void main_callback_t::drop_callback(GLFWwindow *window, int count, const char **paths) {}

main_application::main_application(const std::filesystem::path &assets_path)
    : application(assets_path, 800, 600, std::nullopt, "Main Application", main_callback_t()),
      default_shader(*this, "default") {}

void main_application::main_loop() {
//  default_shader.active();
//  default_shader.inactive();
}
