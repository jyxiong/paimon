#pragma once

#include <format>
#include <string>
#include <type_traits>

namespace paimon {
class ShaderDefine {
public:
  ShaderDefine(const std::string &name);

  template<class T>
    requires std::is_arithmetic_v<T>
  ShaderDefine(const std::string &name, const T &value) {
    m_name = name;
    m_value = std::to_string(value);

    m_hash = std::hash<std::string>()(name) ^ std::hash<std::string>()(m_value);
    m_source = std::format("#define {} {} \n", name, m_value);
  }

  std::size_t getHash() const;

  const std::string &getSource() const;

private:
  std::string m_name;
  std::string m_value;

  std::size_t m_hash;
  std::string m_source;
};

}