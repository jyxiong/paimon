#include "paimon/core/fg/frame_graph.h"

#include <fstream>
#include <stack>

#include "paimon/core/fg/frame_graph_resources.h"

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

ResourceNode &FrameGraph::getResourceNode(NodeId id) {
  return m_resource_nodes[id];
}

const ResourceNode &FrameGraph::getResourceNode(NodeId id) const {
  return m_resource_nodes[id];
}

ResourceEntry &FrameGraph::getResourceEntry(ResourceId id) {
  return m_resource_entries[id];
}

const ResourceEntry &FrameGraph::getResourceEntry(ResourceId id) const {
  return m_resource_entries[id];
}

void FrameGraph::compile() {
  for (auto &passNode : m_pass_nodes) {
    passNode.setRefCount(passNode.getWrites().size());
    for (const auto id : passNode.getReads()) {
      m_resource_nodes[id].increaseRef();
    }
    for (const auto id : passNode.getWrites()) {
      m_resource_nodes[id].setProducer(&passNode);
    }
  }

  // -- Culling:
  std::stack<ResourceNode *> unrefResNodes;
  for (auto &resNode : m_resource_nodes) {
    if (resNode.getRefCount() == 0) {
      unrefResNodes.push(&resNode);
    }
  }

  while (!unrefResNodes.empty()) {
    auto *unrefResNode = unrefResNodes.top();
    unrefResNodes.pop();
    PassNode *producer{unrefResNode->getProducer()};
    if (producer == nullptr /*|| producer->hasSideEffect()*/)
      continue;

    if (producer->decreaseRef() == 0) {
      for (const auto id : producer->getReads()) {
        auto &resNode = m_resource_nodes[id];
        if (resNode.decreaseRef() == 0) {
          unrefResNodes.push(&resNode);
        }
      }
    }
  }

  // -- Calculate resources lifetime:
  for (auto &passNode : m_pass_nodes) {
    if (passNode.getRefCount() == 0)
      continue;

    for (const auto id : passNode.getCreates()) {
      getResourceEntry(id).setProducer(&passNode);
    }
    for (const auto id : passNode.getWrites()) {
      getResourceEntry(id).setLast(&passNode);
    }
    for (const auto id : passNode.getReads()) {
      getResourceEntry(id).setLast(&passNode);
    }
  }
}

void FrameGraph::execute(void *context, void *allocator) {
  for (const auto &passNode : m_pass_nodes) {

    for (const auto id : passNode.getCreates()) {
      getResourceEntry(id).create(allocator);
    }

    FrameGraphResources resources{*this, passNode};
    passNode.execute(resources, context);

    for (auto &entry : m_resource_entries) {
      if (entry.getLast() == &passNode && entry.isTransient()) {
        entry.destroy(allocator);
      }
    }
  }
}

void FrameGraph::exportToDot(const std::string &filename) const {
  std::ofstream file(filename);
  if (!file.is_open()) {
    return;
  }

  file << "digraph FrameGraph {\n";
  file << "  rankdir=LR;\n";
  file << "  node [fontname=\"Arial\"];\n";
  file << "  edge [fontname=\"Arial\"];\n\n";

  // Define styles
  file << "  // Pass nodes\n";
  file << "  node [shape=box, style=filled, fillcolor=lightblue];\n";

  // Draw pass nodes
  for (const auto &pass : m_pass_nodes) {
    std::string label{pass.getName()};
    std::string color = pass.isCulled() ? "lightgray" : "lightblue";
    file << "  pass_" << pass.getId() << " [label=\"" << label
         << "\" fillcolor=" << color << "];\n";
  }

  file << "\n  // Resource nodes\n";
  file << "  node [shape=ellipse, style=filled, fillcolor=lightgreen];\n";

  // Draw resource nodes
  for (const auto &resource : m_resource_nodes) {
    std::string label{resource.getName()};
    // Add version info if not zero
    if (resource.getResourceVersion() > 0) {
      label += " (v" + std::to_string(resource.getResourceVersion()) + ")";
    }
    file << "  res_" << resource.getId() << " [label=\"" << label << "\"];\n";
  }

  file << "\n  // Dependencies\n";

  // Draw dependencies
  for (const auto &pass : m_pass_nodes) {
    if (pass.isCulled())
      continue;

    // Create edges (green)
    for (auto res_id : pass.getCreates()) {
      file << "  pass_" << pass.getId() << " -> res_" << res_id
           << " [label=\"creates\" color=green fontcolor=green];\n";
    }

    // Read edges (blue)
    for (auto res_id : pass.getReads()) {
      file << "  res_" << res_id << " -> pass_" << pass.getId()
           << " [label=\"reads\" color=blue fontcolor=blue];\n";
    }

    // Write edges (red)
    for (auto res_id : pass.getWrites()) {
      file << "  pass_" << pass.getId() << " -> res_" << res_id
           << " [label=\"writes\" color=red fontcolor=red];\n";
    }
  }

  file << "}\n";
  file.close();
}

void FrameGraph::exportExecutionOrderToDot(const std::string &filename) const {
  std::ofstream file(filename);
  if (!file.is_open()) {
    return;
  }

  file << "digraph FrameGraphExecution {\n";
  file << "  rankdir=TB;\n";
  file << "  node [fontname=\"Arial\"];\n";
  file << "  edge [fontname=\"Arial\"];\n\n";

  // Draw execution order
  file << "  // Execution order\n";
  file << "  node [shape=box, style=filled, fillcolor=lightblue];\n";

  PassNode *prevPass = nullptr;
  int executionIndex = 0;

  for (const auto &pass : m_pass_nodes) {
    if (pass.isCulled())
      continue;

    std::string label =
      std::to_string(executionIndex++) + ": " + std::string{pass.getName()};
    file << "  exec_" << pass.getId() << " [label=\"" << label << "\"];\n";

    // Connect to previous pass in execution order
    if (prevPass != nullptr) {
      file << "  exec_" << prevPass->getId() << " -> exec_" << pass.getId()
           << " [style=dashed color=gray];\n";
    }
    prevPass = const_cast<PassNode *>(&pass);
  }

  // Draw resource lifetime
  file << "\n  // Resource lifetime\n";
  file << "  node [shape=ellipse, style=filled, fillcolor=lightgreen];\n";

  for (const auto &resource : m_resource_nodes) {
    if (resource.getRefCount() == 0)
      continue;

    std::string label{resource.getName()};
    file << "  res_" << resource.getId() << " [label=\"" << label << "\"];\n";

    // Draw lifetime edges
    if (resource.getProducer()) {
      file << "  exec_" << resource.getProducer()->getId() << " -> res_"
           << resource.getId()
           << " [label=\"create\" color=green fontcolor=green];\n";
    }

    if (resource.getLastConsumer()) {
      file << "  res_" << resource.getId() << " -> exec_"
           << resource.getLastConsumer()->getId()
           << " [label=\"destroy\" color=red fontcolor=red];\n";
    }
  }

  file << "}\n";
  file.close();
}
