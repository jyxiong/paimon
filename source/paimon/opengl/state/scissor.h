#pragma once

namespace paimon {
struct ScissorRect {
  int x = 0;
  int y = 0;
  int width = 800;
  int height = 600;
  bool operator==(const ScissorRect& other) const = default;
};

struct ScissorState
{
  bool enable = false;
  ScissorRect scissor;
};
} // namespace paimon