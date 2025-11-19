#pragma once

#include <array>
#include <cstdint>
#include <optional>

#include <glm/glm.hpp>

namespace paimon {

class Framebuffer;

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
  AttachmentLoadOp loadOp = AttachmentLoadOp::DontCare;
  AttachmentStoreOp storeOp = AttachmentStoreOp::Store;
  ClearValue clearValue;
};

// Main rendering info structure (similar to VkRenderingInfo)
struct RenderingInfo {
  // Framebuffer to render to (nullptr for default framebuffer)
  Framebuffer* framebuffer = nullptr;
  
  // Render area
  glm::ivec2 renderAreaOffset{0, 0};
  glm::ivec2 renderAreaExtent{0, 0};
  
  // Color attachments (support up to 8 color attachments)
  std::array<std::optional<RenderingAttachmentInfo>, 8> colorAttachments;
  
  // Depth attachment
  std::optional<RenderingAttachmentInfo> depthAttachment;
  
  // Stencil attachment
  std::optional<RenderingAttachmentInfo> stencilAttachment;

  RenderingInfo() = default;
};

} // namespace paimon
