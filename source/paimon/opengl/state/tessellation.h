#pragma once

namespace paimon {

// Similar to VkPipelineTessellationStateCreateInfo
struct TessellationState {
  // Number of control points per patch
  int patchControlPoints = 3;

  bool operator==(const TessellationState &other) const = default;
};

} // namespace paimon