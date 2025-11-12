#pragma once

#include <filesystem>
#include <format>
#include <string>
#include <vector>

namespace paimon {
class ShaderSource {
public:
  ShaderSource(const std::filesystem::path &filePath);

  void define(const std::string &define);

  template <class T>
  void define(const std::string &key, const T &value) {
    define(std::format("{} {}", key, value));
  }

  const std::string &getSource() const { return m_source; }
  const std::vector<std::string> &getDefines() const { return m_defines; }

private:
  std::string m_source;
  std::vector<std::string> m_defines;
};
} // namespace paimon