#include <application.hpp>
#include <logger.hpp>
#include <thread>

void liu::application::create_window() {
  window = glfwCreateWindow(static_cast<int>(width), static_cast<int>(height), title.c_str(), nullptr, nullptr);
  assert_log(window != nullptr, "GLFW create window failed.");

  glfwMakeContextCurrent(window);

  info("Window create succeeded.");
}

void liu::application::run() {
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
      std::uint32_t real_max_frame_rate = max_frame_rate.value();
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

void liu::application::resize(std::uint32_t width, std::uint32_t height) {
  this->width = width;
  this->height = height;
  glfwSetWindowSize(window, static_cast<int>(width), static_cast<int>(height));
}

const std::filesystem::path &liu::application::get_assets_base_path() const { return assets_base_path; }

void liu::application::close() { should_close = true; }

liu::application::application::~application() {
  glfwDestroyWindow(window);
  clean_context();
}
