#pragma once

#include <format>
#include <string>
#include <type_traits>
#include <vector>

#include <glad/gl.h>

namespace paimon {

struct ShaderSource {
  std::string name;
  std::string source;
  GLenum type;
};

class ShaderDefine {
public:
  ShaderDefine(const std::string &name);

  template<class T>
    requires std::is_arithmetic_v<T>
  ShaderDefine(const std::string &name, const T &value) {
    m_hash = std::hash<std::string>()(name) ^ std::hash<std::string>()(value);
    m_source = std::format("#define {} {} \n", name, value);
  }

  std::size_t getHash() const;

  const std::string &getSource() const;

private:
  std::size_t m_hash;
  std::string m_source;
};

struct ShaderVariant {
  const ShaderSource& source;
  std::vector<ShaderDefine> defines;
};

}