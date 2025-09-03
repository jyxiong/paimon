#pragma once

#include "glad/gl.h"

namespace paimon {
struct DepthState {

  bool depthTest = false;
  bool depthWrite = true;
  GLenum depthFunc = GL_LESS;

  bool operator==(const DepthState &other) const noexcept = default;
};
} // namespace paimon