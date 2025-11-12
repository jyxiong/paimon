#include "paimon/core/fg/transient_resources.h"

#include "paimon/core/hash.h"
#include "paimon/core/log_system.h"
#include "paimon/rendering/render_context.h"
#include <algorithm>

using namespace paimon;

namespace std {

template <>
struct hash<FrameGraphTexture::Descriptor> {
  std::size_t
  operator()(const FrameGraphTexture::Descriptor &desc) const noexcept {
    std::size_t h{0};
    hashCombine(h, desc.target, desc.width, desc.height, desc.depth,
                desc.mipLevels, desc.arrayLayers, desc.format);
    return h;
  }
};

template <>
struct hash<FrameGraphBuffer::Descriptor> {
  std::size_t
  operator()(const FrameGraphBuffer::Descriptor &desc) const noexcept {
    std::size_t h{0};
    hashCombine(h, desc.size);
    return h;
  }
};

} // namespace std

namespace {

void heartbeat(auto &objects, auto &pools, float dt, auto &&deleter) {
  constexpr auto kMaxIdleTime = 1.0f; // in seconds

  auto poolIt = pools.begin();
  while (poolIt != pools.end()) {
    auto &[_, pool] = *poolIt;
    if (pool.empty()) {
      poolIt = pools.erase(poolIt);
    } else {
      auto objectIt = pool.begin();
      while (objectIt != pool.cend()) {
        auto &[object, idleTime] = *objectIt;
        idleTime += dt;
        if (idleTime >= kMaxIdleTime) {
          deleter(*object);
          objectIt = pool.erase(objectIt);
        } else {
          ++objectIt;
        }
      }
      ++poolIt;
    }
  }
  objects.erase(std::remove_if(objects.begin(), objects.end(),
                               [](auto &object) { return object->is_valid(); }),
                objects.end());
}

} // namespace

TransientResources::TransientResources(RenderContext &rc)
    : m_renderContext{rc} {}

TransientResources::~TransientResources() {
  // for (auto &texture : m_textures)
  //   m_renderContext.destroy(*texture);
  // for (auto &buffer : m_buffers)
  //   m_renderContext.destroy(*buffer);
}

void TransientResources::update(float dt) {
  const auto deleter = [&](auto &object) {};
  heartbeat(m_textures, m_texturePools, dt, deleter);
  heartbeat(m_buffers, m_bufferPools, dt, deleter);
}

Texture *
TransientResources::acquireTexture(const FrameGraphTexture::Descriptor &desc) {
  const auto h = std::hash<FrameGraphTexture::Descriptor>{}(desc);
  auto &pool = m_texturePools[h];
  if (pool.empty()) {
    Texture texture(desc.target);
    texture.set_storage_2d(desc.mipLevels, desc.format, desc.width,
                           desc.height);

    m_textures.emplace_back(std::make_unique<Texture>(std::move(texture)));
    return m_textures.back().get();
  } else {
    auto *texture = pool.back().resource;
    pool.pop_back();
    return texture;
  }
}

void TransientResources::releaseTexture(
    const FrameGraphTexture::Descriptor &desc, Texture *texture) {
  const auto h = std::hash<FrameGraphTexture::Descriptor>{}(desc);
  m_texturePools[h].emplace_back(texture, 0.0f);
}

Buffer *
TransientResources::acquireBuffer(const FrameGraphBuffer::Descriptor &desc) {
  const auto h = std::hash<FrameGraphBuffer::Descriptor>{}(desc);
  auto &pool = m_bufferPools[h];
  if (pool.empty()) {
    Buffer buffer;
    buffer.set_storage(desc.size, nullptr, desc.usage);
    m_buffers.emplace_back(std::make_unique<Buffer>(std::move(buffer)));
    return m_buffers.back().get();
  } else {
    auto *buffer = pool.back().resource;
    pool.pop_back();
    return buffer;
  }
}

void TransientResources::releaseBuffer(const FrameGraphBuffer::Descriptor &desc,
                                       Buffer *buffer) {
  const auto h = std::hash<FrameGraphBuffer::Descriptor>{}(desc);
  m_bufferPools[h].emplace_back(buffer, 0.0f);
}
