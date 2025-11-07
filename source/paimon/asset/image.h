#pragma once

#include "paimon/asset/asset.h"

namespace paimon {

/**
 * @brief Image asset for loading and managing image data
 * 
 * This is a placeholder for future implementation.
 * Will support formats: PNG, JPG, BMP, TGA, HDR, etc.
 * 
 * Planned features:
 * - Image loading from various formats
 * - Mipmap generation
 * - Image compression
 * - CPU-side image manipulation
 * - Conversion between formats
 */
class Image : public Asset {
public:
  explicit Image(const AssetMetadata &metadata);
  ~Image() override = default;

  bool load() override;
  void unload() override;
  bool isLoaded() const override;

  /**
   * @brief Get image width in pixels
   */
  int getWidth() const { return m_width; }

  /**
   * @brief Get image height in pixels
   */
  int getHeight() const { return m_height; }

  /**
   * @brief Get number of channels (1=grayscale, 3=RGB, 4=RGBA)
   */
  int getChannels() const { return m_channels; }

  /**
   * @brief Get raw pixel data
   */
  const unsigned char *getData() const { return m_data; }

  /**
   * @brief Get pixel data (mutable)
   */
  unsigned char *getData() { return m_data; }

private:
  unsigned char *m_data = nullptr;
  int m_width = 0;
  int m_height = 0;
  int m_channels = 0;
  bool m_isLoaded = false;
};

} // namespace paimon
