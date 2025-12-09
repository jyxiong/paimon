#pragma once

#include <glad/gl.h>

namespace paimon {

// Similar to VkPipelineDepthStencilStateCreateInfo
struct DepthStencilState {
  // Depth test
  bool depthTestEnable = false;
  bool depthWriteEnable = true;
  GLenum depthCompareOp = GL_LESS; // Comparison operator
  
  // Depth bounds test
  bool depthBoundsTestEnable = false;
  float minDepthBounds = 0.0f;
  float maxDepthBounds = 1.0f;
  
  // Stencil test (integrated here like Vulkan)
  bool stencilTestEnable = false;
  
  // Front face stencil operations
  struct StencilOpState {
    GLenum failOp = GL_KEEP;       // Action on stencil test fail
    GLenum passOp = GL_KEEP;       // Action on stencil and depth pass
    GLenum depthFailOp = GL_KEEP;  // Action on stencil pass, depth fail
    GLenum compareOp = GL_ALWAYS;  // Stencil comparison operator
    GLuint compareMask = 0xFF;     // Stencil comparison mask
    GLuint writeMask = 0xFF;       // Stencil write mask
    GLint reference = 0;           // Stencil reference value
    
    bool operator==(const StencilOpState &other) const = default;
  };
  
  StencilOpState front;
  StencilOpState back;

  bool operator==(const DepthStencilState &other) const = default;
};

} // namespace paimon