#include <application.hpp>
#include <logger.hpp>

 class main_application : public liu::application {
 public:
  explicit main_application(const std::filesystem::path &assets_path) : application(assets_path, 800, 600, "Main Application") {}

 protected:
  void main_loop() override {}
};

 int main(int argc, const char *argv[]) {
  liu::init_logger();
  liu::init_context();
  std::filesystem::path assets_path = liu::parse_argument(argc, argv);
  main_application app(assets_path);
  app.run();
}
