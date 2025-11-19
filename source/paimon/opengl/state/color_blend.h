#pragma once

#include <vector>

#include "glad/gl.h"

namespace paimon {

// Similar to VkPipelineColorBlendAttachmentState
struct ColorBlendAttachmentState {
  bool blendEnable = false;
  
  // Color blend factors
  GLenum srcColorBlendFactor = GL_ONE;
  GLenum dstColorBlendFactor = GL_ZERO;
  GLenum colorBlendOp = GL_FUNC_ADD;
  
  // Alpha blend factors
  GLenum srcAlphaBlendFactor = GL_ONE;
  GLenum dstAlphaBlendFactor = GL_ZERO;
  GLenum alphaBlendOp = GL_FUNC_ADD;
  
  // Color write mask (RGBA)
  bool colorWriteMask[4] = {true, true, true, true}; // R, G, B, A

  bool operator==(const ColorBlendAttachmentState &other) const = default;
};

// Similar to VkPipelineColorBlendStateCreateInfo
struct ColorBlendState {
  bool logicOpEnable = false;
  GLenum logicOp = GL_COPY;
  
  // Attachment states
  std::vector<ColorBlendAttachmentState> attachments;
  
  // Blend constants
  float blendConstants[4] = {0.0f, 0.0f, 0.0f, 0.0f}; // R, G, B, A

  bool operator==(const ColorBlendState &other) const = default;
};

// Type aliases for backward compatibility
using ColorBlendAttachment = ColorBlendAttachmentState;

} // namespace paimon