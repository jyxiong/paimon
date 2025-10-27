#include "paimon/core/fg/graph_node.h"

using namespace paimon;

GraphNode::GraphNode(const std::string_view name, NodeId id)
  : m_name{name}, m_id{id} {}

NodeId GraphNode::getId() const { return m_id; }

std::string_view GraphNode::getName() const { return m_name; }

std::size_t GraphNode::getRefCount() const { return m_refCount; }

void GraphNode::setRefCount(std::size_t count) { m_refCount = count; }

std::size_t GraphNode::increaseRef() {
  ++m_refCount;
  return m_refCount;
}

std::size_t GraphNode::decreaseRef() {
  if (m_refCount > 0) {
    --m_refCount;
  }
  return m_refCount;
}
