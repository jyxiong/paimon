#pragma once

#include "paimon/core/fg/frame_graph.h"
#include "paimon/core/fg/pass_node.h"
#include "paimon/core/log_system.h"

namespace paimon {

class FrameGraphResources {
public:
  FrameGraphResources(const FrameGraph &fg, const PassNode &node)
      : m_frameGraph{fg}, m_passNode{node} {}

  FrameGraphResources() = delete;
  FrameGraphResources(const FrameGraphResources &) = delete;
  FrameGraphResources(FrameGraphResources &&) noexcept = delete;

  FrameGraphResources &operator=(const FrameGraphResources &) = delete;
  FrameGraphResources &operator=(FrameGraphResources &&) noexcept = delete;

  template <class TResource>
  TResource &get(NodeId id) const {
    auto has = m_passNode.has_create(id) || m_passNode.has_read(id) ||
               m_passNode.has_write(id);
    if (!has) {
      LOG_ERROR("PassNode does not access the resource with id {}", id);
    }

    return m_frameGraph.get<TResource>(id);
  }

  template <class TResource>
  typename TResource::Descriptor &get_desc(NodeId id) const {
    auto has = m_passNode.has_create(id) || m_passNode.has_read(id) ||
               m_passNode.has_write(id);
    if (!has) {
      LOG_ERROR("PassNode does not access the resource with id {}", id);
    }

    return m_frameGraph.get_desc<TResource>(id);
  }

private:
  const FrameGraph &m_frameGraph;
  const PassNode &m_passNode;
};

} // namespace paimon