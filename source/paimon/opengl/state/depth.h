#pragma once

#include "glad/gl.h"

namespace paimon {
struct DepthState {

  bool depthTestEnable = false;
  bool depthWriteEnable = true;
  GLenum depthFunc = GL_LESS;

  bool operator==(const DepthState &other) const noexcept = default;
};
} // namespace paimon