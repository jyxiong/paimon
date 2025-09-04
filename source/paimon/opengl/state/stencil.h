#pragma once

#include <unordered_map>

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
  };

  bool enable = false;
  std::unordered_map<GLenum, StencilOp> faceOps;
};
} // namespace paimon