#pragma once

#include <concepts>
#include <functional>

#include "paimon/core/fg/pass_node.h"
#include "paimon/core/fg/resource_entry.h"
#include "paimon/core/fg/resource_node.h"

namespace paimon {

class FrameGraphResources;

class FrameGraph {
public:
  class Builder {
  public:
    Builder(FrameGraph &fg, PassNode &node);

    Builder() = delete;
    Builder(const Builder &) = delete;
    Builder(Builder &&) noexcept = delete;

    Builder &operator=(const Builder &) = delete;
    Builder &operator=(Builder &&) noexcept = delete;

    template <class TResource>
    NodeId create(const std::string &name,
                  const typename TResource::Descriptor &desc) {
      auto id = m_frameGraph.create<TResource>(name, desc, m_passNode.getId());
      m_passNode.create(id);
      return id;
    }

    NodeId read(NodeId id, uint32_t flags = 0);

    NodeId write(NodeId id, uint32_t flags = 0);

  private:
    FrameGraph &m_frameGraph;
    PassNode &m_passNode;
  };

public:
  FrameGraph() = default;
  FrameGraph(const FrameGraph &) = delete;
  FrameGraph(FrameGraph &&) noexcept = delete;

  FrameGraph &operator=(const FrameGraph &) = delete;
  FrameGraph &operator=(FrameGraph &&) noexcept = delete;

  template <class TData, class TSetup, class TExecutor>
    requires std::invocable<TSetup, Builder &, TData &> &&
             std::invocable<TExecutor, FrameGraphResources &, void *>
  const TData &create_pass(std::string name, TSetup &&setup,
                           TExecutor &&executor) {
    auto id = m_pass_nodes.size();
    auto *pass = new Pass<TData, TExecutor>(std::forward<TExecutor>(executor));
    auto &passNode = m_pass_nodes.emplace_back(
        std::move(name), id, std::unique_ptr<PassConcept>(pass));

    Builder builder(*this, passNode);
    std::invoke(setup, builder, pass->get_data());
    return pass->get_data();
  }

  template <class TResource>
  NodeId create(const std::string &name,
                const typename TResource::Descriptor &desc, NodeId creator) {
    auto res_id = m_resource_entries.size();
    m_resource_entries.emplace_back(
        res_id, std::make_unique<Resource<TResource>>(desc));

    auto node_id = m_resource_nodes.size();
    m_resource_nodes.emplace_back(name, node_id, res_id, 0);
    return node_id;
  }

  template <class TResource>
  NodeId import(const std::string &name,
                const typename TResource::Descriptor &desc,
                TResource &&resource) {
    auto res_id = m_resource_entries.size();
    m_resource_entries.emplace_back(
        res_id, std::make_unique<ImportedResource<TResource>>(
                    desc, std::move(resource)));

    auto node_id = m_resource_nodes.size();
    m_resource_nodes.emplace_back(name, node_id, res_id, 0);
    return node_id;
  }

  NodeId clone(NodeId id) {
    const auto &node = getResourceNode(id);
    auto &entry = getResourceEntry(node.getResourceId());
    entry.incrementVersion();

    auto node_id = m_resource_nodes.size();
    m_resource_nodes.emplace_back(node.getName(), node_id, node.getResourceId(),
                                  entry.getVersion());
    return node_id;
  }

  template <class TResource>
  TResource &get(NodeId id) {
    return getResourceEntry(getResourceNode(id).getResourceId())
        .get<TResource>();
  }

  template <class TResource>
  typename TResource::Descriptor &get_desc(NodeId id) {
    return getResourceEntry(getResourceNode(id).getResourceId())
        .get_desc<TResource>();
  }

  ResourceNode &getResourceNode(NodeId id);

  const ResourceNode &getResourceNode(NodeId id) const;

  ResourceEntry &getResourceEntry(ResourceId id);

  const ResourceEntry &getResourceEntry(ResourceId id) const;

  void compile();

  void execute(void *context, void *allocator);

  // Visualization methods
  void exportToDot(const std::string &filename) const;
  void exportExecutionOrderToDot(const std::string &filename) const;

private:
  struct PassExecution {
    PassNode *passNode;
    std::vector<ResourceNode *> created;
    std::vector<ResourceNode *> destroyed;
  };

  std::vector<PassNode> m_pass_nodes;
  std::vector<ResourceNode> m_resource_nodes;
  std::vector<ResourceEntry> m_resource_entries;

  std::vector<PassExecution> m_execution_order;
};
} // namespace paimon