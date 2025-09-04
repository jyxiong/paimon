#pragma once

#include "glad/gl.h"

namespace paimon {
struct DepthState {
  bool depthTestEnable = false;
  bool depthWriteEnable = true;
  GLenum depthCompareOp = GL_LESS;
};
} // namespace paimon