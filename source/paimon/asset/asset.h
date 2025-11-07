#pragma once

#include <filesystem>
#include <memory>
#include <chrono>

namespace paimon {

/**
 * @brief Asset type enumeration
 */
enum class AssetType {
  Unknown,
  Shader,
  Image,
  Model,
  Audio,
  Material,
  // Add more types as needed
};

/**
 * @brief Asset metadata
 */
struct AssetMetadata {
  std::filesystem::path filePath;
  AssetType type = AssetType::Unknown;
  std::chrono::system_clock::time_point lastModified;
  size_t memorySize = 0; // Memory size in bytes
  
  AssetMetadata() = default;
  
  explicit AssetMetadata(const std::filesystem::path &path, AssetType assetType = AssetType::Unknown);
};

/**
 * @brief Base class for all assets
 */
class Asset {
public:
  explicit Asset(const AssetMetadata &metadata);
  
  virtual ~Asset() = default;

  // Non-copyable but movable
  Asset(const Asset &) = delete;
  Asset &operator=(const Asset &) = delete;
  Asset(Asset &&) = default;
  Asset &operator=(Asset &&) = default;

  /**
   * @brief Load the asset from file
   * @return true if successful, false otherwise
   */
  virtual bool load() = 0;

  /**
   * @brief Unload the asset to free memory
   */
  virtual void unload() = 0;

  /**
   * @brief Check if the asset is currently loaded
   */
  virtual bool isLoaded() const = 0;

  /**
   * @brief Reload the asset (useful for hot-reloading)
   * @return true if successful, false otherwise
   */
  virtual bool reload();

  /**
   * @brief Get the asset metadata
   */
  const AssetMetadata &getMetadata() const;

  /**
   * @brief Get the file path of the asset
   */
  const std::filesystem::path &getPath() const;

  /**
   * @brief Get the asset type
   */
  AssetType getType() const;

  /**
   * @brief Get the memory size of the asset
   */
  size_t getMemorySize() const;

protected:
  AssetMetadata m_metadata;
};

/**
 * @brief Shared pointer type for assets
 */
template<typename T>
using AssetPtr = std::shared_ptr<T>;

} // namespace paimon
