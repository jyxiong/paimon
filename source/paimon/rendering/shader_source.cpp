#include "paimon/rendering/shader_source.h"

using namespace paimon;

ShaderDefine::ShaderDefine(const std::string &name) {
  m_hash = std::hash<std::string>()(name);
  m_source = std::format("#define {} \n", name);
}

std::size_t ShaderDefine::getHash() const {
  return m_hash;
}

const std::string &ShaderDefine::getSource() const {
  return m_source;
}