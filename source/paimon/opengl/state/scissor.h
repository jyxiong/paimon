#pragma once

#include "glad/gl.h"

namespace paimon {
struct ScissorState {

  bool scissorTestEnable = false;

  int x = 0;
  int y = 0;
  int width = 800;
  int height = 600;
};
} // namespace paimon