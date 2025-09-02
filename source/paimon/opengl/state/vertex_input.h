#pragma once

#include <vector>

#include "glad/gl.h"

namespace paimon {
struct VertexInput {
  class Attribute {
  public:
    unsigned int layout_location{0};
    std::size_t binding;
    std::size_t stride;
    GLint format;
    std::size_t offset;
    // std::string name;
  };

  class Binding {
  public:
    std::size_t binding;
    std::size_t stride;
    unsigned int divisor;
  };

  std::vector<Attribute> attributes;
  std::vector<Binding> bindings;

  bool operator==(const VertexInput &other) const noexcept = default;
};
} // namespace paimon