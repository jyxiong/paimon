#pragma once

#include "glad/gl.h"

namespace paimon {
struct InputAssemblyState
{
  GLenum primitiveTopology = GL_TRIANGLES;
  bool primitiveRestartEnable = false;
  GLuint primitiveRestartIndex = 0;
};
} // namespace paimon