#pragma once

#include <memory>
#include <unordered_map>
#include <vector>

#include "paimon/opengl/framebuffer.h"
#include "paimon/rendering/rendering_info.h"

namespace paimon {

// Hash function for attachment configuration
struct AttachmentConfig {
  std::vector<GLuint> colorTextures;
  GLuint depthTexture = 0;
  GLuint stencilTexture = 0;

  bool operator==(const AttachmentConfig& other) const {
    return colorTextures == other.colorTextures &&
           depthTexture == other.depthTexture &&
           stencilTexture == other.stencilTexture;
  }
};

struct AttachmentConfigHash {
  std::size_t operator()(const AttachmentConfig& config) const {
    std::size_t hash = 0;
    for (const auto& tex : config.colorTextures) {
      hash ^= std::hash<GLuint>{}(tex) + 0x9e3779b9 + (hash << 6) + (hash >> 2);
    }
    hash ^= std::hash<GLuint>{}(config.depthTexture) + 0x9e3779b9 + (hash << 6) + (hash >> 2);
    hash ^= std::hash<GLuint>{}(config.stencilTexture) + 0x9e3779b9 + (hash << 6) + (hash >> 2);
    return hash;
  }
};

// FramebufferCache manages framebuffer objects based on attachment configurations
class FramebufferCache {
public:
  FramebufferCache() = default;
  ~FramebufferCache() = default;

  // Delete copy constructor and assignment
  FramebufferCache(const FramebufferCache&) = delete;
  FramebufferCache& operator=(const FramebufferCache&) = delete;

  // Get or create a framebuffer based on RenderingInfo
  // Returns nullptr if no attachments are specified (use default framebuffer)
  Framebuffer* getOrCreate(const RenderingInfo& info);

  // Clear the cache
  void clear();

  // Get cache statistics
  size_t getCacheSize() const { return m_cache.size(); }

private:
  // Create attachment configuration from RenderingInfo
  AttachmentConfig createConfig(const RenderingInfo& info) const;

  // Create a new framebuffer from RenderingInfo
  std::unique_ptr<Framebuffer> createFramebuffer(const RenderingInfo& info) const;

private:
  std::unordered_map<AttachmentConfig, std::unique_ptr<Framebuffer>, AttachmentConfigHash> m_cache;
};

} // namespace paimon
