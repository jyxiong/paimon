#pragma once

#include <vector>
#include "glad/gl.h"

namespace paimon {

// Similar to VkVertexInputBindingDescription
struct VertexInputBindingDescription {
  uint32_t binding = 0;       // Binding number
  uint32_t stride = 0;        // Stride between consecutive elements
  uint32_t divisor = 0;       // Instancing divisor (0 = per-vertex, 1+ = per-instance)
  
  bool operator==(const VertexInputBindingDescription &other) const = default;
};

// Similar to VkVertexInputAttributeDescription
struct VertexInputAttributeDescription {
  uint32_t location = 0;      // Shader attribute location
  uint32_t binding = 0;       // Which binding to read from
  GLenum format = GL_FLOAT;   // Data type (GL_FLOAT, GL_INT, GL_UNSIGNED_INT, etc.)
  uint32_t size = 1;          // Number of components (1-4)
  uint32_t offset = 0;        // Offset within the vertex data
  bool normalized = false;    // Whether to normalize fixed-point data
  
  bool operator==(const VertexInputAttributeDescription &other) const = default;
};

// Similar to VkPipelineVertexInputStateCreateInfo
struct VertexInputState {
  std::vector<VertexInputBindingDescription> bindings;
  std::vector<VertexInputAttributeDescription> attributes;
  
  bool operator==(const VertexInputState &other) const = default;
};

// Backward compatibility
using PipelineVertexInputStateCreateInfo = VertexInputState;

} // namespace paimon
