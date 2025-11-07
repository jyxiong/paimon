#include "paimon/asset/image.h"

#include "paimon/core/log_system.h"

namespace paimon {

Image::Image(const AssetMetadata &metadata)
    : Asset(metadata) {
  m_metadata.type = AssetType::Image;
}

bool Image::load() {
  if (m_isLoaded) {
    LOG_WARN("Image already loaded: {}", m_metadata.filePath.string());
    return true;
  }

  // TODO: Implement image loading using stb_image or similar library
  // For now, this is a placeholder
  
  LOG_ERROR("Image::load() not yet implemented for: {}", 
            m_metadata.filePath.string());
  return false;
}

void Image::unload() {
  if (!m_isLoaded) {
    return;
  }

  // TODO: Implement proper cleanup
  if (m_data) {
    // Free image data (implementation depends on the loading library used)
    m_data = nullptr;
  }

  m_width = 0;
  m_height = 0;
  m_channels = 0;
  m_isLoaded = false;
  m_metadata.memorySize = 0;
  
  LOG_INFO("Unloaded image: {}", m_metadata.filePath.string());
}

bool Image::isLoaded() const {
  return m_isLoaded && m_data != nullptr;
}

} // namespace paimon
