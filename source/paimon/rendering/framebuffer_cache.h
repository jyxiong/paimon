#pragma once

#include <memory>
#include <unordered_map>

#include "paimon/opengl/framebuffer.h"
#include "paimon/rendering/rendering_info.h"

namespace paimon {

// FramebufferCache manages framebuffer objects based on attachment configurations
class FramebufferCache {
public:
  FramebufferCache();
  ~FramebufferCache() = default;

  // Delete copy constructor and assignment
  FramebufferCache(const FramebufferCache&) = delete;
  FramebufferCache& operator=(const FramebufferCache&) = delete;

  // Get or create a framebuffer based on RenderingInfo
  Framebuffer* get(const RenderingInfo& info);

  // Get or create default framebuffer (uses key=0)
  Framebuffer* get(const SwapchainRenderingInfo&);

  // Clear the cache
  void clear();

  // Get cache statistics
  size_t getCacheSize() const { return m_cache.size(); }

private:
  // Compute hash from RenderingInfo
  std::size_t createHash(const RenderingInfo& info) const;

  // Create a new framebuffer from RenderingInfo
  std::unique_ptr<Framebuffer> createFramebuffer(const RenderingInfo& info) const;

private:
  // Cache uses hash value as key, with 0 reserved for default framebuffer
  std::unordered_map<std::size_t, std::unique_ptr<Framebuffer>> m_cache;
};

} // namespace paimon
