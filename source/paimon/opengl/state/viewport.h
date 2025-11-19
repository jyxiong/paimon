#pragma once

#include <vector>

namespace paimon {

// Similar to VkViewport
struct Viewport {
  float x = 0.0f;
  float y = 0.0f;
  float width = 800.0f;
  float height = 600.0f;
  float minDepth = 0.0f;
  float maxDepth = 1.0f;

  bool operator==(const Viewport &other) const = default;
};

// Similar to VkRect2D
struct Scissor {
  int x = 0;
  int y = 0;
  int width = 800;
  int height = 600;

  bool operator==(const Scissor &other) const = default;
};

// Similar to VkPipelineViewportStateCreateInfo
// In Vulkan, viewport and scissor are in the same state
struct ViewportState {
  // Multiple viewports support
  std::vector<Viewport> viewports;
  
  // Multiple scissors support (matches viewport count)
  std::vector<Scissor> scissors;
  
  // Default constructor with one viewport and one scissor
  ViewportState() {
    viewports.push_back(Viewport{});
    scissors.push_back(Scissor{});
  }

  bool operator==(const ViewportState &other) const = default;
};

} // namespace paimon