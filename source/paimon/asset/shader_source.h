#pragma once

#include <filesystem>
#include <format>
#include <string>
#include <vector>

#include "paimon/asset/asset.h"

namespace paimon {

/**
 * @brief Shader source with asset management and preprocessing
 * 
 * This class provides:
 * - Automatic loading from file
 * - Shader preprocessing with defines
 * - Memory tracking
 * - Hot-reloading support
 * - Asset management integration
 */
class ShaderSource : public Asset {
public:
  explicit ShaderSource(const std::filesystem::path &filePath);
  explicit ShaderSource(const AssetMetadata &metadata);
  ~ShaderSource() override = default;

  /**
   * @brief Load the shader from file
   * @return true if successful
   */
  bool load() override;

  /**
   * @brief Unload the shader to free memory
   */
  void unload() override;

  /**
   * @brief Check if the shader is currently loaded
   */
  bool isLoaded() const override;

  /**
   * @brief Add a preprocessor define
   * @param define Define directive (e.g., "USE_LIGHTING")
   */
  void define(const std::string &define);

  /**
   * @brief Add a preprocessor define with value
   * @tparam T Value type
   * @param key Define key (e.g., "MAX_LIGHTS")
   * @param value Define value (e.g., 8)
   */
  template<typename T>
  void define(const std::string &key, const T &value) {
    define(std::format("{} {}", key, value));
  }

  /**
   * @brief Get the raw shader source code
   */
  const std::string &getSource() const { return m_source; }

  /**
   * @brief Get the list of defines
   */
  const std::vector<std::string> &getDefines() const { return m_defines; }

  /**
   * @brief Clear all defines
   */
  void clearDefines();

private:
  void updateMemorySize();

private:
  std::string m_source;
  std::vector<std::string> m_defines;
  bool m_isLoaded = false;
};

} // namespace paimon
