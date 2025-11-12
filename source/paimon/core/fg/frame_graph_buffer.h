#pragma once

#include <cstdint>

#include "paimon/opengl/buffer.h"

namespace paimon {
class FrameGraphBuffer {
public:
  struct Descriptor {
    size_t size = 0;
    GLbitfield usage = GL_DYNAMIC_STORAGE_BIT;
  };

  void create(void *allocator, const Descriptor &desc);

  void destroy(void *allocator, const Descriptor &desc);

  void preRead(void *context, const Descriptor &desc, uint32_t flags = 0);

  void preWrite(void *context, const Descriptor &desc, uint32_t flags = 0);

private:
  Buffer *m_buffer = nullptr;
};
} // namespace paimon