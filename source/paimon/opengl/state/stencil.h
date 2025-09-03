#pragma once

#include "glad/gl.h"

namespace paimon {
struct StencilState {

  struct StencilOp {
    GLenum failOp = GL_KEEP;
    GLenum passOp = GL_KEEP;
    GLenum depthFailOp = GL_KEEP;
    GLenum compareOp = GL_ALWAYS;
    GLuint compareMask = 0;
    GLuint writeMask = 0;
    GLint reference = 0;

    bool operator==(const StencilOp &) const noexcept = default;
  };

  bool enable = false;
  StencilOp front = {};
  StencilOp back = {};

  bool operator==(const StencilState &other) const noexcept = default;
};
} // namespace paimon