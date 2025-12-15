#include "paimon/rendering/shader_manager.h"

#include <algorithm>

#include "paimon/core/io/file.h"
#include "paimon/core/log_system.h"
#include "paimon/opengl/shader_program.h"

using namespace paimon;

ShaderManager& ShaderManager::getInstance() {
  static ShaderManager instance;
  return instance;
}

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
  if (source.empty()) {
    LOG_WARN("Failed to read shader file: {}", filePath.string());
    return;
  }

  // Resolve includes
  source = m_includer.process(source);

  // Store with filename as key
  std::string filename = filePath.filename().string();
  m_shaderSources.emplace(filename, std::move(source));
  LOG_INFO("Loaded shader source: {} ({} bytes)", filename, m_shaderSources[filename].size());
}

std::shared_ptr<ShaderProgram>
ShaderManager::getShaderProgram(const std::string &filename, GLenum type,
                                const std::vector<ShaderDefine> &defines) {
  // Check if source exists
  auto sourceIt = m_shaderSources.find(filename);
  if (sourceIt == m_shaderSources.end()) {
    LOG_ERROR("Shader source not found: {}", filename);
    return nullptr;
  }

  // Get or create cache for this file
  auto &cache = m_programCaches[filename];

  // Get or create shader program from cache
  auto* program = cache.get(sourceIt->second, type, defines);
  if (!program) {
    LOG_ERROR("Failed to create shader program: {}", filename);
    return nullptr;
  }

  // Return as shared_ptr (non-owning)
  return std::shared_ptr<ShaderProgram>(program, [](ShaderProgram*){});
}

void ShaderManager::clear() {
  m_programCaches.clear();
  m_shaderSources.clear();
}
