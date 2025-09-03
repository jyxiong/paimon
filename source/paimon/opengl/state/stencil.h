#pragma once

#include "glad/gl.h"

namespace paimon {
struct StencilState {

  struct StencilOp {
    struct StencilFunction {
      GLenum func = GL_ALWAYS;
      GLuint mask = 0;
      GLint ref = 0;

      bool operator==(const StencilFunction &) const noexcept = default;
    };

    struct StencilOperation {
      GLenum failOp = GL_KEEP;
      GLenum passOp = GL_KEEP;
      GLenum depthFailOp = GL_KEEP;

      bool operator==(const StencilOperation &) const noexcept = default;
    };

    GLenum failOp = GL_KEEP;
    GLenum passOp = GL_KEEP;
    GLenum depthFailOp = GL_KEEP;


    GLuint writeMask = 0;


    bool operator==(const StencilOp &) const noexcept = default;
  };

  bool enable = false;
  StencilOp front = {};
  StencilOp back = {};
};
} // namespace paimon