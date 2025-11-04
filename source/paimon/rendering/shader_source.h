#pragma once

#include <filesystem>
#include <string>
#include <unordered_map>
#include <unordered_set>
#include <vector>

namespace paimon {
class ShaderSource {
public:
  ShaderSource(const std::filesystem::path &filepath);

  void addDefine(const std::string &define);

  template <class T>
  void addDefine(const std::string &key, const T &value) {
    addDefine(std::format("{} {}", key, value));
  }

  void replace(const std::string &search, const std::string &replace);

  // Set include search paths for resolving #include directives
  void setIncludePaths(const std::vector<std::filesystem::path> &paths);

  // Get the processed shader source with all includes expanded
  std::string getSource() const;

  // Get the original unprocessed source
  const std::string &getOriginalSource() const;

  // Get all files that were included during processing
  const std::unordered_set<std::filesystem::path> &getIncludedFiles() const;

  // Get the base directory of the shader file
  const std::filesystem::path &getBaseDirectory() const;

private:
  // Process the shader source with includes
  void processIncludes(
    std::string &source,
    std::unordered_set<std::filesystem::path> &includedFiles
  ) const;

  // Resolve include file path (supports both "file" and <file> syntax)
  std::filesystem::path resolveIncludePath(
    const std::string &includePath,
    bool isSystemInclude
  ) const;

private:
  std::string m_source;
  std::filesystem::path m_baseDirectory;
  std::vector<std::filesystem::path> m_includePaths;
  std::vector<std::string> m_defines;
  std::unordered_map<std::string, std::string> m_replacements;
  mutable std::unordered_set<std::filesystem::path> m_includedFiles;
};
} // namespace paimon