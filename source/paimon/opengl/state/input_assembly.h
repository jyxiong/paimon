#pragma once

#include "glad/gl.h"

namespace paimon {
struct InputAssemblly
{
  GLenum topology = GL_TRIANGLES;
  bool primitiveRestartEnable = false;

  bool operator==(const InputAssemblly &other) const noexcept = default;
};
} // namespace paimon