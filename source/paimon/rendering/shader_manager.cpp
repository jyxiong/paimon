#include "paimon/rendering/shader_manager.h"

#include <algorithm>
#include <unordered_map>

#include <glad/gl.h>

#include "paimon/core/io/file.h"
#include "paimon/core/log_system.h"
#include "paimon/rendering/shader_source.h"

using namespace paimon;

ShaderManager::ShaderManager() {}

void ShaderManager::load(const std::filesystem::path &directory) {
  if (!std::filesystem::exists(directory)) {
    LOG_WARN("Shader directory does not exist: {}", directory.string());
    return;
  }

  if (!std::filesystem::is_directory(directory)) {
    LOG_WARN("Path is not a directory: {}", directory.string());
    return;
  }

  auto absDirectory = std::filesystem::absolute(directory);
  LOG_INFO("Loading shaders from: {}", absDirectory.string());

  // Add directory to include paths for shader preprocessing
  m_includer.addSearchPath(absDirectory);

  for (const auto &entry :
        std::filesystem::recursive_directory_iterator(absDirectory)) {
    if (entry.is_regular_file()) {
      loadShaderFile(entry.path());
    }
  }
}

ShaderProgram* ShaderManager::createShaderProgram(const std::string &name,
                                            const std::vector<ShaderDefine>& defines) {
  return m_shaderProgramCache.get(m_shaderSources.at(name), defines);
}

void ShaderManager::loadShaderFile(const std::filesystem::path &filePath) {
  // Check if it's a shader file (common extensions)
  auto extension = filePath.extension().string();
  std::transform(extension.begin(), extension.end(), extension.begin(),
                 ::tolower);

  static const std::vector<std::string> shaderExtensions = {
      ".vert", ".frag", ".geom", ".comp", ".tesc", ".tese", ".glsl"};

  if (std::find(shaderExtensions.begin(), shaderExtensions.end(), extension) ==
      shaderExtensions.end()) {
    return;
  }

  // Read the shader source
  std::string source = File::readText(filePath);

  // Resolve includes
  m_includer.process(source);

  // Determine shader type from extension using a lookup table
  static const std::unordered_map<std::string, GLenum> extToType = {
      {".vert", GL_VERTEX_SHADER},
      {".frag", GL_FRAGMENT_SHADER},
      {".geom", GL_GEOMETRY_SHADER},
      {".comp", GL_COMPUTE_SHADER},
      {".tesc", GL_TESS_CONTROL_SHADER},
      {".tese", GL_TESS_EVALUATION_SHADER},
      {".glsl", GL_INVALID_ENUM} // Generic GLSL, type must be specified later
  };

  auto it = extToType.find(extension);
  GLenum shaderType = (it != extToType.end()) ? it->second : GL_FRAGMENT_SHADER;

  // Store with filename as key
  std::string filename = filePath.filename().string();
  ShaderSource src{filename, std::move(source), shaderType};
  m_shaderSources.emplace(filename, std::move(src));
}
