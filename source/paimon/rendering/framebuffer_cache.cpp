#include "paimon/rendering/framebuffer_cache.h"

#include "paimon/core/hash.h"
#include "paimon/core/log_system.h"

namespace paimon {

Framebuffer* FramebufferCache::getOrCreate(const RenderingInfo& info) {
  // Compute hash directly from RenderingInfo
  std::size_t hash = createHash(info);
  
  // Ensure hash is not 0 (reserved for default framebuffer)
  if (hash == 0) {
    hash = 1;
  }

  // Check if framebuffer already exists in cache
  auto it = m_cache.find(hash);
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
  m_cache[hash] = std::move(framebuffer);

  LOG_INFO("Created and cached new framebuffer with hash {} (cache size: {})", hash, m_cache.size());
  
  return ptr;
}

Framebuffer* FramebufferCache::getOrCreate(const SwapchainRenderingInfo& info) {
  // Default framebuffer uses key = 0
  constexpr std::size_t defaultKey = 0;
  
  // Check if default framebuffer already exists in cache
  auto it = m_cache.find(defaultKey);
  if (it != m_cache.end()) {
    return it->second.get();
  }
  
  // Create default framebuffer
  auto framebuffer = std::make_unique<Framebuffer>(true); // true = default framebuffer
  Framebuffer* ptr = framebuffer.get();
  m_cache[defaultKey] = std::move(framebuffer);
  
  LOG_INFO("Created and cached default framebuffer with key 0 (cache size: {})", m_cache.size());
  
  return ptr;
}

void FramebufferCache::clear() {
  m_cache.clear();
  LOG_INFO("Framebuffer cache cleared");
}

std::size_t FramebufferCache::createHash(const RenderingInfo& info) const {
  std::size_t hash = 0;

  // Hash color attachments
  for (const auto& colorAttachment : info.colorAttachments) {
    hashCombine(hash, colorAttachment.texture.get_name());
  }

  // Hash depth attachment
  if (info.depthAttachment.has_value()) {
    hashCombine(hash, info.depthAttachment.value().texture.get_name());
  }

  // Hash stencil attachment
  if (info.stencilAttachment.has_value()) {
    hashCombine(hash, info.stencilAttachment.value().texture.get_name());
  }

  return hash;
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
