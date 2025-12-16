#pragma once

#include <filesystem>
#include <set>
#include <string>
#include <vector>

namespace paimon {

/**
 * @brief GLSL Shader Include Processor
 * Resolves #include directives in GLSL shaders.
 */
class ShaderIncluder {
public:
  /**
   * @brief Construct a new Shader Includer
   */
  ShaderIncluder() = default;

  /**
   * @brief Add an include search path
   * @param path Directory to search for included files
   */
  void addSearchPath(const std::filesystem::path &path);

  /**
   * @brief Process shader source and resolve all #include directives
   * @param source Shader source code to process
   * @return Processed source with includes resolved
   */
  void process(std::string &source);

private:
  /**
   * @brief Resolve all #include directives in the source code
   * @param source Source code to process (will be modified in place)
   */
  void resolveIncludes(std::string &source);

private:
  /// Search paths for #include directives
  std::vector<std::filesystem::path> m_searchPaths;

  /// Set of already included files (for include guards)
  std::set<std::filesystem::path> m_includedFiles;

  /// Current processing stack (for detecting circular includes)
  std::vector<std::filesystem::path> m_processingStack;
};

} // namespace paimon
