#include "paimon/rendering/framebuffer_cache.h"

#include "paimon/core/log_system.h"

namespace paimon {

Framebuffer* FramebufferCache::getOrCreate(const RenderingInfo& info) {
  // Check if we need to use the default framebuffer (no attachments)
  if (info.colorAttachments.empty() && 
      !info.depthAttachment.has_value() && 
      !info.stencilAttachment.has_value()) {
    return nullptr; // Use default framebuffer
  }

  // Create configuration key
  AttachmentConfig config = createConfig(info);

  // Check if framebuffer already exists in cache
  auto it = m_cache.find(config);
  if (it != m_cache.end()) {
    return it->second.get();
  }

  // Create new framebuffer
  auto framebuffer = createFramebuffer(info);
  if (!framebuffer) {
    LOG_ERROR("Failed to create framebuffer");
    return nullptr;
  }

  // Check if framebuffer is complete
  if (!framebuffer->isComplete(GL_FRAMEBUFFER)) {
    LOG_ERROR("Framebuffer is not complete");
    return nullptr;
  }

  Framebuffer* ptr = framebuffer.get();
  m_cache[config] = std::move(framebuffer);

  LOG_INFO("Created and cached new framebuffer (cache size: {})", m_cache.size());
  
  return ptr;
}

void FramebufferCache::clear() {
  m_cache.clear();
  LOG_INFO("Framebuffer cache cleared");
}

AttachmentConfig FramebufferCache::createConfig(const RenderingInfo& info) const {
  AttachmentConfig config;

  // Add color attachments
  for (const auto& colorAttachment : info.colorAttachments) {
    config.colorTextures.push_back(
      static_cast<GLuint>(colorAttachment.texture.get_name())
    );
  }

  // Add depth attachment
  if (info.depthAttachment.has_value()) {
    config.depthTexture = static_cast<GLuint>(
      info.depthAttachment.value().texture.get_name()
    );
  }

  // Add stencil attachment
  if (info.stencilAttachment.has_value()) {
    config.stencilTexture = static_cast<GLuint>(
      info.stencilAttachment.value().texture.get_name()
    );
  }

  return config;
}

std::unique_ptr<Framebuffer> FramebufferCache::createFramebuffer(
    const RenderingInfo& info) const {
  auto framebuffer = std::make_unique<Framebuffer>();

  std::vector<GLenum> drawBuffers;

  // Attach color textures
  for (size_t i = 0; i < info.colorAttachments.size(); ++i) {
    const auto& attachment = info.colorAttachments[i];
    GLenum attachmentPoint = GL_COLOR_ATTACHMENT0 + static_cast<GLenum>(i);
    
    framebuffer->attachTexture(attachmentPoint, &attachment.texture, 0);
    drawBuffers.push_back(attachmentPoint);
  }

  // Set draw buffers
  if (!drawBuffers.empty()) {
    framebuffer->setDrawBuffers(
      static_cast<GLsizei>(drawBuffers.size()), 
      drawBuffers.data()
    );
  } else {
    GLenum none = GL_NONE;
    framebuffer->setDrawBuffers(0, &none);
  }

  // Attach depth texture
  if (info.depthAttachment.has_value()) {
    const auto& attachment = info.depthAttachment.value();
    framebuffer->attachTexture(GL_DEPTH_ATTACHMENT, &attachment.texture, 0);
  }

  // Attach stencil texture
  if (info.stencilAttachment.has_value()) {
    const auto& attachment = info.stencilAttachment.value();
    framebuffer->attachTexture(GL_STENCIL_ATTACHMENT, &attachment.texture, 0);
  }

  return framebuffer;
}

} // namespace paimon
