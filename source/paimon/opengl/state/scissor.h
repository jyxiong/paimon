#pragma once

#include <vector>

namespace paimon {
// TODO: support multiple scissors for multiple viewports
struct ScissorRect {
  bool enable = false;
  int x = 0;
  int y = 0;
  int width = 800;
  int height = 600;
  bool operator==(const ScissorRect& other) const = default;
};

struct ScissorState {
  std::vector<ScissorRect> scissors;
};
} // namespace paimon