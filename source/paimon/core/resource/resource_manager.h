#pragma once

#include <filesystem>
#include <string>
#include <unordered_map>

namespace paimon {

/**
 * @brief Central resource management system for the engine
 * 
 * Provides centralized path resolution and resource loading.
 * Supports multiple search paths and resource caching.
 */
class ResourceManager {
public:
  /**
   * @brief Resource type categories
   */
  enum class ResourceType {
    Shader,
    Texture,
    Model,
    Config,
    Other
  };

  /**
   * @brief Get the singleton instance
   */
  static ResourceManager& getInstance();

  /**
   * @brief Initialize the resource manager with the application root path
   * @param appPath Path to the application executable or project root
   */
  void initialize(const std::filesystem::path& appPath);

  /**
   * @brief Add a custom search path for resources
   * @param path Additional search path
   */
  void addSearchPath(const std::filesystem::path& path);

  /**
   * @brief Set the asset root directory (default: "asset")
   * @param assetRoot Relative or absolute path to asset directory
   */
  void setAssetRoot(const std::filesystem::path& assetRoot);

  /**
   * @brief Resolve a resource path to an absolute path
   * @param relativePath Relative path from asset root (e.g., "shader/vertex/basic.vert")
   * @param type Resource type for category-specific search
   * @return Absolute path if found, empty path otherwise
   */
  std::filesystem::path resolve(
    const std::string& relativePath,
    ResourceType type = ResourceType::Other
  ) const;

  /**
   * @brief Check if a resource exists
   * @param relativePath Relative path from asset root
   * @param type Resource type
   * @return true if resource exists
   */
  bool exists(
    const std::string& relativePath,
    ResourceType type = ResourceType::Other
  ) const;

  /**
   * @brief Get the absolute path to the asset root directory
   */
  const std::filesystem::path& getAssetRoot() const;

  /**
   * @brief Get the shader directory path
   */
  std::filesystem::path getShaderPath() const;

  /**
   * @brief Get the texture directory path
   */
  std::filesystem::path getTexturePath() const;

  /**
   * @brief Get the model directory path
   */
  std::filesystem::path getModelPath() const;

  /**
   * @brief Enable/disable resource path caching for performance
   */
  void setCachingEnabled(bool enabled);

private:
  ResourceManager() = default;
  ~ResourceManager() = default;
  ResourceManager(const ResourceManager&) = delete;
  ResourceManager& operator=(const ResourceManager&) = delete;

  /**
   * @brief Try to find resource in all search paths
   */
  std::filesystem::path findInSearchPaths(const std::string& relativePath) const;

  /**
   * @brief Get subdirectory for resource type
   */
  std::string getSubdirectoryForType(ResourceType type) const;

private:
  std::filesystem::path m_assetRoot;
  std::filesystem::path m_appRoot;
  std::vector<std::filesystem::path> m_searchPaths;
  
  // Optional caching for resolved paths
  mutable std::unordered_map<std::string, std::filesystem::path> m_pathCache;
  bool m_cachingEnabled = true;
};

} // namespace paimon
