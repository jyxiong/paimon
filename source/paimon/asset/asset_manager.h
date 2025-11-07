#pragma once

#include <filesystem>
#include <functional>
#include <memory>
#include <mutex>
#include <string>
#include <unordered_map>

#include "paimon/asset/asset.h"

namespace paimon {

/**
 * @brief Asset cache entry with reference counting and usage tracking
 */
template<typename T>
struct AssetCacheEntry {
  AssetPtr<T> asset;
  size_t refCount = 0;
  std::chrono::system_clock::time_point lastAccessed;

  AssetCacheEntry(AssetPtr<T> ptr) 
      : asset(std::move(ptr))
      , lastAccessed(std::chrono::system_clock::now()) {}
};

/**
 * @brief Asset manager with caching and resource management
 * 
 * Features:
 * - Automatic caching with LRU eviction policy
 * - Reference counting to prevent premature unloading
 * - Configurable memory limits
 * - Hot-reloading support
 * - Thread-safe operations
 */
class AssetManager {
public:
  /**
   * @brief Get the singleton instance
   */
  static AssetManager &getInstance();

  /**
   * @brief Set the maximum cache memory size in bytes
   * Default is 512MB
   */
  void setMaxCacheSize(size_t sizeBytes);

  /**
   * @brief Get the current cache memory size
   */
  size_t getCurrentCacheSize() const;

  /**
   * @brief Load an asset with the specified type
   * @tparam T Asset type derived from Asset
   * @param path File path to the asset
   * @param forceReload Force reload even if cached
   * @return Shared pointer to the loaded asset, or nullptr on failure
   */
  template<typename T>
  AssetPtr<T> load(const std::filesystem::path &path, bool forceReload = false);

  /**
   * @brief Unload a specific asset from cache
   * @param path File path to the asset
   */
  void unload(const std::filesystem::path &path);

  /**
   * @brief Unload all assets of a specific type
   */
  void unloadByType(AssetType type);

  /**
   * @brief Unload all cached assets
   */
  void unloadAll();

  /**
   * @brief Reload an asset (useful for hot-reloading)
   * @param path File path to the asset
   * @return true if successful
   */
  bool reload(const std::filesystem::path &path);

  /**
   * @brief Check if an asset is currently cached
   */
  bool isCached(const std::filesystem::path &path) const;

  /**
   * @brief Get cache statistics
   */
  struct CacheStats {
    size_t totalAssets = 0;
    size_t totalMemoryUsed = 0;
    size_t maxMemory = 0;
    std::unordered_map<AssetType, size_t> assetCountByType;
  };
  
  CacheStats getCacheStats() const;

  /**
   * @brief Register a custom asset factory
   * @tparam T Asset type derived from Asset
   * @param type Asset type enum
   * @param factory Factory function to create the asset
   */
  template<typename T>
  void registerAssetFactory(
    AssetType type,
    std::function<std::unique_ptr<T>(const std::filesystem::path &)> factory
  );

private:
  AssetManager();
  ~AssetManager();

  // Delete copy and move constructors
  AssetManager(const AssetManager &) = delete;
  AssetManager &operator=(const AssetManager &) = delete;
  AssetManager(AssetManager &&) = delete;
  AssetManager &operator=(AssetManager &&) = delete;

  /**
   * @brief Evict least recently used assets to free memory
   * @param requiredSpace Memory space needed in bytes
   */
  void evictLRU(size_t requiredSpace);

  /**
   * @brief Compute hash for file path
   */
  std::string pathToKey(const std::filesystem::path &path) const;

  /**
   * @brief Infer asset type from file extension
   */
  AssetType inferAssetType(const std::filesystem::path &path) const;

private:
  mutable std::mutex m_mutex;
  
  // Asset cache: path -> asset
  std::unordered_map<std::string, std::shared_ptr<void>> m_cache;
  
  // Asset metadata cache: path -> metadata
  std::unordered_map<std::string, AssetMetadata> m_metadataCache;
  
  // Access tracking for LRU
  std::unordered_map<std::string, std::chrono::system_clock::time_point> m_lastAccess;
  
  // Asset factories
  std::unordered_map<AssetType, 
    std::function<std::shared_ptr<Asset>(const std::filesystem::path &)>> m_factories;
  
  size_t m_maxCacheSize = 512 * 1024 * 1024; // 512MB default
  size_t m_currentCacheSize = 0;
};

// Template implementation
template<typename T>
AssetPtr<T> AssetManager::load(const std::filesystem::path &path, bool forceReload) {
  std::lock_guard<std::mutex> lock(m_mutex);
  
  const auto key = pathToKey(path);
  
  // Check if already cached
  if (!forceReload && m_cache.find(key) != m_cache.end()) {
    m_lastAccess[key] = std::chrono::system_clock::now();
    return std::static_pointer_cast<T>(m_cache[key]);
  }
  
  // Infer asset type if not registered
  auto metadata = AssetMetadata(path, inferAssetType(path));
  
  // Create new asset
  auto asset = std::make_shared<T>(metadata);
  
  // Load the asset
  if (!asset->load()) {
    return nullptr;
  }
  
  // Check if we need to evict
  const size_t assetSize = asset->getMemorySize();
  if (m_currentCacheSize + assetSize > m_maxCacheSize) {
    evictLRU(assetSize);
  }
  
  // Cache the asset
  m_cache[key] = asset;
  m_metadataCache[key] = metadata;
  m_lastAccess[key] = std::chrono::system_clock::now();
  m_currentCacheSize += assetSize;
  
  return asset;
}

template<typename T>
void AssetManager::registerAssetFactory(
  AssetType type,
  std::function<std::unique_ptr<T>(const std::filesystem::path &)> factory
) {
  std::lock_guard<std::mutex> lock(m_mutex);
  m_factories[type] = [factory](const std::filesystem::path &path) -> std::shared_ptr<Asset> {
    return factory(path);
  };
}

} // namespace paimon
