#include <base_application.hpp>
#include <logger.hpp>

class main_callbacks_t : public liu::callbacks_t {};

class main_application : public liu::base_application {
public:
  explicit main_application(const std::filesystem::path &assets_path)
      : base_application(assets_path, 800, 600, "Main Application", std::make_unique<main_callbacks_t>()) {}

protected:
  void main_loop() override {}
};

void run(const std::filesystem::path &assets_path) {
  main_application app(assets_path);
  app.run();
}

int main(int argc, const char *argv[]) {
  liu::init_logger();
  liu::init_context();
  std::filesystem::path assets_path = liu::parse_argument(argc, argv);
  run(assets_path);
  liu::clean_context();
  liu::clean_logger();
}
