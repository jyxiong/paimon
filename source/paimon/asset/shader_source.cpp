#include "paimon/asset/shader_source.h"

#include "paimon/core/io/file.h"
#include "paimon/core/log_system.h"

namespace paimon {

ShaderSource::ShaderSource(const std::filesystem::path &filePath)
    : Asset(AssetMetadata(filePath, AssetType::Shader)) {
  m_metadata.type = AssetType::Shader;
}

ShaderSource::ShaderSource(const AssetMetadata &metadata)
    : Asset(metadata) {
  m_metadata.type = AssetType::Shader;
}

bool ShaderSource::load() {
  if (m_isLoaded) {
    LOG_WARN("Shader already loaded: {}", m_metadata.filePath.string());
    return true;
  }

  try {
    m_source = File::readText(m_metadata.filePath);
    m_isLoaded = true;
    updateMemorySize();
    
    LOG_INFO("Loaded shader: {}", m_metadata.filePath.string());
    return true;
  } catch (const std::exception &e) {
    LOG_ERROR("Failed to load shader {}: {}", 
              m_metadata.filePath.string(), e.what());
    return false;
  }
}

void ShaderSource::unload() {
  if (!m_isLoaded) {
    return;
  }

  m_source.clear();
  m_defines.clear();
  m_isLoaded = false;
  m_metadata.memorySize = 0;
  
  LOG_INFO("Unloaded shader: {}", m_metadata.filePath.string());
}

bool ShaderSource::isLoaded() const {
  return m_isLoaded;
}

void ShaderSource::define(const std::string &define) {
  m_defines.push_back(define);
  updateMemorySize();
}

void ShaderSource::clearDefines() {
  m_defines.clear();
  updateMemorySize();
}

void ShaderSource::updateMemorySize() {
  // Calculate approximate memory usage
  size_t size = 0;
  
  // Source code
  size += m_source.size();
  
  // Defines
  for (const auto &define : m_defines) {
    size += define.size();
  }
  
  // Add overhead for containers and strings
  size += sizeof(ShaderSource);
  size += m_defines.capacity() * sizeof(std::string);
  
  m_metadata.memorySize = size;
}

} // namespace paimon
