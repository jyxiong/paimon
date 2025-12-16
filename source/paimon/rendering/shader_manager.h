#pragma once

#include <filesystem>
#include <string>
#include <unordered_map>

#include <glad/gl.h>
#include "paimon/rendering/shader_source.h"
#include "paimon/rendering/shader_includer.h"

namespace paimon {

class ShaderProgram;

/**
 * @brief Shader Manager
 * Manages shader source files and creates/caches ShaderProgram instances
 * Singleton pattern for global shader management
 */
class ShaderManager {
public:
  /**
   * @brief Get the singleton instance
   * @return Reference to the ShaderManager instance
   */
  static ShaderManager& getInstance();

  // Delete copy constructor and assignment operator
  ShaderManager(const ShaderManager&) = delete;
  ShaderManager& operator=(const ShaderManager&) = delete;

  /**
   * @brief Load all shader files from a directory
   * @param directory Directory containing shader files
   */
  void load(const std::filesystem::path &directory);

  const ShaderSource& getShaderSource(const std::string &name) const;

private:
  /**
   * @brief Construct a new Shader Manager (private for singleton)
   */
  ShaderManager();

  /**
   * @brief Load a single shader file
   * @param filePath Path to the shader file
   */
  void loadShaderFile(const std::filesystem::path &filePath);

private:
  /// Map of shader filename to base shader source
  std::unordered_map<std::string, ShaderSource> m_shaderSources;

  /// Shader includer for processing #include directives
  ShaderIncluder m_includer;
};

} // namespace paimon
