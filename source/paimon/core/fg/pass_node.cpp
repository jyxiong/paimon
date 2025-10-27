#include "paimon/core/fg/pass_node.h"

using namespace paimon;

namespace {
bool hasId(const std::vector<NodeId> &ids, NodeId id) {
  return std::find(ids.begin(), ids.end(), id) != ids.end();
}
} // namespace

PassNode::PassNode(
  std::string name, NodeId id, std::unique_ptr<PassConcept> &&pass
)
  : GraphNode(std::move(name), id), m_pass(std::move(pass)) {}

const std::vector<NodeId> &PassNode::getCreates() const { return m_creates; }

const std::vector<NodeId> &PassNode::getReads() const { return m_reads; }

const std::vector<NodeId> &PassNode::getWrites() const { return m_writes; }

NodeId PassNode::create(NodeId resource) {
  return m_creates.emplace_back(resource);
}

NodeId PassNode::read(NodeId resource, uint32_t flags) {
  return m_reads.emplace_back(resource);
}

NodeId PassNode::write(NodeId resource, uint32_t flags) {
  return m_writes.emplace_back(resource);
}

bool PassNode::has_create(NodeId resource) const {
  return hasId(m_creates, resource);
}

bool PassNode::has_read(NodeId resource) const {
  return hasId(m_reads, resource);
}

bool PassNode::has_write(NodeId resource) const {
  return hasId(m_writes, resource);
}

bool PassNode::isCulled() const { return m_culled; }

void PassNode::setCulled(bool culled) { m_culled = culled; }

void PassNode::execute(FrameGraphResources &resources, void *context) const {
  m_pass->execute(resources, context);
}
