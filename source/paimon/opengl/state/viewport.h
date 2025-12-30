#pragma once

#include <vector>

#include <glad/gl.h>

namespace paimon {

// Similar to VkPipelineViewportStateCreateInfo
// In Vulkan, viewport and scissor are in the same state
struct ViewportState {
  // Similar to VkViewport
  struct Viewport {
    float x = 0.0f;
    float y = 0.0f;
    float width = 0.0f;
    float height = 0.0f;
    float minDepth = 0.0f;
    float maxDepth = 1.0f;

    bool operator==(const Viewport &other) const = default;
  };

  // Similar to VkRect2D
  struct Scissor {
    int x = 0;
    int y = 0;
    int width = 0;
    int height = 0;

    bool operator==(const Scissor &other) const = default;
  };

  // Multiple viewports support
  std::vector<Viewport> viewports;

  // Multiple scissors support (matches viewport count)
  std::vector<Scissor> scissors;

  // Default constructor with one viewport and one scissor
  ViewportState() {
    int maxViewports = 0;
    glGetIntegerv(GL_MAX_VIEWPORTS, &maxViewports);
    viewports.resize(static_cast<size_t>(maxViewports));
    scissors.resize(static_cast<size_t>(maxViewports));
  }

  bool operator==(const ViewportState &other) const = default;
};

} // namespace paimon