#pragma once

#include "paimon/core/fg/graph_node.h"
#include "paimon/core/fg/resource.h"

namespace paimon {
class ResourceNode : public GraphNode {
public:
  ResourceNode(
    const std::string_view name, NodeId id, ResourceId resource, Version version
  );

  ResourceNode(const ResourceNode &) = delete;
  ResourceNode(ResourceNode &&) noexcept = default;

  ResourceNode &operator=(const ResourceNode &) = delete;
  ResourceNode &operator=(ResourceNode &&) noexcept = delete;

  ~ResourceNode() override = default;

  ResourceId getResourceId() const;
  Version getResourceVersion() const;

  PassNode *getProducer() const;
  PassNode *getLastConsumer() const;

  void setProducer(PassNode *node);

  void setLastConsumer(PassNode *node);

private:
  ResourceId m_resource_id;
  Version m_resource_version;

  PassNode *m_producer{nullptr};
  PassNode *m_last{nullptr};
};

} // namespace paimon