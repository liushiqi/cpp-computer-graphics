#ifdef _MSC_VER
#pragma warning(push)
#pragma warning(disable : 4267)
#pragma warning(disable : 4819)
#pragma warning(disable : 26444)
#else
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wconversion"
#endif

#include <base_application.hpp>
#include <cxxopts.hpp>
#include <filesystem>
#include <iostream>

#ifdef _MSC_VER
#pragma warning(pop)
#else
#pragma GCC diagnostic pop
#endif

std::filesystem::path liu::parse_argument(int argc, const char *argv[]) {
  std::filesystem::path assets_path = "assets";
  cxxopts::Options options("computer-graphics", "A program for computer graphics class");
  try {
    options.add_options()("h,help", "Display this help message.")("a,assets", "the path to assets directory.",
                                                                  cxxopts::value<std::filesystem::path>(assets_path));
    auto result = options.parse(argc, argv);
    if (result.count("help")) {
      std::cout << options.help() << std::endl;
      exit(0);
    }
    if (result.count("assets")) {
      assets_path = result["assets"].as<std::filesystem::path>();
    }
  } catch (const cxxopts::OptionSpecException &error) {
    std::cerr << "arguments parse failed: " << error.what() << std::endl << options.help();
    exit(1);
  }
  return assets_path;
}
