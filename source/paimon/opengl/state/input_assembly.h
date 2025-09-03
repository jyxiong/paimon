#pragma once

#include "glad/gl.h"

namespace paimon {
struct InputAssemblyState
{
  bool primitiveRestartEnable = false;
  GLuint primitiveRestartIndex = 0;

  bool operator==(const InputAssemblyState &other) const noexcept = default;
};
} // namespace paimon