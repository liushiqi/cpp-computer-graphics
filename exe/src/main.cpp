#include <application_t.hpp>
#include <logger.hpp>
#include <main_application.hpp>
#include <shader.hpp>
#include <singleton_t.hpp>

void run(const std::filesystem::path &assets_path) {
  main_application::init(assets_path).run();
}

int main(int argc, const char *argv[]) {
  liu::init_logger();
  liu::init_context();
  std::filesystem::path assets_path = liu::parse_argument(argc, argv);
  run(assets_path);
  liu::clean_context();
  liu::clean_logger();
}
