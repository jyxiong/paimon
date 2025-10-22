#include "paimon/core/fg/pass_node.h"

using namespace paimon;

PassNode::PassNode(
  std::string name, NodeId id, std::unique_ptr<PassConcept> &&pass
)
  : GraphNode(std::move(name), id), m_pass(std::move(pass)) {}

void PassNode::create(NodeId resource) { 
  m_creates.push_back(resource);
  increaseRef();
}

void PassNode::read(NodeId resource, uint32_t flags) {
  m_reads.push_back(resource);
}

void PassNode::write(NodeId resource, uint32_t flags) {
  m_writes.push_back(resource);
  increaseRef();
}

bool PassNode::has_create(NodeId resource) const {
  return std::find(m_creates.begin(), m_creates.end(), resource) !=
         m_creates.end();
}

bool PassNode::has_read(NodeId resource) const {
  return std::find(m_reads.begin(), m_reads.end(), resource) != m_reads.end();
}

bool PassNode::has_write(NodeId resource) const {
  return std::find(m_writes.begin(), m_writes.end(), resource) !=
         m_writes.end();
}