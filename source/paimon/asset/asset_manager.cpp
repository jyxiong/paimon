#include "paimon/asset/asset_manager.h"

#include <algorithm>

#include "paimon/core/log_system.h"

namespace paimon {

AssetManager::AssetManager() = default;

AssetManager::~AssetManager() {
  unloadAll();
}

AssetManager &AssetManager::getInstance() {
  static AssetManager instance;
  return instance;
}

void AssetManager::setMaxCacheSize(size_t sizeBytes) {
  std::lock_guard<std::mutex> lock(m_mutex);
  m_maxCacheSize = sizeBytes;
  
  // Evict if current size exceeds new limit
  if (m_currentCacheSize > m_maxCacheSize) {
    evictLRU(m_currentCacheSize - m_maxCacheSize);
  }
}

size_t AssetManager::getCurrentCacheSize() const {
  std::lock_guard<std::mutex> lock(m_mutex);
  return m_currentCacheSize;
}

void AssetManager::unload(const std::filesystem::path &path) {
  std::lock_guard<std::mutex> lock(m_mutex);
  
  const auto key = pathToKey(path);
  auto it = m_cache.find(key);
  
  if (it != m_cache.end()) {
    // Get asset size before erasing
    auto metaIt = m_metadataCache.find(key);
    if (metaIt != m_metadataCache.end()) {
      m_currentCacheSize -= metaIt->second.memorySize;
      m_metadataCache.erase(metaIt);
    }
    
    m_cache.erase(it);
    m_lastAccess.erase(key);
    
    LOG_INFO("Unloaded asset: {}", path.string());
  }
}

void AssetManager::unloadByType(AssetType type) {
  std::lock_guard<std::mutex> lock(m_mutex);
  
  std::vector<std::string> toRemove;
  
  for (const auto &[key, metadata] : m_metadataCache) {
    if (metadata.type == type) {
      toRemove.push_back(key);
    }
  }
  
  for (const auto &key : toRemove) {
    m_currentCacheSize -= m_metadataCache[key].memorySize;
    m_cache.erase(key);
    m_metadataCache.erase(key);
    m_lastAccess.erase(key);
  }
  
  LOG_INFO("Unloaded {} assets of type {}", toRemove.size(), static_cast<int>(type));
}

void AssetManager::unloadAll() {
  std::lock_guard<std::mutex> lock(m_mutex);
  
  const size_t count = m_cache.size();
  
  m_cache.clear();
  m_metadataCache.clear();
  m_lastAccess.clear();
  m_currentCacheSize = 0;
  
  if (count > 0) {
    LOG_INFO("Unloaded all {} cached assets", count);
  }
}

bool AssetManager::reload(const std::filesystem::path &path) {
  std::lock_guard<std::mutex> lock(m_mutex);
  
  const auto key = pathToKey(path);
  auto it = m_cache.find(key);
  
  if (it != m_cache.end()) {
    auto asset = std::static_pointer_cast<Asset>(it->second);
    const bool success = asset->reload();
    
    if (success) {
      m_lastAccess[key] = std::chrono::system_clock::now();
      
      // Update metadata
      auto metaIt = m_metadataCache.find(key);
      if (metaIt != m_metadataCache.end()) {
        m_currentCacheSize -= metaIt->second.memorySize;
        metaIt->second.memorySize = asset->getMemorySize();
        m_currentCacheSize += asset->getMemorySize();
      }
      
      LOG_INFO("Reloaded asset: {}", path.string());
    } else {
      LOG_ERROR("Failed to reload asset: {}", path.string());
    }
    
    return success;
  }
  
  LOG_WARN("Cannot reload asset that is not cached: {}", path.string());
  return false;
}

bool AssetManager::isCached(const std::filesystem::path &path) const {
  std::lock_guard<std::mutex> lock(m_mutex);
  const auto key = pathToKey(path);
  return m_cache.find(key) != m_cache.end();
}

AssetManager::CacheStats AssetManager::getCacheStats() const {
  std::lock_guard<std::mutex> lock(m_mutex);
  
  CacheStats stats;
  stats.totalAssets = m_cache.size();
  stats.totalMemoryUsed = m_currentCacheSize;
  stats.maxMemory = m_maxCacheSize;
  
  for (const auto &[key, metadata] : m_metadataCache) {
    stats.assetCountByType[metadata.type]++;
  }
  
  return stats;
}

void AssetManager::evictLRU(size_t requiredSpace) {
  if (m_cache.empty()) {
    return;
  }
  
  // Build list of (key, last_access_time) pairs
  std::vector<std::pair<std::string, std::chrono::system_clock::time_point>> entries;
  entries.reserve(m_cache.size());
  
  for (const auto &[key, time] : m_lastAccess) {
    entries.emplace_back(key, time);
  }
  
  // Sort by access time (oldest first)
  std::sort(entries.begin(), entries.end(), 
    [](const auto &a, const auto &b) { return a.second < b.second; });
  
  size_t freedSpace = 0;
  size_t evictedCount = 0;
  
  for (const auto &[key, time] : entries) {
    if (freedSpace >= requiredSpace) {
      break;
    }
    
    auto metaIt = m_metadataCache.find(key);
    if (metaIt != m_metadataCache.end()) {
      freedSpace += metaIt->second.memorySize;
      m_currentCacheSize -= metaIt->second.memorySize;
      m_metadataCache.erase(metaIt);
    }
    
    m_cache.erase(key);
    m_lastAccess.erase(key);
    evictedCount++;
  }
  
  LOG_INFO("Evicted {} assets to free {} bytes", evictedCount, freedSpace);
}

std::string AssetManager::pathToKey(const std::filesystem::path &path) const {
  // Normalize the path to ensure consistent keys
  return std::filesystem::absolute(path).string();
}

AssetType AssetManager::inferAssetType(const std::filesystem::path &path) const {
  const auto ext = path.extension().string();
  
  // Shader extensions
  if (ext == ".vert" || ext == ".frag" || ext == ".geom" || 
      ext == ".comp" || ext == ".tesc" || ext == ".tese" ||
      ext == ".glsl" || ext == ".shader") {
    return AssetType::Shader;
  }
  
  // Image/Texture extensions
  if (ext == ".png" || ext == ".jpg" || ext == ".jpeg" || 
      ext == ".bmp" || ext == ".tga" || ext == ".hdr") {
    return AssetType::Image;
  }
  
  // Model extensions
  if (ext == ".obj" || ext == ".fbx" || ext == ".gltf" || 
      ext == ".glb" || ext == ".dae") {
    return AssetType::Model;
  }
  
  // Audio extensions
  if (ext == ".wav" || ext == ".mp3" || ext == ".ogg") {
    return AssetType::Audio;
  }
  
  return AssetType::Unknown;
}

} // namespace paimon
