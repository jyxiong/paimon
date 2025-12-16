#pragma once

#include <memory>
#include <unordered_map>

#include "paimon/opengl/shader_program.h"
#include "paimon/rendering/shader_source.h"

namespace paimon {

// ShaderProgramCache manages shader program objects for a single shader source
// with different define combinations
class ShaderProgramCache {
public:
  ShaderProgramCache() = default;
  ~ShaderProgramCache() = default;

  // Delete copy constructor and assignment operator
  ShaderProgramCache(const ShaderProgramCache &) = delete;
  ShaderProgramCache &operator=(const ShaderProgramCache &) = delete;

  // Get or create shader program with given defines
  ShaderProgram *get(const ShaderSource &source,
                     const std::vector<ShaderDefine> &defines);

  // Clear the cache
  void clear();

  // Get cache statistics
  size_t getCacheSize() const { return m_cache.size(); }

private:
  // Compute hash from defines
  std::size_t createHash(const ShaderSource &source,
                     const std::vector<ShaderDefine> &defines) const;

  // Create a new shader program
  std::unique_ptr<ShaderProgram>
  createShaderProgram(const ShaderSource &source,
                     const std::vector<ShaderDefine> &defines) const;

private:
  // Cache uses hash value as key
  std::unordered_map<std::size_t, std::unique_ptr<ShaderProgram>> m_cache;
};

} // namespace paimon