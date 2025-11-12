#include "paimon/core/fg/resource_node.h"

using namespace paimon;

ResourceNode::ResourceNode(const std::string_view name, NodeId id,
                           ResourceId resource, Version version)
    : GraphNode(name, id), m_resource_id(resource),
      m_resource_version(version) {}

ResourceId ResourceNode::getResourceId() const { return m_resource_id; }

Version ResourceNode::getResourceVersion() const { return m_resource_version; }

PassNode *ResourceNode::getProducer() const { return m_producer; }

PassNode *ResourceNode::getLastConsumer() const { return m_last; }

void ResourceNode::setProducer(PassNode *node) { m_producer = node; }

void ResourceNode::setLastConsumer(PassNode *node) { m_last = node; }