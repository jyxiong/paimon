#include "paimon/rendering/shader_template.h"

using namespace paimon;

ShaderTemplate::ShaderTemplate(const std::string& source)
    : m_source(source) {}

std::size_t ShaderTemplate::getVariantHash(const std::vector<ShaderDefine>& defines) {
    std::size_t hash = 0;
    for (const auto& define : defines) {
        hash ^= define.getHash();
    }
    return hash;
}

std::pair<std::size_t, const ShaderVariant&> ShaderTemplate::createVariant(const std::vector<ShaderDefine>& defines) {
    // Compute hash for the defines
    std::size_t hash = getVariantHash(defines);

    // Check if variant already exists
    auto it = m_variants.find(hash);
    if (it != m_variants.end()) {
        return {hash, it->second};
    }

    // Create new variant and store it
    auto [newIt, inserted] = m_variants.emplace(hash, ShaderVariant(*this, defines));
    return {hash, newIt->second};
}

const std::string& ShaderTemplate::getSource() const {
    return m_source;
}
