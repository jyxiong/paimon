#pragma once

#include "glad/gl.h"

namespace paimon {

// Similar to VkPipelineInputAssemblyStateCreateInfo
struct InputAssemblyState {
  // Primitive topology
  // GL_POINTS, GL_LINES, GL_LINE_STRIP, GL_LINE_LOOP,
  // GL_TRIANGLES, GL_TRIANGLE_STRIP, GL_TRIANGLE_FAN,
  // GL_LINES_ADJACENCY, GL_LINE_STRIP_ADJACENCY,
  // GL_TRIANGLES_ADJACENCY, GL_TRIANGLE_STRIP_ADJACENCY,
  // GL_PATCHES
  GLenum topology = GL_TRIANGLES;
  
  // Primitive restart
  bool primitiveRestartEnable = false;

  bool operator==(const InputAssemblyState &other) const = default;
};

} // namespace paimon