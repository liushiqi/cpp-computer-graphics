#pragma once

#include <application_t.hpp>
#include <shader.hpp>
#include <singleton_t.hpp>

class main_callback_t {
public:
  static void key_callback(GLFWwindow *window, int key, int scan_code, int action, int mode);

  static void character_callback(GLFWwindow *window, unsigned int codepoint);

  static void cursor_position_callback(GLFWwindow *window, double x_pos, double y_pos);

  static void cursor_enter_callback(GLFWwindow *window, int entered);

  static void mouse_button_callback(GLFWwindow *window, int button, int action, int mods);

  static void scroll_callback(GLFWwindow *window, double x_offset, double y_offset);

  static void drop_callback(GLFWwindow *window, int count, const char **paths);
};

class main_application : public liu::application_t, public liu::singleton_t<main_application> {
protected:
  explicit main_application(const std::filesystem::path &assets_path);

  void main_loop() override;

private:
  liu::shader default_shader;

  friend class main_callback_t;
  friend class liu::singleton_t<main_application>;
};
