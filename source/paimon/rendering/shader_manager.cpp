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

  // Resolve includes to create the template
  source = m_includer.process(source);

  // Store with filename as key
  std::string filename = filePath.filename().string();
  m_shaderTemplates.emplace(filename, ShaderTemplate(source));
  LOG_INFO("Loaded shader template: {} ({} bytes)", filename, source.size());
}

std::shared_ptr<ShaderProgram>
ShaderManager::getShaderProgram(const std::string &filename, GLenum type,
                                const std::vector<ShaderDefine> &defines) {
  // Check if template exists
  auto templateIt = m_shaderTemplates.find(filename);
  if (templateIt == m_shaderTemplates.end()) {
    LOG_ERROR("Shader template not found: {}", filename);
    return nullptr;
  }

  // Create variant from template (computes hash once)
  auto [hash, variant] = templateIt->second.createVariant(defines);

  // Check cache using the computed hash
  auto &fileCache = m_shaderCache[filename];
  auto cacheIt = fileCache.find(hash);
  if (cacheIt != fileCache.end()) {
    LOG_DEBUG("Using cached shader program: {}", filename);
    return cacheIt->second;
  }

  // Get processed source from variant
  const std::string& processedSource = variant.getSource();

  // Create shader program
  auto shaderProgram = std::make_shared<ShaderProgram>(type, processedSource);

  // Check for compilation errors
  if (!shaderProgram->is_valid()) {
    LOG_ERROR("Failed to create shader program: {}", filename);
    std::string infoLog = shaderProgram->get_info_log();
    if (!infoLog.empty()) {
      LOG_ERROR("Shader compilation error:\n{}", infoLog);
    }
    return nullptr;
  }

  // Check for warnings/info
  std::string infoLog = shaderProgram->get_info_log();
  if (!infoLog.empty()) {
    LOG_INFO("Shader program info log for {}:\n{}", filename, infoLog);
  }

  // Cache and return using the pre-computed hash
  fileCache[hash] = shaderProgram;
  LOG_INFO("Created and cached shader program: {}", filename);

  return shaderProgram;
}

void ShaderManager::clear() {
  m_shaderCache.clear();
  m_shaderTemplates.clear();
}
