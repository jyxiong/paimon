#pragma once

#include <memory>
#include <vector>

#include "paimon/core/fg/graph_node.h"
#include "paimon/core/fg/pass.h"

namespace paimon {
class PassNode : public GraphNode {
public:
  PassNode(std::string name, NodeId id, std::unique_ptr<PassConcept> &&pass);

  PassNode(const PassNode &) = delete;
  PassNode(PassNode &&) noexcept = default;

  PassNode &operator=(const PassNode &) = delete;
  PassNode &operator=(PassNode &&) noexcept = delete;

  ~PassNode() override = default;

  const std::vector<NodeId> &getCreates() const;

  const std::vector<NodeId> &getReads() const;

  const std::vector<NodeId> &getWrites() const;

  NodeId create(NodeId resource);

  NodeId read(NodeId resource, uint32_t flags = 0);

  NodeId write(NodeId resource, uint32_t flags = 0);

  bool has_create(NodeId resource) const;

  bool has_read(NodeId resource) const;

  bool has_write(NodeId resource) const;

  // TODO:
  bool isCulled() const;

  void setCulled(bool culled);

  void execute(FrameGraphResources &resources, void *context) const;

private:
  std::unique_ptr<PassConcept> m_pass;

  bool m_culled{false};

  std::vector<NodeId> m_creates;
  std::vector<NodeId> m_reads;
  std::vector<NodeId> m_writes;
};

} // namespace paimon