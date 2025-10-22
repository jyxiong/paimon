#pragma once

#include <memory>
#include <vector>

#include "paimon/core/fg/graph_node.h"
#include "paimon/core/fg/resource.h"

namespace paimon {
class ResourceNode : public GraphNode {
public:
  ResourceNode(
    const std::string &name, NodeId id,
    std::unique_ptr<ResourceConcept> &&resource
  );

  ResourceNode(
    const std::string &name, NodeId id,
    std::unique_ptr<ResourceConcept> &&resource, NodeId creator
  );

  ResourceNode(const ResourceNode &) = delete;
  ResourceNode(ResourceNode &&) noexcept = default;

  ResourceNode &operator=(const ResourceNode &) = delete;
  ResourceNode &operator=(ResourceNode &&) noexcept = delete;

  ~ResourceNode() override = default;

  template <class TResource>
  TResource &get() {
    return dynamic_cast<Resource<TResource>>(*m_resource).get();
  }

  template <class TResource>
  typename TResource::Descriptor &get_desc() {
    return dynamic_cast<Resource<TResource>>(*m_resource).get_desc();
  }

  void create(void* allocator) { m_resource->create(allocator); }
  void destroy(void* allocator) { m_resource->destroy(allocator); }

  void addReader(NodeId passId);
  void addWriter(NodeId passId);
  
  const std::vector<NodeId>& getReaders() const { return m_readers; }
  const std::vector<NodeId>& getWriters() const { return m_writers; }
  NodeId getCreator() const { return m_creator; }

  bool isTransient() const { return m_resource->isTransient(); }

private:
  std::unique_ptr<ResourceConcept> m_resource;

  NodeId m_creator;
  std::vector<NodeId> m_readers;
  std::vector<NodeId> m_writers;
};

} // namespace paimon