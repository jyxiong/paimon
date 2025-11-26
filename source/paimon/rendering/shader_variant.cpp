#include "paimon/rendering/shader_variant.h"

#include <regex>

#include "paimon/rendering/shader_template.h"

using namespace paimon;

ShaderVariant::ShaderVariant(const ShaderTemplate &shaderTemplate,
                             const std::vector<ShaderDefine> &defines)
    : m_template(shaderTemplate), m_defines(defines),
      m_source(shaderTemplate.getSource()) {

  std::string defineBlock;
  for (const auto &define : defines) {
    defineBlock += define.getSource();
  }

  static const std::regex versionPattern(R"(^[ \t]*#version\s+\d+[^\n]*)");
  std::smatch match;

  if (std::regex_search(m_source, match, versionPattern)) {
    // Insert defines after #version line
    size_t insertPos = match.position() + match.length();
    // Find the end of the line
    size_t newlinePos = m_source.find('\n', insertPos);
    if (newlinePos != std::string::npos) {
      m_source.insert(newlinePos + 1, defineBlock);
    } else {
      m_source += "\n" + defineBlock;
    }
  } else {
    // No #version, insert at beginning
    m_source.insert(0, defineBlock + "\n");
  }
}

const std::string& ShaderVariant::getSource() const {
  return m_source;
}
