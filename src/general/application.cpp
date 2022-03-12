#include <application_t.hpp>
#include <logger.hpp>
#include <thread>

template<liu::callback_v callback_t>
void liu::application_t<callback_t>::create_window() {
  window = glfwCreateWindow(static_cast<int>(width), static_cast<int>(height), title.c_str(), nullptr, nullptr);
  assert_log(window != nullptr, "GLFW create window failed.");

  glfwMakeContextCurrent(window);

  info("Window create succeeded.");
}

template<liu::callback_v callback_t>
void liu::application_t<callback_t>::register_callbacks() {
  glfwSetKeyCallback(window, callback_t::key_callback);
  glfwSetCharCallback(window, callback_t::character_callback);
  glfwSetCursorPosCallback(window, callback_t::cursor_position_callback);
  glfwSetCursorEnterCallback(window, callback_t::cursor_enter_callback);
  glfwSetMouseButtonCallback(window, callback_t::mouse_button_callback);
  glfwSetScrollCallback(window, callback_t::scroll_callback);
  glfwSetDropCallback(window, callback_t::drop_callback);
  info("Callbacks register succeeded.");
}

template<liu::callback_v callback_t>
liu::application_t<callback_t>::application_t(const std::filesystem::path &assets_path, uint32_t width, uint32_t height,
                                              const std::optional<uint32_t> &max_frame_rate, const std::string &title)
    : assets_base_path(assets_path), max_frame_rate(max_frame_rate), callbacks(std::move(callbacks)), title(title),
      window(nullptr), width(width), height(height), should_close(false) {
  create_window();
  init_context();
  register_callbacks();
}

template<liu::callback_v callback_t>
void liu::application_t<callback_t>::run() {
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

template<liu::callback_v callback_t>
void liu::application_t<callback_t>::resize(uint32_t width, uint32_t height) {}

template<liu::callback_v callback_t>
const std::filesystem::path &liu::application_t<callback_t>::get_assets_base_path() const {
  return assets_base_path;
}

template<liu::callback_v callback_t>
void liu::application_t<callback_t>::close() {
  should_close = true;
}

template<liu::callback_v callback_t>
liu::application_t<callback_t>::application_t::~application_t() {
  glfwDestroyWindow(window);
  clean_context();
}
