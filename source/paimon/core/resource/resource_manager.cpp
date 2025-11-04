#include "paimon/core/resource/resource_manager.h"

#include "paimon/core/log_system.h"

namespace paimon {

ResourceManager& ResourceManager::getInstance() {
  static ResourceManager instance;
  return instance;
}

void ResourceManager::initialize(const std::filesystem::path& appPath) {
  m_appRoot = std::filesystem::absolute(appPath);
  
  // Default asset root is "asset" relative to app root
  if (m_assetRoot.empty()) {
    m_assetRoot = m_appRoot / "asset";
  }
  
  // Add common search paths
  m_searchPaths.push_back(m_assetRoot);
  m_searchPaths.push_back(m_appRoot);
  
  LOG_INFO("ResourceManager initialized");
  LOG_INFO("  App root: {}", m_appRoot.string());
  LOG_INFO("  Asset root: {}", m_assetRoot.string());
}

void ResourceManager::addSearchPath(const std::filesystem::path& path) {
  std::filesystem::path absolutePath = std::filesystem::absolute(path);
  
  // Check if path already exists in search paths
  for (const auto& existing : m_searchPaths) {
    if (existing == absolutePath) {
      return;
    }
  }
  
  m_searchPaths.push_back(absolutePath);
  LOG_DEBUG("Added search path: {}", absolutePath.string());
}

void ResourceManager::setAssetRoot(const std::filesystem::path& assetRoot) {
  m_assetRoot = std::filesystem::absolute(assetRoot);
  
  // Update search paths
  m_searchPaths.clear();
  m_searchPaths.push_back(m_assetRoot);
  m_searchPaths.push_back(m_appRoot);
  
  // Clear cache as paths have changed
  m_pathCache.clear();
  
  LOG_INFO("Asset root set to: {}", m_assetRoot.string());
}

std::filesystem::path ResourceManager::resolve(
  const std::string& relativePath,
  ResourceType type
) const {
  // Check cache first
  if (m_cachingEnabled) {
    auto it = m_pathCache.find(relativePath);
    if (it != m_pathCache.end()) {
      return it->second;
    }
  }
  
  std::filesystem::path resolvedPath;
  
  // Try with type-specific subdirectory first
  std::string typeSubdir = getSubdirectoryForType(type);
  if (!typeSubdir.empty()) {
    std::filesystem::path typeSpecificPath = typeSubdir + "/" + relativePath;
    resolvedPath = findInSearchPaths(typeSpecificPath.string());
  }
  
  // If not found, try direct path
  if (resolvedPath.empty()) {
    resolvedPath = findInSearchPaths(relativePath);
  }
  
  // Cache the result
  if (m_cachingEnabled && !resolvedPath.empty()) {
    m_pathCache[relativePath] = resolvedPath;
  }
  
  if (resolvedPath.empty()) {
    LOG_WARN("Failed to resolve resource: {}", relativePath);
  }
  
  return resolvedPath;
}

bool ResourceManager::exists(
  const std::string& relativePath,
  ResourceType type
) const {
  return !resolve(relativePath, type).empty();
}

const std::filesystem::path& ResourceManager::getAssetRoot() const {
  return m_assetRoot;
}

std::filesystem::path ResourceManager::getShaderPath() const {
  return m_assetRoot / "shader";
}

std::filesystem::path ResourceManager::getTexturePath() const {
  return m_assetRoot / "texture";
}

std::filesystem::path ResourceManager::getModelPath() const {
  return m_assetRoot / "model";
}

void ResourceManager::setCachingEnabled(bool enabled) {
  m_cachingEnabled = enabled;
  if (!enabled) {
    m_pathCache.clear();
  }
}

std::filesystem::path ResourceManager::findInSearchPaths(
  const std::string& relativePath
) const {
  for (const auto& searchPath : m_searchPaths) {
    std::filesystem::path fullPath = searchPath / relativePath;
    
    std::error_code ec;
    if (std::filesystem::exists(fullPath, ec) && !ec) {
      return std::filesystem::canonical(fullPath, ec);
    }
  }
  
  return {};
}

std::string ResourceManager::getSubdirectoryForType(ResourceType type) const {
  switch (type) {
    case ResourceType::Shader:
      return "shader";
    case ResourceType::Texture:
      return "texture";
    case ResourceType::Model:
      return "model";
    case ResourceType::Config:
      return "config";
    case ResourceType::Other:
    default:
      return "";
  }
}

} // namespace paimon
