#include <main_application.hpp>

#include "../include/main_application.hpp"

void main_callback_t::key_callback(int key, int scan_code, int action, int mode) {}

void main_callback_t::character_callback(unsigned int codepoint) {}

void main_callback_t::cursor_position_callback(double x_pos, double y_pos) {}

void main_callback_t::cursor_enter_callback(int entered) {}

void main_callback_t::mouse_button_callback(int button, int action, int mods) {}

void main_callback_t::scroll_callback(double x_offset, double y_offset) {}

void main_callback_t::drop_callback(int count, const char **paths) {}

main_application::main_application(const std::filesystem::path &assets_path)
    : application_t(assets_path, 800, 600, std::nullopt, "Main Application", std::make_unique<main_callback_t>()),
      default_shader(*this, "default") {}

void main_application::main_loop() {}
