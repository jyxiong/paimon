#pragma once

#include <vector>

namespace paimon {
struct ViewportRect {
  int x = 0;
  int y = 0;
  int width = 800;
  int height = 600;
  float minDepth = 0.0f;
  float maxDepth = 1.0f;
  bool operator==(const ViewportRect& other) const = default;
};

struct ViewportState {
  std::vector<ViewportRect> viewports;
};
} // namespace paimon