#pragma once

#include <cstdint>

#include "paimon/opengl/texture.h"

namespace paimon {

class FrameGraphTexture {
public:
  struct Descriptor {
    GLenum target{GL_TEXTURE_2D};
    uint32_t width{0};
    uint32_t height{0};
    uint32_t depth{0};
    uint32_t mipLevels{1};
    uint32_t arrayLayers{1};
    GLenum format{GL_RGBA8};
  };

  void create(void *allocator, const Descriptor &desc);

  void destroy(void *allocator, const Descriptor &desc);

  void preRead(void *context, const Descriptor &desc, uint32_t flags = 0);

  void preWrite(void *context, const Descriptor &desc, uint32_t flags = 0);

  Texture *getTexture() { return m_texture; }
  const Texture *getTexture() const { return m_texture; }

private:
  Texture *m_texture = nullptr;
};

} // namespace paimon