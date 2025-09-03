#pragma once


#include <vector>
#include "glad/gl.h"

namespace paimon {
// TODO: support multiple scissors for multiple viewports
struct ScissorRect {
  int x = 0;
  int y = 0;
  int width = 800;
  int height = 600;
  bool operator==(const ScissorRect& other) const {
    return x == other.x && y == other.y && width == other.width && height == other.height;
  }
  bool operator!=(const ScissorRect& other) const {
    return !(*this == other);
  }
};

struct ScissorState {
  bool scissorTestEnable = false;
  std::vector<ScissorRect> scissors;
};
} // namespace paimon