#include "paimon/core/fg/frame_graph_buffer.h"

#include "paimon/core/fg/transient_resources.h"

using namespace paimon;

void FrameGraphBuffer::create(
  void* allocator, const Descriptor& desc
) {
  auto* transientResources = static_cast<TransientResources*>(allocator);
  m_buffer = transientResources->acquireBuffer(desc);
}

void FrameGraphBuffer::destroy(
  void* allocator, const Descriptor& desc
)  {
  auto* transientResources = static_cast<TransientResources*>(allocator);
  transientResources->releaseBuffer(desc, m_buffer);
  m_buffer = nullptr;
}

void FrameGraphBuffer::preRead(
  void* context, const Descriptor& desc, uint32_t flags
) {
  // auto* rc = static_cast<RenderContext*>(context);
  // rc->barrierBuffer(
  //   *m_buffer, GL_SHADER_STORAGE_BARRIER_BIT | GL_VERTEX_ATTRIB_ARRAY_BARRIER_BIT
  // );
}

void FrameGraphBuffer::preWrite(
  void* context, const Descriptor& desc, uint32_t flags
) {
  // auto* rc = static_cast<RenderContext*>(context);
  // rc->barrierBuffer(
  //   *m_buffer, GL_SHADER_STORAGE_BARRIER_BIT | GL_BUFFER_UPDATE_BARRIER_BIT
  // );
}