#pragma once

#define SPDLOG_LEVEL_NAMES                                                                                                                           \
  { "trace", "debug", "info", "warn", "error", "fatal", "off" }
#define SPDLOG_ACTIVE_LEVEL SPDLOG_LEVEL_TRACE
#ifdef _MSC_VER
#define SPDLOG_FUNCTION __FUNCSIG__
#else
#define SPDLOG_FUNCTION __PRETTY_FUNCTION__
#endif

#ifdef _MSC_VER
#pragma warning(push)
#pragma warning(disable : 26812)
#pragma warning(disable : 26495)
#elif __clang__
#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wsign-conversion"
#endif
// clang-format off
#include <spdlog/spdlog.h>
#include <spdlog/fmt/ostr.h>
// clang-format on
#ifdef _MSC_VER
#pragma warning(pop)
#elif __clang__
#pragma clang diagnostic pop
#endif

#define trace(...)                                                                                                                                   \
  do {                                                                                                                                               \
    SPDLOG_TRACE(__VA_ARGS__);                                                                                                                       \
  } while (false)
#define debug(...)                                                                                                                                   \
  do {                                                                                                                                               \
    SPDLOG_DEBUG(__VA_ARGS__);                                                                                                                       \
  } while (false)
#define info(...)                                                                                                                                    \
  do {                                                                                                                                               \
    SPDLOG_INFO(__VA_ARGS__);                                                                                                                        \
  } while (false)
#define warn(...)                                                                                                                                    \
  do {                                                                                                                                               \
    SPDLOG_WARN(__VA_ARGS__);                                                                                                                        \
  } while (false)
#define error(...)                                                                                                                                   \
  do {                                                                                                                                               \
    SPDLOG_ERROR(__VA_ARGS__);                                                                                                                       \
  } while (false)
#define fatal(...)                                                                                                                                   \
  do {                                                                                                                                               \
    SPDLOG_CRITICAL(__VA_ARGS__);                                                                                                                    \
  } while (false)

namespace liu {

void init_logger();

void clean_logger();

} // namespace liu
