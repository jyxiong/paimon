#pragma once

#include <string>
#include <vector>

#include "paimon/rendering/shader_define.h"

namespace paimon {

class ShaderTemplate;

class ShaderVariant {
public:
  ShaderVariant(const ShaderTemplate& shaderTemplate, const std::vector<ShaderDefine> &defines);

  const std::string& getSource() const;

private:
  const ShaderTemplate& m_template;

  std::vector<ShaderDefine> m_defines;

  std::string m_source;
};

}