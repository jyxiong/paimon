#include "paimon/core/fg/frame_graph_texture.h"

#include "paimon/core/fg/transient_resources.h"

using namespace paimon;

void FrameGraphTexture::create(void *allocator, const Descriptor &desc) {
  auto *transientResources = static_cast<TransientResources *>(allocator);
  m_texture = transientResources->acquireTexture(desc);
}

void FrameGraphTexture::destroy(void *allocator, const Descriptor &desc) {
  auto *transientResources = static_cast<TransientResources *>(allocator);
  transientResources->releaseTexture(desc, m_texture);
}

void FrameGraphTexture::preRead(void *context, const Descriptor &desc,
                                uint32_t flags) {
  // auto* rc = static_cast<RenderContext*>(context);
  // rc->barrierTexture(
  //   *m_texture, GL_SHADER_IMAGE_ACCESS_BARRIER_BIT |
  //   GL_TEXTURE_FETCH_BARRIER_BIT
  // );
}

void FrameGraphTexture::preWrite(void *context, const Descriptor &desc,
                                 uint32_t flags) {
  // auto* rc = static_cast<RenderContext*>(context);
  // rc->barrierTexture(
  //   *m_texture, GL_SHADER_IMAGE_ACCESS_BARRIER_BIT |
  //   GL_TEXTURE_UPDATE_BARRIER_BIT
  // );
}