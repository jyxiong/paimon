#pragma once

#include <string>

namespace paimon {

using NodeId = std::size_t;

class GraphNode {
public:
  GraphNode(const std::string_view name, NodeId id);

  GraphNode() = delete;
  GraphNode(const GraphNode &) = delete;
  GraphNode(GraphNode &&) noexcept = default;
  virtual ~GraphNode() = default;

  GraphNode &operator=(const GraphNode &) = delete;
  GraphNode &operator=(GraphNode &&) noexcept = delete;

  NodeId getId() const;
  std::string_view getName() const;
  std::size_t getRefCount() const;

  void setRefCount(std::size_t count);

  std::size_t increaseRef();
  std::size_t decreaseRef();

private:
  std::string m_name;
  NodeId m_id;
  std::size_t m_refCount{0};
};

} // namespace paimon