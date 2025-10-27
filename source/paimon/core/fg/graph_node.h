#pragma once

#include <string>

namespace paimon {

using NodeId = std::size_t;

class GraphNode {
public:
  GraphNode(const std::string_view name, NodeId id) : m_name{name}, m_id{id} {}

  GraphNode() = delete;
  GraphNode(const GraphNode &) = delete;
  GraphNode(GraphNode &&) noexcept = default;
  virtual ~GraphNode() = default;

  GraphNode &operator=(const GraphNode &) = delete;
  GraphNode &operator=(GraphNode &&) noexcept = delete;

  [[nodiscard]] auto getId() const { return m_id; }
  [[nodiscard]] std::string_view getName() const { return m_name; }
  [[nodiscard]] auto getRefCount() const { return m_refCount; }

  void setRefCount(std::size_t count) { m_refCount = count; }

  std::size_t increaseRef() { 
    ++m_refCount; 
    return m_refCount;
  }
  std::size_t decreaseRef() {
    if (m_refCount > 0) {
      --m_refCount;
    }
    return m_refCount;
  }

private:
  std::string m_name;
  NodeId m_id;
  std::size_t m_refCount{0};
};

} // namespace paimon