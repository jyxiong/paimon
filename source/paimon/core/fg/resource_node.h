#pragma once

#include "paimon/core/fg/graph_node.h"
#include "paimon/core/fg/resource.h"

namespace paimon {
class ResourceNode : public GraphNode {
public:
  ResourceNode(
    const std::string_view name, NodeId id, ResourceId resource, Version version
  )
    : GraphNode(name, id), m_resource_id(resource),
      m_resource_version(version) {}

  ResourceNode(const ResourceNode &) = delete;
  ResourceNode(ResourceNode &&) noexcept = default;

  ResourceNode &operator=(const ResourceNode &) = delete;
  ResourceNode &operator=(ResourceNode &&) noexcept = delete;

  ~ResourceNode() override = default;

  ResourceId getResourceId() const { return m_resource_id; }
  Version getResourceVersion() const { return m_resource_version; }

  PassNode *getProducer() const { return m_producer; }
  PassNode *getLastConsumer() const { return m_last; }

  void setProducer(PassNode *node) { m_producer = node; }

  void setLastConsumer(PassNode *node) { m_last = node; }

private:
  ResourceId m_resource_id;
  Version m_resource_version;

  PassNode *m_producer{nullptr};
  PassNode *m_last{nullptr};
};

} // namespace paimon