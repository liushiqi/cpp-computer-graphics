#pragma once

#include <cstdint>
#include <vector>

namespace liu {
enum class input_format : std::uint32_t {
  UINT_X32,
  SINT_X32,
  FLOAT_X32,
  UINT_X32Y32,
  SINT_X32Y32,
  FLOAT_X32Y32,
  UINT_X32Y32Z32,
  SINT_X32Y32Z32,
  FLOAT_X32Y32Z32,
  UINT_X32Y32Z32W32,
  SINT_X32Y32Z32W32,
  FLOAT_X32Y32Z32W32,
  UINT_X64,
  SINT_X64,
  FLOAT_X64,
  UINT_X64Y64,
  SINT_X64Y64,
  FLOAT_X64Y64,
  UINT_X64Y64Z64,
  SINT_X64Y64Z64,
  FLOAT_X64Y64Z64,
  UINT_X64Y64Z64W64,
  SINT_X64Y64Z64W64,
  FLOAT_X64Y64Z64W64
};

#if defined(__clang__)
#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wnull-pointer-subtraction"
#endif

template<typename T, typename U>
constexpr size_t offset_of(U T::*member) {
  return (char *)&((T *)nullptr->*member) - (char *)nullptr;
}

#if defined(__clang__)
#pragma clang diagnostic pop
#endif

struct input_description {
  struct binding_description {
    std::uint32_t location;
    input_format format;
    std::size_t offset;
  };

  std::size_t stride;
  std::vector<binding_description> bindings;
};
} // namespace liu
