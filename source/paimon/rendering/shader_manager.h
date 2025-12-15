#pragma once

#include <filesystem>
#include <memory>
#include <string>
#include <unordered_map>
#include <vector>

#include <glad/gl.h>
#include "paimon/rendering/shader_define.h"
#include "paimon/rendering/shader_includer.h"
#include "paimon/rendering/shader_program_cache.h"

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

  /**
   * @brief Get or create a ShaderProgram with given defines
   * @param filename Shader source filename (relative to loaded directories)
   * @param type Shader type (GL_VERTEX_SHADER, GL_FRAGMENT_SHADER, etc.)
   * @param defines Shader defines to apply
   * @return Pointer to cached or newly created ShaderProgram
   */
  std::shared_ptr<ShaderProgram>
  getShaderProgram(const std::string &filename, GLenum type,
                   const std::vector<ShaderDefine> &defines = {});

  /**
   * @brief Clear all cached shader programs
   */
  void clear();

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
  std::unordered_map<std::string, std::string> m_shaderSources;

  /// Cache structure: filename -> ShaderProgramCache
  std::unordered_map<std::string, ShaderProgramCache> m_programCaches;

  /// Shader includer for processing #include directives
  ShaderIncluder m_includer;
};

} // namespace paimon
