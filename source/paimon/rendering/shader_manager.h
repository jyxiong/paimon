#pragma once

#include <filesystem>
#include <memory>
#include <string>
#include <unordered_map>
#include <vector>

#include <glad/gl.h>
#include "paimon/rendering/shader_template.h"
#include "paimon/rendering/shader_define.h"
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
  /// Map of shader filename to shader template
  std::unordered_map<std::string, ShaderTemplate> m_shaderTemplates;

  /// Cache structure: (filename, hash) -> ShaderProgram
  std::unordered_map<std::string, std::unordered_map<std::size_t, std::shared_ptr<ShaderProgram>>> m_shaderCache;

  /// Shader includer for processing #include directives
  ShaderIncluder m_includer;
};

} // namespace paimon
