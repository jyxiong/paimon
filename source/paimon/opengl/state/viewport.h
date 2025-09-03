#pragma once

#include <vector>

#include "glad/gl.h"

namespace paimon {
struct ViewportState {
  int x = 0;
  int y = 0;
  int width = 800;
  int height = 600;
  float minDepth = 0.0f;
  float maxDepth = 1.0f;
};
} // namespace paimon