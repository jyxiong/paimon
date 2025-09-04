#pragma once

namespace paimon {
struct ViewportState {
  struct ViewportRect {
    int x = 0;
    int y = 0;
    int width = 800;
    int height = 600;
    bool operator==(const ViewportRect& other) const = default;
  };

  struct DepthRange {
    float near = 0.0f;
    float far = 1.0f;
    bool operator==(const DepthRange& other) const = default;
  };
  ViewportRect viewport;
  DepthRange depthRange;
};

} // namespace paimon