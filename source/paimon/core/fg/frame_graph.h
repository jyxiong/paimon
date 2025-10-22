#pragma once

#include "paimon/core/fg/pass_node.h"
#include "paimon/core/fg/resource_node.h"
#include "paimon/core/log_system.h"

namespace paimon {

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
    NodeId create(
      const std::string &name, const typename TResource::Descriptor &desc
    ) {
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
  const TData &
  create_pass(std::string name, TSetup &&setup, TExecutor &&executor) {
    auto id = m_pass_nodes.size();
    auto *pass = new Pass<TData, TExecutor>(std::forward<TExecutor>(executor));
    auto &passNode = m_pass_nodes.emplace_back(
      std::move(name), id, std::unique_ptr<PassConcept>(pass)
    );

    Builder builder(*this, passNode);
    std::invoke(setup, builder, pass->get_data());
    return pass->get_data();
  }

  template <class TResource>
  NodeId create(
    const std::string &name, const typename TResource::Descriptor &desc,
    NodeId creator
  ) {
    auto id = m_resource_nodes.size();
    auto *resource = new Resource<TResource>(desc);
    m_resource_nodes.emplace_back(
      name, id, std::unique_ptr<ResourceConcept>(resource), creator
    );
    return id;
  }

  template <class TResource>
  NodeId import(
    const std::string &name, const typename TResource::Descriptor &desc,
    TResource &&resource
  ) {
    auto id = m_resource_nodes.size();
    auto *impoerted_resource =
      new ImportedResource<TResource>(desc, std::move(resource));
    m_resource_nodes.emplace_back(
      name, id, std::unique_ptr<ResourceConcept>(impoerted_resource)
    );
    return id;
  }

  template <class TResource>
  TResource &get(NodeId id) {
    return m_resource_nodes[id].get<TResource>();
  }

  template <class TResource>
  typename TResource::Descriptor &get_desc(NodeId id) {
    return m_resource_nodes[id].get_desc<TResource>();
  }

  ResourceNode &getResourceNode(NodeId id) { return m_resource_nodes[id]; }

  void compile();

  void execute(void *context = nullptr);

private:
  struct PassExecution {
    PassNode *passNode;
    std::vector<ResourceNode *> created;
    std::vector<ResourceNode *> destroyed;
  };

  std::vector<PassNode> m_pass_nodes;
  std::vector<ResourceNode> m_resource_nodes;
  std::vector<PassExecution> m_execution_order;
};

class FrameGraphResources {
public:
  FrameGraphResources(const FrameGraph &fg, const PassNode &node);

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