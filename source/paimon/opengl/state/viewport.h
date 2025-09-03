#pragma once

#include <vector>

#include "glad/gl.h"

namespace paimon {
// TODO: support multiple viewports
struct ViewportRect {
  int x = 0;
  int y = 0;
  int width = 800;
  int height = 600;
  float minDepth = 0.0f;
  float maxDepth = 1.0f;
  bool operator==(const ViewportRect& other) const {
    return x == other.x && y == other.y && width == other.width && height == other.height &&
           minDepth == other.minDepth && maxDepth == other.maxDepth;
  }
  bool operator!=(const ViewportRect& other) const {
    return !(*this == other);
  }
};

struct ViewportState {
  std::vector<ViewportRect> viewports;
};
} // namespace paimon