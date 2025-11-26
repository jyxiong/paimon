#pragma once

#include <cstddef>
#include <unordered_map>
#include <utility>

#include "paimon/rendering/shader_variant.h"

namespace paimon {
class ShaderTemplate {
public:
  ShaderTemplate(const std::string& source);

  // Compute hash for a set of defines
  static std::size_t getVariantHash(const std::vector<ShaderDefine>& defines);

  // Create or get a variant, returns pair of (hash, variant)
  std::pair<std::size_t, const ShaderVariant&> createVariant(const std::vector<ShaderDefine>& defines);

  const std::string& getSource() const;

private:
  std::string m_source;

  std::unordered_map<std::size_t, ShaderVariant> m_variants;
};
}