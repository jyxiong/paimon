#include "paimon/core/io/image.h"

#include <stb_image.h>

#include "paimon/core/log_system.h"

namespace paimon {

sg::Image Image::load(const std::filesystem::path &filepath,
                      int desired_channels) {
  sg::Image result;

  int width, height, channels;
  uint8_t *data = stbi_load(filepath.string().c_str(), &width, &height,
                            &channels, desired_channels);

  if (!data) {
    // LOG_ERROR("Failed to load image: {} - {}", filepath,
    // stbi_failure_reason());
    return result;
  }

  if (desired_channels != 0) {
    channels = desired_channels;
  }

  result.data =
      std::vector<unsigned char>(data, data + (width * height * channels));
  result.width = width;
  result.height = height;
  result.components = channels;
  result.bits = 8;

  // LOG_INFO("Successfully loaded image: {} ({}x{}, {} channels)", filepath,
  // width, height, channels);

  stbi_image_free(data);

  return result;
}

} // namespace paimon
