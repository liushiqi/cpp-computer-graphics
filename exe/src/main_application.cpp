#include <default_shader_inputs.hpp>
#include <main_application.hpp>

#if defined(__clang__)
#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wunused-parameter"
#elif defined(__GNUC__) || defined(__GNUG__)
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wunused-parameter"
#endif

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

#if defined(__clang__)
#pragma clang diagnostic pop
#elif defined(__GNUC__) || defined(__GNUG__)
#pragma GCC diagnostic pop
#endif

main_application::main_application(const std::filesystem::path &assets_path)
    : application(assets_path, 800, 600, std::nullopt, "Main Application", main_callback_t()),
      default_shader(*this, "default", default_shader_inputs::get_input_description(),
                     default_shader_uniforms::get_uniform_description()) {}

void main_application::main_loop() {
  //  default_shader.active();
  //  default_shader.inactive();
}
