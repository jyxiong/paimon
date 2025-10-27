#include "paimon/core/fg/resource_node.h"

#include <algorithm>

using namespace paimon;

// ResourceNode::ResourceNode(
//   const std::string &name, NodeId id,
//   std::unique_ptr<ResourceConcept> &&resource
// )
//   : GraphNode(name, id), m_resource(std::move(resource)),
//     m_creator(static_cast<NodeId>(-1)) {}

// ResourceNode::ResourceNode(
//   const std::string &name, NodeId id,
//   std::unique_ptr<ResourceConcept> &&resource, NodeId creator
// )
//   : GraphNode(name, id), m_resource(std::move(resource)), m_creator(creator) {}

// void ResourceNode::addReader(NodeId passId) {
//   auto it = std::find(m_readers.begin(), m_readers.end(), passId);
//   if (it == m_readers.end()) {
//     m_readers.push_back(passId);
//     increaseRef();
//   }
// }

// void ResourceNode::addWriter(NodeId passId) {
//   auto it = std::find(m_writers.begin(), m_writers.end(), passId);
//   if (it == m_writers.end()) {
//     m_writers.push_back(passId);
//     increaseRef();
//   }
// }