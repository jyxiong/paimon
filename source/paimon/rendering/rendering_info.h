#pragma once

#include <cstdint>
#include <optional>
#include <vector>

#include <glm/glm.hpp>

#include "paimon/opengl/texture.h"

namespace paimon {

// Attachment load operation (similar to VkAttachmentLoadOp)
enum class AttachmentLoadOp {
  Load,     // Load existing contents
  Clear,    // Clear to specified value
  DontCare  // Contents undefined
};

// Attachment store operation (similar to VkAttachmentStoreOp)
enum class AttachmentStoreOp {
  Store,    // Store rendering results
  DontCare  // Contents undefined after rendering
};

// Clear value for color/depth/stencil attachments
struct ClearValue {
  union {
    struct {
      float r, g, b, a;
    } color;
    struct {
      float depth;
      uint32_t stencil;
    } depthStencil;
  };

  ClearValue() : color{0.0f, 0.0f, 0.0f, 0.0f} {}
  
  static ClearValue Color(float r, float g, float b, float a = 1.0f) {
    ClearValue value;
    value.color.r = r;
    value.color.g = g;
    value.color.b = b;
    value.color.a = a;
    return value;
  }

  static ClearValue DepthStencil(float depth, uint32_t stencil = 0) {
    ClearValue value;
    value.depthStencil.depth = depth;
    value.depthStencil.stencil = stencil;
    return value;
  }
};

// Rendering attachment info (similar to VkRenderingAttachmentInfo)
struct RenderingAttachmentInfo {
  Texture &texture;
  AttachmentLoadOp loadOp = AttachmentLoadOp::DontCare;
  AttachmentStoreOp storeOp = AttachmentStoreOp::Store;
  ClearValue clearValue;
};

// Main rendering info structure (similar to VkRenderingInfo)
struct RenderingInfo {
  // Render area
  glm::ivec2 renderAreaOffset{0, 0};
  glm::ivec2 renderAreaExtent{0, 0};
  
  // Color attachments (dynamic size)
  std::vector<RenderingAttachmentInfo> colorAttachments;
  
  // Depth attachment
  std::optional<RenderingAttachmentInfo> depthAttachment;
  
  // Stencil attachment
  std::optional<RenderingAttachmentInfo> stencilAttachment;

  RenderingInfo() = default;
};

// Swapchain rendering info for default framebuffer
struct SwapchainRenderingInfo {
  // Render area
  glm::ivec2 renderAreaOffset{0, 0};
  glm::ivec2 renderAreaExtent{0, 0};
  
  // Clear color
  ClearValue clearColor;
  
  // Clear depth
  float clearDepth = 1.0f;
  
  // Clear stencil
  uint32_t clearStencil = 0;

  SwapchainRenderingInfo() = default;
};

} // namespace paimon
