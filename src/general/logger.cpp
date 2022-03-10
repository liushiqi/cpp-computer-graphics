#include <filesystem>
#include <iostream>
#include <logger.hpp>
#include <spdlog/common.h>
#include <spdlog/pattern_formatter.h>
#include <spdlog/sinks/rotating_file_sink.h>
#include <spdlog/sinks/stdout_color_sinks.h>

class simple_file_path_formatter_flag : public spdlog::custom_flag_formatter {
public:
  void format(const spdlog::details::log_msg &msg, const std::tm &, spdlog::memory_buf_t &dest) override {
    if (msg.source.empty()) {
      return;
    }

    std::filesystem::path prefix(__SOURCE_DIR__ "");
    std::filesystem::path file_name = std::filesystem::absolute(msg.source.filename).lexically_relative(prefix);
    std::string file = file_name.string();

    std::string source_line = std::to_string(msg.source.line);

    dest.append(file.data(), file.data() + file.length());
    dest.push_back(':');
    dest.append(source_line.data(), source_line.data() + source_line.length());
  }

  [[nodiscard]] std::unique_ptr<custom_flag_formatter> clone() const override {
    return std::make_unique<simple_file_path_formatter_flag>();
  }
};

void liu::init_logger() {
  auto console_sink = std::make_shared<spdlog::sinks::stdout_color_sink_mt>();
  console_sink->set_level(spdlog::level::info);
  auto formatter = std::make_unique<spdlog::pattern_formatter>();
  formatter->add_flag<simple_file_path_formatter_flag>('*').set_pattern("%^[%H:%M:%S] [%8l] [%*] [%!]%$ %v");
  console_sink->set_formatter(std::move(formatter));

  auto file_sink = std::make_shared<spdlog::sinks::rotating_file_sink_mt>("logs/trace.log", 10 * 1024 * 1024, 64, true);
  formatter = std::make_unique<spdlog::pattern_formatter>();
  formatter->add_flag<simple_file_path_formatter_flag>('*').set_pattern("[%Y-%m-%d %H:%M:%S.%e] [%8l] [%*] [%!] %v");
  file_sink->set_formatter(std::move(formatter));
  file_sink->set_level(spdlog::level::trace);

  std::shared_ptr<spdlog::logger> logger =
      std::make_shared<spdlog::logger>("homework", spdlog::sinks_init_list({console_sink, file_sink}));
  spdlog::set_default_logger(logger);
  spdlog::set_level(spdlog::level::trace);
  spdlog::flush_on(spdlog::level::warn);
}

void liu::clean_logger() { spdlog::shutdown(); }
