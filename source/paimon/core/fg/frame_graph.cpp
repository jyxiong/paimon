#include "paimon/core/fg/frame_graph.h"

#include "paimon/core/fg/frame_graph_resources.h"

#include <stack>

using namespace paimon;

FrameGraph::Builder::Builder(FrameGraph &fg, PassNode &node)
  : m_frameGraph{fg}, m_passNode{node} {}

NodeId FrameGraph::Builder::read(NodeId id, uint32_t flags) {
  return m_passNode.read(id, flags);
}

NodeId FrameGraph::Builder::write(NodeId id, uint32_t flags) {

if (m_passNode.has_create(id)) {
    return m_passNode.write(id, flags);
  } else {
    // Writing to a texture produces a renamed handle.
    // This allows us to catch errors when resources are modified in
    // undefined order (when same resource is written by different passes).
    // Renaming resources enforces a specific execution order of the render
    // passes.
    m_passNode.read(id, 0);

    auto resource = m_frameGraph.clone(id);
    // auto &node = m_frameGraph.getResourceNode(resource);
    // node.setProducer(&m_passNode);

    return m_passNode.write(resource, flags);
  }
}

void FrameGraph::compile() {
  for (auto &pass : m_pass_nodes) {
    pass.setRefCount(pass.getWrites().size());
    for (const auto id : pass.getReads()) {
      auto &consumed = m_resource_nodes[id];
      consumed.increaseRef();
    }
    for (const auto id : pass.getWrites()) {
      auto &written = m_resource_nodes[id];
      written.m_producer = &pass;
    }
  }

  // -- Culling:

  std::stack<ResourceNode *> unreferencedResources;
  for (auto &node : m_resource_nodes) {
    if (node.m_refCount == 0) unreferencedResources.push(&node);
  }
  while (!unreferencedResources.empty()) {
    auto *unreferencedResource = unreferencedResources.top();
    unreferencedResources.pop();
    PassNode *producer{unreferencedResource->m_producer};
    if (producer == nullptr || producer->hasSideEffect()) continue;

    assert(producer->m_refCount >= 1);
    if (--producer->m_refCount == 0) {
      for (const auto [id, _] : producer->m_reads) {
        auto &node = m_resource_nodes[id];
        if (--node.m_refCount == 0) unreferencedResources.push(&node);
      }
    }
  }

  // -- Calculate resources lifetime:

  for (auto &pass : m_passNodes) {
    if (pass.m_refCount == 0) continue;

    for (const auto id : pass.m_creates)
      getResourceEntry(id).m_producer = &pass;
    for (const auto [id, _] : pass.m_writes)
      getResourceEntry(id).m_last = &pass;
    for (const auto [id, _] : pass.m_reads)
      getResourceEntry(id).m_last = &pass;
  }
}

void FrameGraph::execute(void *context, void *allocator) {
  for (const auto &pass : m_pass_nodes) {

    for (const auto id : pass.getCreates())
      getResourceEntry(id).create(allocator);

    FrameGraphResources resources{*this, pass};
    pass.execute(resources, context);

    for (auto &entry : m_resource_entries) {
      if (entry.getLast() == &pass && entry.isTransient())
        entry.destroy(allocator);
    }
  }
}
