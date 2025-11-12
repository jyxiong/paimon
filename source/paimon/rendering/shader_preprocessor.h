#pragma once

#include <filesystem>
#include <set>
#include <string>
#include <vector>

namespace paimon {

class ShaderSource;

/**
 * @brief GLSL Shader Preprocessor
 * Implements a C++-like #include preprocessor for GLSL shaders.
 */
class ShaderPreprocessor {
public:
  /**
   * @brief Construct a new Shader Preprocessor
   */
  ShaderPreprocessor();

  /**
   * @brief Add an include search path
   * @param path Directory to search for included files
   */
  void addIncludePath(const std::filesystem::path &path);

  /**
   * @brief Set multiple include search paths
   * @param paths Vector of directories to search
   */
  void setIncludePaths(const std::vector<std::filesystem::path> &paths);

  /**
   * @brief Process a ShaderSource object with defines and includes
   * @param shaderSource ShaderSource object containing source and defines
   * @return Processed shader source with defines and includes resolved
   */
  std::string processShaderSource(const ShaderSource &shaderSource);

  /**
   * @brief Clear the processed files cache (for include guards)
   * Call this between processing different shader programs
   */
  void clearCache();

private:
  /**
   * @brief Resolve #define directives from a list and insert them into source
   * @param source Original shader source code (will be modified in place)
   * @param defines List of define directives to insert
   */
  void resolveDefines(std::string &source,
                      const std::vector<std::string> &defines) const;

  /**
   * @brief Resolve all #include directives in the source code
   * @param source Source code to process (will be modified in place)
   */
  void resolveIncludes(std::string &source);

private:
  /// Search paths for #include <> directives
  std::vector<std::filesystem::path> m_includePaths;

  /// Set of already included files (for include guards)
  std::set<std::filesystem::path> m_includedFiles;

  /// Current processing stack (for detecting circular includes)
  std::vector<std::filesystem::path> m_processingStack;
};

} // namespace paimon
