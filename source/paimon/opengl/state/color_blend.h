#pragma once

#include <vector>

#include "glad/gl.h"

namespace paimon {

// Similar to VkPipelineColorBlendStateCreateInfo
struct ColorBlendState {
  bool logicOpEnable = false;
  GLenum logicOp = GL_COPY;

  // Similar to VkPipelineColorBlendAttachmentState
  struct AttachmentState {
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

    bool operator==(const AttachmentState &other) const = default;
  };

  // Attachment states
  std::vector<AttachmentState> attachments;

  // Blend constants
  float blendConstants[4] = {0.0f, 0.0f, 0.0f, 0.0f}; // R, G, B, A

  ColorBlendState() {
    int maxColorAttachments = 0;
    glGetIntegerv(GL_MAX_COLOR_ATTACHMENTS, &maxColorAttachments);
    attachments.resize(static_cast<size_t>(maxColorAttachments));
  }

  bool operator==(const ColorBlendState &other) const = default;
};

} // namespace paimon