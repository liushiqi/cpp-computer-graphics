#include <application.hpp>
#include <logger.hpp>
#include <spdlog/fmt/chrono.h>
#include <thread>

void liu::application::create_window(int width, int height) {
  window = glfwCreateWindow(width, height, title.c_str(), nullptr, nullptr);
  if (!window) {
    fatal("GLFW create window failed.");
    throw std::runtime_error("GLFW create window failed.");
  }

  glfwMakeContextCurrent(window);

  info("Window create succeeded.");
}

void liu::application::register_callbacks() {
  glfwSetWindowUserPointer(window, this);

  glfwSetKeyCallback(window, [](GLFWwindow *local_window, int key, int scan_code, int action, int mode) {
    auto *app = static_cast<application *>(glfwGetWindowUserPointer(local_window));
    app->callbacks->key_callback(local_window, key, scan_code, action, mode);
  });
  glfwSetCharCallback(window, [](GLFWwindow *local_window, unsigned int codepoint) {
    auto *app = static_cast<application *>(glfwGetWindowUserPointer(local_window));
    app->callbacks->character_callback(local_window, codepoint);
  });
  glfwSetCursorPosCallback(window, [](GLFWwindow *local_window, double x_pos, double y_pos) {
    auto *app = static_cast<application *>(glfwGetWindowUserPointer(local_window));
    app->callbacks->cursor_position_callback(local_window, x_pos, y_pos);
  });
  glfwSetCursorEnterCallback(window, [](GLFWwindow *local_window, int entered) {
    auto *app = static_cast<application *>(glfwGetWindowUserPointer(local_window));
    app->callbacks->cursor_enter_callback(local_window, entered);
  });
  glfwSetMouseButtonCallback(window, [](GLFWwindow *local_window, int button, int action, int mods) {
    auto *app = static_cast<application *>(glfwGetWindowUserPointer(local_window));
    app->callbacks->mouse_button_callback(local_window, button, action, mods);
  });
  glfwSetScrollCallback(window, [](GLFWwindow *local_window, double x_offset, double y_offset) {
    auto *app = static_cast<application *>(glfwGetWindowUserPointer(local_window));
    app->callbacks->scroll_callback(local_window, x_offset, y_offset);
  });
  glfwSetDropCallback(window, [](GLFWwindow *local_window, int count, const char **paths) {
    auto *app = static_cast<application *>(glfwGetWindowUserPointer(local_window));
    app->callbacks->drop_callback(local_window, count, paths);
  });

  info("Callbacks register succeeded.");
}

liu::application::application(const std::filesystem::path &assets_path, int width, int height, const std::string &title,
                              std::unique_ptr<callbacks_t> callbacks)
    : assets_base_path(assets_path), callbacks(std::move(callbacks)), title(title), window(nullptr), should_close(false) {
  create_window(width, height);
  init_context();
  register_callbacks();
}

void liu::application::run() {
  auto second_begin = std::chrono::system_clock::now();
  int frame_count = 0;
  while (!glfwWindowShouldClose(window)) {
    using namespace std::chrono_literals;
    auto start = std::chrono::system_clock::now();
    main_loop();

    glfwSwapBuffers(window);
    glfwPollEvents();

    frame_count += 1;

    if (auto duration = (std::chrono::system_clock::now() - start); duration < (1.0s / 60)) {
      std::this_thread::sleep_for(1.0s / 60 - duration);
    }
    if (start - second_begin > 1s) {
      info("fps: {}/s", frame_count);
      frame_count = 0;
      second_begin = start;
    }
  }
}

liu::application::~application() {
  glfwDestroyWindow(window);
  cleanup_context();
}
