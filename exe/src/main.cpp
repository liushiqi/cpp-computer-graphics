#include <application_t.hpp>
#include <logger.hpp>
#include <shader.hpp>
#include <singleton_t.hpp>
#include <main_application.hpp>

void run(const std::filesystem::path &assets_path) {
  auto app = main_application::init(assets_path);
  app.lock()->run();
}

int main(int argc, const char *argv[]) {
  liu::init_logger();
  liu::init_context();
  std::filesystem::path assets_path = liu::parse_argument(argc, argv);
  run(assets_path);
  liu::clean_context();
  liu::clean_logger();
}
