#include "paimon/rendering/vertex_array_cache.h"

#include "paimon/core/hash.h"
#include "paimon/core/log_system.h"

namespace paimon {

VertexArray* VertexArrayCache::get(const VertexInputState& state) {
  // Compute hash from VertexInputState
  std::size_t hash = createHash(state);

  // Check if vertex array already exists in cache
  auto it = m_cache.find(hash);
  if (it != m_cache.end()) {
    return it->second.get();
  }

  // Create new vertex array
  auto vertexArray = createVertexArray(state);
  if (!vertexArray) {
    LOG_ERROR("Failed to create vertex array");
    return nullptr;
  }

  // Check if vertex array is valid
  if (!vertexArray->is_valid()) {
    LOG_ERROR("Vertex array is not valid");
    return nullptr;
  }

  VertexArray* ptr = vertexArray.get();
  m_cache[hash] = std::move(vertexArray);

  LOG_INFO("Created and cached new vertex array with hash {} (cache size: {})", 
           hash, m_cache.size());

  return ptr;
}

void VertexArrayCache::clear() {
  m_cache.clear();
  LOG_INFO("Vertex array cache cleared");
}

std::size_t VertexArrayCache::createHash(const VertexInputState& state) const {
  std::size_t hash = 0;

  // Hash bindings
  for (const auto& binding : state.bindings) {
    hashCombine(hash, 
                binding.binding, 
                binding.stride, 
                binding.divisor);
  }

  // Hash attributes
  for (const auto& attribute : state.attributes) {
    hashCombine(hash,
                attribute.location,
                attribute.binding,
                attribute.format,
                attribute.size,
                attribute.offset,
                attribute.normalized);
  }

  return hash;
}

std::unique_ptr<VertexArray> VertexArrayCache::createVertexArray(
    const VertexInputState& state) const {
  auto vertexArray = std::make_unique<VertexArray>();

  // Configure attributes
  for (const auto& attribute : state.attributes) {
    vertexArray->enable_attribute(attribute.location);
    vertexArray->set_attribute_format(attribute.location,
                                      attribute.size,
                                      attribute.format,
                                      attribute.normalized ? GL_TRUE : GL_FALSE,
                                      attribute.offset);
    vertexArray->set_attribute_binding(attribute.location, attribute.binding);
  }

  // Configure bindings (divisors)
  for (const auto& binding : state.bindings) {
    if (binding.divisor > 0) {
      vertexArray->set_binding_divisor(binding.binding, binding.divisor);
    }
  }

  return vertexArray;
}

} // namespace paimon
