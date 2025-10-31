#pragma once

#include <memory>
#include <unordered_map>
#include <vector>

#include "paimon/core/fg/frame_graph_buffer.h"
#include "paimon/core/fg/frame_graph_texture.h"
#include "paimon/opengl/buffer.h"
#include "paimon/opengl/texture.h"

namespace paimon {

class RenderContext;

class TransientResources {
public:
  TransientResources() = delete;
  explicit TransientResources(RenderContext &);
  TransientResources(const TransientResources &) = delete;
  TransientResources(TransientResources &&) noexcept = delete;
  ~TransientResources();

  TransientResources &operator=(const TransientResources &) = delete;
  TransientResources &operator=(TransientResources &&) noexcept = delete;

  void update(float dt);

  Texture *acquireTexture(const FrameGraphTexture::Descriptor &);
  void releaseTexture(const FrameGraphTexture::Descriptor &, Texture *);

  Buffer *acquireBuffer(const FrameGraphBuffer::Descriptor &);
  void releaseBuffer(const FrameGraphBuffer::Descriptor &, Buffer *);

private:
  RenderContext &m_renderContext;

  std::vector<std::unique_ptr<Texture>> m_textures;
  std::vector<std::unique_ptr<Buffer>> m_buffers;

  template <typename T>
  struct ResourceEntry {
    T resource;
    float life;
  };
  template <typename T>
  using ResourcePool = std::vector<ResourceEntry<T>>;

  std::unordered_map<std::size_t, ResourcePool<Texture *>> m_texturePools;
  std::unordered_map<std::size_t, ResourcePool<Buffer *>> m_bufferPools;
};

} // namespace paimon