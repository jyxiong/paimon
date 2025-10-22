#include "paimon/core/fg/frame_graph.h"

#include <stack>

using namespace paimon;

FrameGraph::Builder::Builder(FrameGraph &fg, PassNode &node)
  : m_frameGraph{fg}, m_passNode{node} {}

NodeId FrameGraph::Builder::read(NodeId id, uint32_t flags) {
  m_passNode.read(id, flags);
  m_frameGraph.m_resource_nodes[id].addReader(m_passNode.getId());
  return id;
}

NodeId FrameGraph::Builder::write(NodeId id, uint32_t flags) {
  m_passNode.write(id, flags);
  m_frameGraph.m_resource_nodes[id].addWriter(m_passNode.getId());
  return id;
}

FrameGraphResources::FrameGraphResources(
  const FrameGraph &fg, const PassNode &node
)
  : m_frameGraph{fg}, m_passNode{node} {}

void FrameGraph::compile() {
  // Culling via flood fill from unreferenced resources.
  std::stack<NodeId> unrefResId;
  for (NodeId i = 0; i < m_resource_nodes.size(); ++i) {
    if (m_resource_nodes[i].getRefCount() == 0) {
      unrefResId.push(i);
    }
  }

  while (!unrefResId.empty()) {
    auto resId = unrefResId.top();
    unrefResId.pop();
    auto &resNode = m_resource_nodes[resId];

    // process the create pass of unreferenced resource
    auto &creator = m_pass_nodes[resNode.getCreator()];
    creator.decreaseRef();
    if (creator.getRefCount() == 0 && !creator.isCulled()) {
      for (auto readResId : creator.getReads()) {
        auto &readResNode = m_resource_nodes[readResId];
        readResNode.decreaseRef();
        if (readResNode.getRefCount() == 0 && readResNode.isTransient()) {
          unrefResId.push(readResId);
        }
      }
    }

    // process all write passes of unreferenced resource
    for (auto passId : resNode.getWriters()) {
      auto &writer = m_pass_nodes[passId];
      writer.decreaseRef();
      if (writer.getRefCount() == 0 && !writer.isCulled()) {
        for (auto readResId : writer.getReads()) {
          auto &readResNode = m_resource_nodes[readResId];
          readResNode.decreaseRef();
          if (readResNode.getRefCount() == 0 && readResNode.isTransient()) {
            unrefResId.push(readResId);
          }
        }
      }
    }
  } // while(!unrefResources.empty())

  // Compute execution order via topological sort
  m_execution_order.clear();
  for (auto &passNode : m_pass_nodes) {
    // TODO: check && or ||
    if (passNode.getRefCount() == 0 && passNode.isCulled()) {
      continue;
    }

    PassExecution exec;
    exec.passNode = &passNode;

    // process created resources
    for (auto resId : passNode.getCreates()) {
      auto &resNode = m_resource_nodes[resId];
      exec.created.push_back(&resNode);

      if (resNode.getReaders().empty() && resNode.getWriters().empty()) {
        exec.destroyed.push_back(&resNode);
      }
    }

    // process destroyed resources
    auto readWrites = passNode.getReads();
    readWrites.insert(
      readWrites.end(), passNode.getWrites().begin(), passNode.getWrites().end()
    );
    for (auto resId : readWrites) {
      auto &resNode = m_resource_nodes[resId];
      if (!resNode.isTransient()) {
        continue;
      }

      bool valid = false;
      std::size_t lastId;
      if (!resNode.getReaders().empty()) {
        auto lastReader = std::find_if(
          m_pass_nodes.begin(), m_pass_nodes.end(),
          [&](const PassNode &passNode) {
            return passNode.getId() == resNode.getReaders().back();
          }
        );

        if (lastReader != m_pass_nodes.end()) {
          lastId = static_cast<std::size_t>(std::distance(
            m_pass_nodes.begin(), lastReader
          ));
          valid = true;
        }
      }

      if (!resNode.getWriters().empty()) {
        auto lastWriter = std::find_if(
          m_pass_nodes.begin(), m_pass_nodes.end(),
          [&](const PassNode &passNode) {
            return passNode.getId() == resNode.getWriters().back();
          }
        );

        if (lastWriter != m_pass_nodes.end()) {
          lastId = std::max(lastId, static_cast<std::size_t>(std::distance(
            m_pass_nodes.begin(), lastWriter
          )));
          valid = true;
        }
      }

      if (valid && m_pass_nodes[lastId].getId() == passNode.getId()) {
        exec.destroyed.push_back(&resNode);     
      }
    }

    m_execution_order.emplace_back(std::move(exec));
  }
}

void FrameGraph::execute(void *context) {
  for (auto &exec : m_execution_order) {
    for (auto *resNode : exec.created) {
      resNode->create(context);
    }

    FrameGraphResources resources{*this, *exec.passNode};
    exec.passNode->execute(resources, context);

    for (auto *resNode : exec.destroyed) {
      resNode->destroy(context);
    }
  }
}
