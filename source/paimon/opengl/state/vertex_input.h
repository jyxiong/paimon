#pragma once

#include <vector>

#include "glad/gl.h"

namespace paimon {
struct VertexInput {
  struct BindingDescription {
    unsigned int
        location; // glEnableVertexArrayAttrib + glVertexArrayAttribFormat
    unsigned int binding; // glVertexArrayAttribBinding
    GLenum format;        // glVertexArrayAttribFormat
    unsigned int offset;  // glVertexArrayAttribFormat
  };

  std::vector<BindingDescription> attributes;
};
} // namespace paimon