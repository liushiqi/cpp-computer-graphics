#pragma once

#include <cstdint>

struct binding_definition {
  std::size_t stride;
  std::uint32_t location;
  std::int32_t offset;

};
