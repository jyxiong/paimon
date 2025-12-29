#pragma once

#include <filesystem>
#include <string>
#include <unordered_map>

#include "paimon/rendering/shader_source.h"
#include "paimon/rendering/shader_includer.h"
#include "paimon/rendering/shader_program_cache.h"

namespace paimon {

class ShaderProgram;

/**
 * @brief Shader Manager
 * Manages shader source files and creates/caches ShaderProgram instances
 * Should be managed by Application instance
 */
class ShaderManager {
public:
  /**
   * @brief Construct a new Shader Manager
   */
  ShaderManager();

  // Delete copy constructor and assignment operator
  ShaderManager(const ShaderManager&) = delete;
  ShaderManager& operator=(const ShaderManager&) = delete;

  /**
   * @brief Load all shader files from a directory
   * @param directory Directory containing shader files
   */
  void load(const std::filesystem::path &directory);

  /**
   * @brief Create or get cached shader program with defines
   * @param source Shader source
   * @param defines Shader defines/macros
   * @return Pointer to shader program
   */
  ShaderProgram* createShaderProgram(const std::string &name,
                                     const std::vector<ShaderDefine>& defines = {});

private:

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

  /// Cache for compiled shader programs with different defines
  ShaderProgramCache m_shaderProgramCache;
};

} // namespace paimon
