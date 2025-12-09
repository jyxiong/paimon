#pragma once

#include <memory>
#include <unordered_map>

#include "paimon/opengl/state/vertex_input.h"
#include "paimon/opengl/vertex_array.h"

namespace paimon {

// VertexArrayCache manages vertex array objects based on vertex input configurations
class VertexArrayCache {
public:
  VertexArrayCache() = default;
  ~VertexArrayCache() = default;

  // Delete copy constructor and assignment
  VertexArrayCache(const VertexArrayCache&) = delete;
  VertexArrayCache& operator=(const VertexArrayCache&) = delete;

  // Get or create a vertex array based on VertexInputState
  VertexArray* get(const VertexInputState& state);

  // Clear the cache
  void clear();

  // Get cache statistics
  size_t getCacheSize() const { return m_cache.size(); }

private:
  // Compute hash from VertexInputState
  std::size_t createHash(const VertexInputState& state) const;

  // Create a new vertex array from VertexInputState
  std::unique_ptr<VertexArray> createVertexArray(const VertexInputState& state) const;

private:
  // Cache uses hash value as key
  std::unordered_map<std::size_t, std::unique_ptr<VertexArray>> m_cache;
};

} // namespace paimon
