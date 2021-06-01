#include <base_application.hpp>
#include <csignal>
#include <logger.hpp>
#include <random>
#include <shader.hpp>

class main_callbacks_t : public liu::callbacks_t {};

class main_callbacks_t : public liu::callbacks_t {};

class main_application : public liu::base_application {
public:
  explicit main_application(const std::filesystem::path &assets_path)
      : base_application(assets_path, 800, 600, std::nullopt, "Main Application", std::make_unique<main_callbacks_t>()),
        default_shader(*this, "default") {}

protected:
  void main_loop() override {}

private:
  liu::shader default_shader;
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
