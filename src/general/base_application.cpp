#include <base_application.hpp>
#include <logger.hpp>
#include <thread>

void liu::base_application::create_window() {
  window = glfwCreateWindow(static_cast<int>(width), static_cast<int>(height), title.c_str(), nullptr, nullptr);
  assert_log(window != nullptr, "GLFW create window failed.");

  glfwMakeContextCurrent(window);

  info("Window create succeeded.");
}

void liu::base_application::register_callbacks() {
  glfwSetWindowUserPointer(window, this);

  glfwSetKeyCallback(window, [](GLFWwindow *local_window, int key, int scan_code, int action, int mode) {
    auto *app = static_cast<base_application *>(glfwGetWindowUserPointer(local_window));
    app->callbacks->key_callback(app, key, scan_code, action, mode);
  });
  glfwSetCharCallback(window, [](GLFWwindow *local_window, unsigned int codepoint) {
    auto *app = static_cast<base_application *>(glfwGetWindowUserPointer(local_window));
    app->callbacks->character_callback(app, codepoint);
  });
  glfwSetCursorPosCallback(window, [](GLFWwindow *local_window, double x_pos, double y_pos) {
    auto *app = static_cast<base_application *>(glfwGetWindowUserPointer(local_window));
    app->callbacks->cursor_position_callback(app, x_pos, y_pos);
  });
  glfwSetCursorEnterCallback(window, [](GLFWwindow *local_window, int entered) {
    auto *app = static_cast<base_application *>(glfwGetWindowUserPointer(local_window));
    app->callbacks->cursor_enter_callback(app, entered);
  });
  glfwSetMouseButtonCallback(window, [](GLFWwindow *local_window, int button, int action, int mods) {
    auto *app = static_cast<base_application *>(glfwGetWindowUserPointer(local_window));
    app->callbacks->mouse_button_callback(app, button, action, mods);
  });
  glfwSetScrollCallback(window, [](GLFWwindow *local_window, double x_offset, double y_offset) {
    auto *app = static_cast<base_application *>(glfwGetWindowUserPointer(local_window));
    app->callbacks->scroll_callback(app, x_offset, y_offset);
  });
  glfwSetDropCallback(window, [](GLFWwindow *local_window, int count, const char **paths) {
    auto *app = static_cast<base_application *>(glfwGetWindowUserPointer(local_window));
    app->callbacks->drop_callback(app, count, paths);
  });

  info("Callbacks register succeeded.");
}

liu::base_application::base_application(const std::filesystem::path &assets_path, uint32_t width, uint32_t height,
                                        const std::optional<uint32_t> &max_frame_rate, const std::string &title,
                                        std::unique_ptr<callbacks_t> callbacks)
    : assets_base_path(assets_path), max_frame_rate(max_frame_rate), callbacks(std::move(callbacks)), title(title), window(nullptr), width(width),
      height(height), should_close(false) {
  create_window();
  init_context();
  register_callbacks();
}

volatile int id;

void liu::base_application::run() {
  auto second_begin = std::chrono::system_clock::now();
  int frame_count = 0;
  while (true) {
    using namespace std::chrono_literals;
    auto start = std::chrono::system_clock::now();

    if (should_close) {
      break;
    }

    main_loop();

    glfwSwapBuffers(window);
    glfwPollEvents();

    frame_count += 1;

    if (max_frame_rate != std::nullopt) {
      uint32_t real_max_frame_rate = max_frame_rate.value();
      if (auto duration = (std::chrono::system_clock::now() - start); duration < (1.0s / real_max_frame_rate)) {
        std::this_thread::sleep_for(1.0s / real_max_frame_rate - duration);
      }
    }
    if (start - second_begin > 1s) {
      info("fps: {}/s", frame_count);
      frame_count = 0;
      second_begin = start;
    }
  }
}

void liu::base_application::close() { should_close = true; }

liu::base_application::~base_application() {
  glfwDestroyWindow(window);
  clean_context();
}
