#include "paimon/core/io/image.h"

#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

#include "paimon/core/log_system.h"

namespace paimon {

ImageData Image::load(const std::string& filepath, int desired_channels) {
  ImageData result;
  
  int width, height, channels;
  uint8_t* data = stbi_load(filepath.c_str(), &width, &height, &channels, desired_channels);
  
  if (!data) {
    LOG_ERROR("Failed to load image: {} - {}", filepath, stbi_failure_reason());
    return result;
  }
  
  if (desired_channels != 0) {
    channels = desired_channels;
  }
  
  result.data = std::unique_ptr<uint8_t[], void(*)(void*)>(data, stbi_image_free);
  result.width = width;
  result.height = height;
  
  LOG_INFO("Successfully loaded image: {} ({}x{}, {} channels)", filepath, width, height, channels);
  
  return result;
}

} // namespace paimon
